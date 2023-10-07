
#ifndef __TS3_ENGINE_MATERIAL_COMMON_H__
#define __TS3_ENGINE_MATERIAL_COMMON_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/resources/textureCommon.h>

namespace ts3
{

	enum EMaterialTextureID: uint32
	{
		Albedo,
		DiffuseMap,
		NormalMap,
		RoughnessMap,
		_Reserved
	};

	namespace cxdefs
	{

		constexpr uint32 MATERIAL_MAX_TEXTURES_NUM = static_cast<uint32>( EMaterialTextureID::_Reserved );

	}

	using MaterialTextureArray = std::array<gpuapi::TextureHandle, cxdefs::MATERIAL_MAX_TEXTURES_NUM>;

	struct MaterialParameters
	{
	};

	struct MaterialTextureSet
	{
	private:
		MaterialTextureArray _textureArray;

	public:
		const MaterialTextureArray & commonShaderArray;

		gpuapi::TextureHandle & albedoTexture;
		
		gpuapi::TextureHandle & diffuseMap;
		
		gpuapi::TextureHandle & normalMap;
		
		gpuapi::TextureHandle & roughnessMap;

	public:
		MaterialTextureSet();

		MaterialTextureSet( const MaterialTextureSet & pSource );

		explicit MaterialTextureSet( const MaterialTextureArray & pTextureArray );

		MaterialTextureSet & operator=( const MaterialTextureSet & pRhs );

		MaterialTextureSet & operator=( const MaterialTextureArray & pRhs );

		TS3_ATTR_NO_DISCARD gpuapi::Texture * operator[]( size_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD gpuapi::Texture * operator[]( EMaterialTextureID pTextureID ) const noexcept;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;
	};

} // namespace ts3

#endif // __TS3_ENGINE_MATERIAL_COMMON_H__
