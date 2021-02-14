
#include "rectAllocator.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{

	RectAllocator::RectAllocator( const math::Vec2u32 & pBoundingRectDimensions, const RectAllocatorConfig & pAllocatorConfig )
	: _boundingRectDimensions( pBoundingRectDimensions )
	, _config( pAllocatorConfig )
	, _allocPointerOffset( 0, 0 )
	, _rectCounter( 0 )
	{}

	RectAllocator::~RectAllocator()
	{}

	bool RectAllocator::addRect( const math::Vec2u32 & pRect, math::Vec2u32 * pOutPosition )
	{
		// Spacing also serves to ensure, that glyph images won't be located
		// too close to the border itself. Hence we add it twice at this point.
		auto targetWidth = _config.horizontalLayout.hSpacing + pRect.x + _config.horizontalLayout.hSpacing;
		auto targetHeight = _config.verticalLayout.vSpacing + pRect.y + _config.verticalLayout.vSpacing;

		// If the rect is larger than the layer itself (including padding), just ignore the request.
		if( ( targetWidth > _boundingRectDimensions.x ) || ( pRect.y > _config.verticalLayout.baseLineHeight ) )
		{
			return false;
		}

		// Rect fits into the layer assuming the layer is empty.
		// Check now if the remaining space can actually hold it.
		uint32 horizontalSpaceLeft = _boundingRectDimensions.x - _allocPointerOffset.x;

		bool moveToNextLine = false;

		if( horizontalSpaceLeft < targetWidth )
		{
			// Current line may not have enough space, but we can still check the next one.
			// Vertical offset always moves by _baseLineHeight + vertical spacing, so we need
			// at least that much vertical space for the rect to be inserted.
			uint32 allocOffsetYNextLine = _allocPointerOffset.y + _config.verticalLayout.vSpacing + _config.verticalLayout.baseLineHeight;

			if( allocOffsetYNextLine + targetHeight > _boundingRectDimensions.y )
			{
				return false;
			}

			// Apparently we have enough vertical space to add another line of
			// rectangles and insert our rect there. Move offsets accordingly.
			moveToNextLine = true;
		}

		if( pOutPosition )
		{
			if( moveToNextLine )
			{
				_allocPointerOffset.y += ( _config.verticalLayout.vSpacing + _config.verticalLayout.baseLineHeight );
				_allocPointerOffset.x = 0;
			}

			// Write the insert position. Of course the client is interested only in the geometry
			// of the glyph itself, so the position must be already shifted by the padding values.
			pOutPosition->x = _allocPointerOffset.x + _config.horizontalLayout.hSpacing;
			pOutPosition->y = _allocPointerOffset.y + _config.verticalLayout.vSpacing;

			// ...but the alloc offset is moved by the total width with both paddings included.
			_allocPointerOffset.x += ( _config.horizontalLayout.hSpacing + pRect.x );

			// Increment the number of sub-rectangles we allocated space for.
			++_rectCounter;
		}

		return true;
	}

	void RectAllocator::updateHorizontalLayout( RectAllocatorConfig::HorizontalLayout & pHorizontalLayout )
	{
		if( pHorizontalLayout.hSpacing < _config.horizontalLayout.hSpacing )
		{
			auto hSpacingDiff = _config.horizontalLayout.hSpacing - pHorizontalLayout.hSpacing;
			_allocPointerOffset.x = getMinOf( _allocPointerOffset.x + hSpacingDiff, _boundingRectDimensions.x );
		}

		_config.horizontalLayout = pHorizontalLayout;
	}

	void RectAllocator::updateVerticalLayout( RectAllocatorConfig::VerticalLayout & pVerticalLayout )
	{
		auto vMoveOffset = getMaxOf( _config.verticalLayout.vSpacing, pVerticalLayout.vSpacing );
		_allocPointerOffset.y = getMinOf( _allocPointerOffset.y + vMoveOffset, _boundingRectDimensions.y );
		_allocPointerOffset.x = 0;
	}

	void RectAllocator::reset()
	{
		_allocPointerOffset.x = 0;
		_allocPointerOffset.y = 0;
		_rectCounter = 0;
	}

	bool RectAllocator::checkFreeSpace( const math::Vec2u32 & pRect ) const
	{
		auto targetWidth = _config.horizontalLayout.hSpacing + pRect.x + _config.horizontalLayout.hSpacing;
		auto targetHeight = _config.verticalLayout.vSpacing + pRect.y + _config.verticalLayout.vSpacing;

		if( ( targetWidth > _boundingRectDimensions.x ) || ( pRect.y > _config.verticalLayout.baseLineHeight ) )
		{
			return false;
		}

		uint32 horizontalSpaceLeft = _boundingRectDimensions.x - _allocPointerOffset.x;
		if( ( horizontalSpaceLeft < targetWidth ) )
		{
			uint32 allocOffsetYNextLine = _allocPointerOffset.y + _config.verticalLayout.vSpacing + _config.verticalLayout.baseLineHeight;
			if( allocOffsetYNextLine + targetHeight > _boundingRectDimensions.y )
			{
				return false;
			}
		}

		return true;
	}

	bool RectAllocator::isEmpty() const
	{
		return _rectCounter == 0;
	}

}
