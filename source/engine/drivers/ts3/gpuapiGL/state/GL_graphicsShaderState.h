
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_SHADER_STATE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_SHADER_STATE_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/graphicsShaderState.h>
#include <ts3/gpuapi/state/graphicsShaderLinkageImmutableState.h>
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );

	/// @brief
	class GLGraphicsShaderLinkageImmutableState : public GraphicsShaderLinkageImmutableState
	{
	public:
		GLGraphicsShaderLinkageImmutableState(
				GLGPUDevice & pGPUDevice,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties );

		virtual ~GLGraphicsShaderLinkageImmutableState();
	};

	/// @brief
	class GLGraphicsShaderLinkageImmutableStateCore : public GLGraphicsShaderLinkageImmutableState
	{
	public:
		GLShaderPipelineObjectHandle const mGLShaderPipelineObject;

	public:
		GLGraphicsShaderLinkageImmutableStateCore(
				GLGPUDevice & pGPUDevice,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties,
				GLShaderPipelineObjectHandle pGLShaderPipelineObject );

		virtual ~GLGraphicsShaderLinkageImmutableStateCore();

		TS3_ATTR_NO_DISCARD static GpaHandle<GLGraphicsShaderLinkageImmutableStateCore> createInstance(
				GLGPUDevice & pGPUDevice,
				const GraphicsShaderSet & pShaderSet );
	};

	/// @brief
	class GLGraphicsShaderLinkageImmutableStateCompat : public GLGraphicsShaderLinkageImmutableState
	{
	public:
		GLShaderProgramObjectHandle const mGLShaderProgramObject;

	public:
		GLGraphicsShaderLinkageImmutableStateCompat(
				GLGPUDevice & pGPUDevice,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties,
				GLShaderProgramObjectHandle pGLShaderProgramObject );

		virtual ~GLGraphicsShaderLinkageImmutableStateCompat();

		TS3_ATTR_NO_DISCARD static GpaHandle<GLGraphicsShaderLinkageImmutableStateCompat> createInstance(
				GLGPUDevice & pGPUDevice,
				const GraphicsShaderSet & pShaderSet );
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLShaderPipelineObjectHandle createGraphicsShaderPipelineObject(
				const GraphicsShaderSet & pShaderSet );

		TS3_ATTR_NO_DISCARD GLShaderProgramObjectHandle createGraphicsShaderProgramObject(
				const GraphicsShaderSet & pShaderSet );

		void updateUniformDataCurrent(
				GLShaderPipelineObject & pShaderPipeline,
				uint32 pUniformIndex,
				EBaseDataType pBaseType,
				uint32 pLength,
				const void * pData );

		void updateUniformDataExplicit(
				GLShaderProgramObject & pShaderProgram,
				uint32 pUniformIndex,
				EBaseDataType pBaseType,
				uint32 pLength,
				const void * pData );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_SHADER_STATE_H__
