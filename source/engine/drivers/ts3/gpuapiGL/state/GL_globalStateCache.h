
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__

#include "GL_commonGraphicsConfig.h"
#include "GL_inputAssembler.h"

namespace ts3::gpuapi
{

	struct GLGlobalState
	{
		struct BlendConfig : public GLBlendConfig
		{
			uint32 blendActiveGlobal;
		};

		struct TextureUnitBinding
		{
			GLenum textureType;
			GLuint textureHandle;
		};

		using SamplerBindings = std::array<GLuint, gpm::RES_MAX_TEXTURE_UNITS_NUM>;
		using TextureUnitBindings = std::array<TextureUnitBinding, gpm::RES_MAX_TEXTURE_UNITS_NUM>;

		GLuint shaderPipelineBinding;
		GLuint shaderProgramBinding;
		GLuint vertexArrayObjectBinding;
		GLuint indexBufferBinding;

		BlendConfig blendConfig;
		GLDepthStencilConfig depthStencilConfig;
		GLRasterizerConfig rasterizerConfig;
		SamplerBindings samplerBindings;
		TextureUnitBindings textureUnitBindings;
	};

	class GLGlobalStateCache
	{
	public:
		static const GLGlobalState sDefaultState;

	public:
		GLGlobalStateCache();
		~GLGlobalStateCache() = default;

		void reset();

		void applyShaderPipelineBinding( GLuint pShaderPipelineHandle );
		void applyShaderProgramBinding( GLuint pShaderProgramHandle );
		void applyIndexBufferBinding( GLuint pIndexBufferObjectHandle );
		void applyVertexArrayObjectBinding( GLuint pVertexArrayObjectHandle );

		void applyBlendState( const GLBlendConfig & pBlendConfig );
		void applyDepthStencilState( const GLDepthStencilConfig & pDepthStencilConfig, uint8 pStencilRefValue );
		void applyRasterizerState( const GLRasterizerConfig & pRasterizerConfig );

		static GLGlobalState getDefaultGlobalState();

	private:
		GLGlobalState _cachedState;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__
