
#include "hardwareGeometryBuffer.h"

namespace ts3
{

	HardwareGeometryBuffer::HardwareGeometryBuffer( EGeometryDataType pGeometryDataType )
	: mGeometryDataType( pGeometryDataType )
	{}

	HardwareGeometryBuffer::~HardwareGeometryBuffer() = default;


	HardwareVertexBuffer::HardwareVertexBuffer()
	: HardwareGeometryBuffer( EGeometryDataType::VertexData )
	{}

	HardwareVertexBuffer::~HardwareVertexBuffer() = default;


	HardwareIndexBuffer::HardwareIndexBuffer()
	: HardwareGeometryBuffer( EGeometryDataType::IndexData )
	{}

	HardwareIndexBuffer::~HardwareIndexBuffer() = default;

}
