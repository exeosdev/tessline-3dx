
#ifndef __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__

#include "hwBuffer.h"

namespace ts3
{

	class HWGeometryBuffer : public HWBuffer
	{
	public:
		EGeometryDataType const mGeometryDataType;

		explicit HWGeometryBuffer( EGeometryDataType pGeometryDataType );
		virtual ~HWGeometryBuffer();
	};

	class HWVertexBuffer : public HWGeometryBuffer
	{
	public:
		const HWVertexBufferMetrics & mVBMetrics;

		HWVertexBuffer( const HWVertexBufferMetrics & pVBMetrics );
		virtual ~HWVertexBuffer();

		void setBufferMetrics( const HWVertexBufferMetrics & pVBMetrics );

	private:
		HWVertexBufferMetrics _vbMetrics;
	};

	class HWIndexBuffer : public HWGeometryBuffer
	{
	public:
		const HWIndexBufferMetrics & mIBMetrics;

		HWIndexBuffer();
		virtual ~HWIndexBuffer();

		void setBufferMetrics( const HWIndexBufferMetrics & pIBMetrics );

	private:
		HWIndexBufferMetrics _ibMetrics;
	};

} // namespace ts3

#endif // __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__
