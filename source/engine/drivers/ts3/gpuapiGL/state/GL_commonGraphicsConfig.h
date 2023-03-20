
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/commonGraphicsConfigImmutableStates.h>

namespace ts3::gpuapi
{

	struct GLRTColorAttachmentBlendSettings
	{
		struct Equation
		{
			GLenum rgb;
			GLenum alpha;
		};

		struct Factor
		{
			GLenum rgbSrc;
			GLenum rgbDst;
			GLenum alphaSrc;
			GLenum alphaDst;
		};

		uint32 blendActive;
		Equation equation;
		Factor factor;

		static constexpr auto sEquationDataSize = sizeof( Equation );
		static constexpr auto sFactorDataSize = sizeof( Factor );
	};

	struct GLBlendConfig
	{
		Bitmask<ERTAttachmentFlags> attachmentsMask;
		Bitmask<EBlendConfigFlags> flags;
		GLRTColorAttachmentBlendSettings attachments[gpm::RT_MAX_COLOR_ATTACHMENTS_NUM];
		math::RGBAColorR32Norm constantColor;
	};

	struct GLDepthStencilConfig
	{
		struct GLDepthSettings
		{
			GLenum depthCompFunc;
			GLuint writeMask;
		};

		struct GLStencilFaceDesc
		{
			GLenum compFunc;
			GLint  refValue;
			GLuint readMask;
			GLenum opFail;
			GLenum opPassDepthFail;
			GLenum opPassDepthPass;
			GLuint writeMask;
		};

		struct GLStencilSettings
		{
			GLStencilFaceDesc frontFace;
			GLStencilFaceDesc backFace;
		};

		uint16 depthTestActive;
		uint16 stencilTestActive;
		GLDepthSettings depthSettings;
		GLStencilSettings stencilSettings;
	};

	struct GLRasterizerConfig
	{
		uint32 scissorTestActive;
		GLenum cullMode;
		GLenum frontFaceVerticesOrder;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		GLenum primitiveFillMode;
	#endif
	};

	///
	class GLBlendImmutableState : public BlendImmutableState
	{
	public:
		GLBlendConfig const mGLBlendConfig;

	public:
		GLBlendImmutableState( GLGPUDevice & pGPUDevice, const GLBlendConfig & pGLBlendConfig, Bitmask<EBlendConfigFlags> pBlendFlags );
		virtual ~GLBlendImmutableState();

		static GpaHandle<GLBlendImmutableState> createInstance( GLGPUDevice & pGPUDevice, const BlendConfig & pBlendConfig );
	};

	///
	class GLDepthStencilImmutableState : public DepthStencilImmutableState
	{
	public:
		GLDepthStencilConfig const mGLDepthStencilConfig;

	public:
		GLDepthStencilImmutableState( GLGPUDevice & pGPUDevice, const GLDepthStencilConfig & pGLDepthStencilConfig, Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags  );
		virtual ~GLDepthStencilImmutableState();

		static GpaHandle<GLDepthStencilImmutableState> createInstance( GLGPUDevice & pGPUDevice, const DepthStencilConfig & pDepthStencilConfig );
	};

	///
	class GLRasterizerImmutableState : public RasterizerImmutableState
	{
	public:
		GLRasterizerConfig const mGLRasterizerConfig;

	public:
		GLRasterizerImmutableState( GLGPUDevice & pGPUDevice, const GLRasterizerConfig & pGLRasterizerConfig, Bitmask<ERasterizerConfigFlags> pRasterizerFlags );
		virtual ~GLRasterizerImmutableState();

		static GpaHandle<GLRasterizerImmutableState> createInstance( GLGPUDevice & pGPUDevice, const RasterizerConfig & pRasterizerConfig );
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLBlendConfig translateBlendConfig( const BlendConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLDepthStencilConfig translateDepthStencilConfig( const DepthStencilConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettings(
				const RTColorAttachmentBlendSettings & pSettings );

	}

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__
