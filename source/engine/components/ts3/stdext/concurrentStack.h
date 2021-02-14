
#ifndef __TS3_STDEXT_CONCURRENT_STACK_H__
#define __TS3_STDEXT_CONCURRENT_STACK_H__

#include "common.h"
#include <atomic>


namespace stdx
{


	template <class _T>
	struct concurrent_stack_node
	{
	public:
		using my_type = concurrent_stack_node<_T>;
		using value_type = _T;

	public:
		my_type* next;

		uint32_t control_tag;

		value_type value;

	public:
		concurrent_stack_node()
			: next( nullptr )
		{}

		concurrent_stack_node( value_type&& value )
			: next( nullptr )
			, value( std::move( value ) )
		{}

		concurrent_stack_node( const value_type& value )
			: next( nullptr )
			, value( value )
		{}
	};


	template <class _T>
	struct concurrent_stack_value_wrapper
	{
	public:
		using my_type = concurrent_stack_value_wrapper<_T>;
		using value_type = _T;

	public:
		bool result;

		value_type value;

	public:
		concurrent_stack_value_wrapper( bool result = false )
			: result( result )
		{}

		explicit operator bool() const
		{
			return this->result;
		}

		value_type& operator*() const
		{
			return this->value;
		}

		value_type* operator->() const
		{
			return &( this->value );
		}
	};


	template <class _T>
	using concurrent_stack_alloc_type = concurrent_stack_node<_T>;


	template <class _T, class _Allocator = std::allocator< concurrent_stack_alloc_type<_T> > >
	class concurrent_stack
	{
	public:
		using my_type = concurrent_stack<_T>;
		using node_type = concurrent_stack_node<_T>;
		using allocator_type = _Allocator;
		using value_type = _T;
		using value_wrapper = concurrent_stack_value_wrapper<_T>;

	public:
		concurrent_stack()
			: _top( ATOMIC_VAR_INIT( nullptr ) )
			, _retained_node_list( ATOMIC_VAR_INIT( nullptr ) )
			, _control_tag_counter( ATOMIC_VAR_INIT( 0 ) )
			, _pop_counter( ATOMIC_VAR_INIT( 0 ) )
			, _size( ATOMIC_VAR_INIT( 0 ) )
		{}

		concurrent_stack( allocator_type allocator )
			: concurrent_stack()
			, _allocator( std::move( allocator ) )
		{}

		~concurrent_stack()
		{
			this->clear();
		}

		value_type& top()
		{
			return this->_top.load( std::memory_order_consume )->value;
		}

		const value_type& top() const
		{
			return this->_top.load( std::memory_order_consume )->value;
		}

		template <class... _Args>
		value_type* emplace( _Args&& ...args )
		{
			node_type* node = this->_create_node( std::forward<_Args>( args )... );
			this->_push( node );

			return &( node->value );
		}

		value_type* push()
		{
			node_type* node = this->_create_node();
			this->_push( node );

			return &( node->value );
		}

		value_type* push( value_type&& value )
		{
			node_type* node = this->_create_node( std::move( value ) );
			this->_push( node );

			return &( node->value );
		}

		value_type* push( const value_type& value )
		{
			node_type* node = this->_create_node( value );
			this->_push( node );

			return &( node->value );
		}

		void pop()
		{
			this->_pop();
		}

		bool fetch( value_type* dest )
		{
			return this->_pop( dest );
		}

		value_wrapper fetch()
		{
			value_wrapper wrapper;
			wrapper.result = this->_pop( &( wrapper.value ) );
			return wrapper;
		}

		void clear()
		{
			if ( node_type* top_node = this->_top.exchange( nullptr, std::memory_order_acq_rel ) )
			{
				while ( top_node != nullptr )
				{
					node_type* next_node = top_node->next;
					this->_destroy_node( top_node );
					top_node = next_node;
				}
			}
		}

		Size_t size() const
		{
			return this->_size.load( std::memory_order_relaxed );
		}

		bool empty() const
		{
			return this->_top.load( std::memory_order_acquire ) == nullptr;
		}

	private:
		// Allocates new node using internal allocator.
		node_type* _alloc_node()
		{
			return this->_allocator.allocate( 1 );
		}

		// Frees node by actually releasing occupied memory.
		void _free_node( node_type* node )
		{
			this->_allocator.deallocate( node, 1 );
		}

		// Freed the whole list of nodes, starting from specified node (the beginning of the list).
		void _free_node_list( node_type* node )
		{
			while ( node != nullptr )
			{
				node_type* next_node = node->next;
				this->_free_node( node );

				node = next_node;
			}
		}

		// Creates new node. Allocates memory from the allocator and constructs stored value object using args.
		template <class... _Args>
		node_type* _create_node( _Args&&... args )
		{
			return new ( this->_alloc_node() ) node_type( value_type( std::forward<_Args>( args )... ) );
		}

