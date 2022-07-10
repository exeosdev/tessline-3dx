
#ifndef __TS3_CORE_RECT_ALLOCATOR_H__
#define __TS3_CORE_RECT_ALLOCATOR_H__

#include "../mathImports.h"

namespace ts3
{

	struct RectAllocatorConfig
	{
		struct HorizontalLayout
		{
			uint32 hLineAdvanceThreshold = Limits<uint32>::maxValue;
			uint32 hSpacing = 0;
		};
		struct VerticalLayout
		{
			uint32 baseLineHeight;
			uint32 vSpacing = 0;
		};
		HorizontalLayout horizontalLayout;
		VerticalLayout verticalLayout;
	};

	class RectAllocator
	{
	public:
		RectAllocator( const math::Vec2u32 & pBoundingRectDimensions, const RectAllocatorConfig & pAllocatorConfig );
		~RectAllocator();

		bool addRect( const math::Vec2u32 & pRect, math::Vec2u32 * pOutPosition );

		void updateHorizontalLayout( RectAllocatorConfig::HorizontalLayout & pHorizontalLayout );
		void updateVerticalLayout( RectAllocatorConfig::VerticalLayout & pVerticalLayout );

		void reset();

		TS3_PCL_ATTR_NO_DISCARD bool checkFreeSpace( const math::Vec2u32 & pRect ) const;
        TS3_PCL_ATTR_NO_DISCARD bool empty() const;

        TS3_PCL_ATTR_NO_DISCARD const RectAllocatorConfig & getConfig() const
		{
			return _config;
		}

	private:
		const math::Vec2u32 _boundingRectDimensions;
		RectAllocatorConfig _config;
		math::Vec2u32       _allocPointerOffset;
		uint32              _rectCounter;
	};
	
}

#endif // __TS3_CORE_RECT_ALLOCATOR_H__
