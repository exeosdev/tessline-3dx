
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
		HardwareVertexBuffer();
		virtual ~HardwareVertexBuffer();
	};

	class HardwareIndexBuffer : public HardwareGeometryBuffer
	{
	public:
		HardwareIndexBuffer();
		virtual ~HardwareIndexBuffer();
	};

}

#endif // __TS3_ENGINE_HARDWARE_GEOMETRY_BUFFER_H__
