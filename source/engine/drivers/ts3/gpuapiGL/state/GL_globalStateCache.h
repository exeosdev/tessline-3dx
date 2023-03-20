
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__

#include "../state/GL_commonGraphicsConfig.h"
#include "../state/GL_inputAssembler.h"

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

		BlendConfig blendConfig;
		GLDepthStencilConfig depthStencilConfig;
		GLRasterizerConfig rasterizerConfig;
		SamplerBindings samplerBindings;
		TextureUnitBindings textureUnitBindings;
		GLuint vertexArrayObjectBinding;
		GLIAInputLayoutDefinition vertexArrayObjectInputLayout;
		GLIAVertexStreamDefinition vertexArrayObjectVertexStream;
		GLuint shaderPipelineBinding;
		GLuint shaderProgramBinding;
	};

	class GLGlobalStateCache
	{
	public:
		static const GLGlobalState sDefaultState;

	public:
		GLGlobalStateCache();
		~GLGlobalStateCache() = default;

		void reset();

		void applyBlendState( const GLBlendConfig & pBlendConfig );
		void applyDepthStencilState( const GLDepthStencilConfig & pDepthStencilConfig );
		void applyRasterizerState( const GLRasterizerConfig & pRasterizerConfig );

		void applyShaderPipelineBinding( GLuint pShaderPipelineHandle );
		void applyShaderProgramBinding( GLuint pShaderProgramHandle );

		static GLGlobalState getDefaultGlobalState();

	private:
		GLGlobalState _cachedState;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GLOBAL_STATE_CACHE_H__