		// Destroys node by calling its destructor (and, thus, guaranteeing, that value member is destroyed
		// when node is removed from the stack) but does not frees the node itself. Freed nodes are added
		// to the internal release list and freed when it is guaranteed, that the are no longer used (@_Pop()).
		void _destroy_node( node_type* node )
		{
			node->~node_type();
			this->_add_retained_node( node );
		}

		// Adds node to the list of nodes to be released.
		void _add_retained_node( node_type* node )
		{
			node_type* top_retained_node = this->_retained_node_list.load( std::memory_order_relaxed );

			while ( true )
			{
				node->next = top_retained_node;

				// Note: ABA problem is not an issue here, since node is being reclaimed
				// (is has been removed from the stack) and no other thread could acsess
				// it in any way (it is neither on the stack, nor on the release list).

				if ( this->_retained_node_list.compare_exchange_weak( top_retained_node, node, std::memory_order_acq_rel, std::memory_order_relaxed ) )
				{
					break;
				}
			}
		}

		// Tries to reclaim retained nodes and free them. Nodes can be reclaimed if there is no thread
		// inside the protected section of the _Pop() function at the time this function is called.
		void _try_reclaim_reatined_nodes()
		{
			node_type* top_retained_node = this->_retained_node_list.load( std::memory_order_relaxed );

			while ( top_retained_node != nullptr )
			{
				if ( this->_pop_counter.load( std::memory_order_relaxed ) != 0 )
				{
					// If any thread is currently inside the protected section of _Pop() method, we cannot
					// free retained nodes. This prevents from freeing the node which may be still in use.
					break;
				}

				if ( this->_retained_node_list.compare_exchange_weak( top_retained_node, nullptr, std::memory_order_acq_rel, std::memory_order_relaxed ) )
				{
					this->_free_node_list( top_retained_node );
					break;
				}

			}
		}

		// Pushes specified node on top of the stack.
		void _push( node_type* node )
		{
			node_type* top_node = this->_top.load( std::memory_order_relaxed );

			while ( true )
			{
				node->next = top_node;
				node->control_tag = this->_control_tag_counter.fetch_add( 1, std::memory_order_relaxed );

				if ( this->_top.compare_exchange_weak( top_node, node, std::memory_order_acq_rel, std::memory_order_relaxed ) )
				{
					this->_size.fetch_add( 1, std::memory_order_relaxed );
					
					break;
				}
			}
		}

		// Pops value from the top of the stack. If pop was successful and 'dest' is not NULL, it saves
		// popped value at the address pointed to be 'dest'. Returns value indicating if pop was successful.
		bool _pop( value_type* dest = nullptr )
		{
			bool result = false;

			// Increment pop counter. Its value correspond to the number of threads which are inside following code section
			// (we call it "protected pop section"). When pop counter is greater than zero, removed nodes are not freed,
			// but instead they are placed on an adiitional list ("retained node list"). This guarantees that, even if
			// top node is removed between load and cmpxchg, access to ::next and ::control_tag will be valid.
			this->_pop_counter.fetch_add( 1, std::memory_order_relaxed );

			// Get current pointer to the top of the stack.
			if ( node_type* top_node = this->_top.load( std::memory_order_consume ) )
			{
				//
				auto control_tag = top_node->control_tag;

				// Nullptr --> empty stack, nothing to pop.
				while ( top_node != nullptr )
				{
					// Check if stack was not modified from the time of the first load. If not, atomically set top of the stack to next node.
					if ( ( top_node->control_tag == control_tag ) && this->_top.compare_exchange_weak( top_node, top_node->next, std::memory_order_acq_rel, std::memory_order_consume ) )
					{
						if ( dest != nullptr )
						{
							*dest = std::move( top_node->value );
						}

						// Destroy the node. It only calls the destructor of the stored value, without freeing the actual node.
						// Then, it adds the node to the retained list to ensure other threads can read its content in case of
						// concurrent calls to Pop(). Retained nodes are freed when pop counter drops to zero.
						this->_destroy_node( top_node );

						this->_size.fetch_sub( 1, std::memory_order_relaxed );

						result = true;

						break;
					}
					else if ( top_node != nullptr )
					{
						control_tag = top_node->control_tag;
					}
				}
			}

			this->_pop_counter.fetch_sub( 1, std::memory_order_relaxed );

			// Try to reclaim retained nodes after leaving protected section. This will guarantee,
			// that nodes are freed as soon as the last thread finishes removing the element.
			this->_try_reclaim_reatined_nodes();

			return result;
		}

	private:
		// Memory proxy object, responsible for allocation/deallocation and other, memory-related functionality.
		allocator_type _allocator;
		// Pointer to the top of the stack.
		std::atomic<node_type*> _top;
		// List of nodes removed from the stack.
		std::atomic<node_type*> _retained_node_list;
		// Counter used to generate control tags.
		std::atomic<Uint32> _control_tag_counter;
		// Stores number of threads which are currently inside _Pop() method. Used to determine if rel nodes can be freed (_pop_counter == 0).
		std::atomic<Uint32> _pop_counter;
		//
		std::atomic<Size_t> _size;
	};


}


#endif /* __TS3_STDEXT_CONCURRENT_STACK_H__ */
