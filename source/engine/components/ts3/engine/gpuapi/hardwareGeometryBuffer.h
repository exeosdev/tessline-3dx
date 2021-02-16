
#ifndef __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__

#include "hardwareBuffer.h"

namespace ts3
{

	class HardwareGeometryBuffer : public HardwareBuffer
	{
	public:
		EGeometryDataType const mGeometryDataType;

		explicit HardwareGeometryBuffer( EGeometryDataType pGeometryDataType );
		virtual ~HardwareGeometryBuffer();
	};

	class HardwareVertexBuffer : public HardwareGeometryBuffer
	{
	public:
		const & VertexBufferMetrics mVBMetrics;

		HardwareVertexBuffer( const VertexBufferMetrics & pVBMetrics );
		virtual ~HardwareVertexBuffer();

		void setBufferMetrics( const VertexBufferMetrics & pVBMetrics );

	private:
		VertexBufferMetrics _vbMetrics;
	};

	class HardwareIndexBuffer : public HardwareGeometryBuffer
	{
	public:
		const & IndexBufferMetrics mIBMetrics;

		HardwareIndexBuffer();
		virtual ~HardwareIndexBuffer();

		void setBufferMetrics( const IndexBufferMetrics & pIBMetrics );

	private:
		IndexBufferMetrics _ibMetrics;
	};

}

#endif // __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__
