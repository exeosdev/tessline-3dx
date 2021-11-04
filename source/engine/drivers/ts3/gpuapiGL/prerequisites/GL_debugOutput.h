
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_DEBUG_OUTPUT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_DEBUG_OUTPUT_H__

#include <unordered_set>

#if( TS3_SYSTEM_GL_PLATFORM_TYPE == TS3_SYSTEM_GL_PLATFORM_TYPE_ES )
#  define GLAPIENTRY
#endif

namespace ts3
{
namespace gpuapi
{

	enum class GLDebugOutputVersion : enum_default_value_t
	{
		AMDExt,
		ARBExt,
		KHRCore,
		Unknown
	};

	class GLDebugOutput
	{
	public:
		GLDebugOutput( GLDebugOutputVersion pVersion );
		virtual ~GLDebugOutput();

		virtual void enableDebugOutput( bool pEnable );
		virtual void enableSync( bool pEnable );

		void enableBreakOnEvent( bool pEnable );
		void enableEventFilter( bool pEnable );

		void setEventFilter( GLuint pEventID, bool pIgnored );
		void resetEventFilters();

		const char * getExtensionName() const;
		uint64 getEventsCounter() const;
		GLDebugOutputVersion getVersion() const;

		bool isDebugOutputActive() const;
		bool isEventFilterEnabled() const;
		bool isEventIgnored( GLuint pEventID ) const;

		static std::unique_ptr<GLDebugOutput> createInterface( GLDebugOutputVersion pHint = GLDebugOutputVersion::Unknown );

	protected:
		void processEvent( GLenum pEventSeverity, const char * pEventInfo );

	private:
		virtual void setCallbackActive( bool pEnable ) = 0;

		static bool checkAPISupport( GLDebugOutputVersion pVersion );
		static bool validateVersion( GLDebugOutputVersion pVersion );

	protected:
		enum StateFlags : uint32
		{
			STATE_FLAG_DEBUG_CALLBACK_ACTIVE = 0x0001,
			STATE_FLAG_ENABLE_BREAK_ON_EVENT = 0x0010,
			STATE_FLAG_ENABLE_EVENT_FILTER = 0x0020
		};
		using IgnoredEventsMap = std::unordered_set<GLuint>;
		GLDebugOutputVersion _apiVersion;
		IgnoredEventsMap _ignoredEventSet;
		uint64 _processedEventsNum;
		Bitmask<StateFlags> _stateFlags;
	};


#if( TS3_SYSTEM_GL_PLATFORM_TYPE == TS3_SYSTEM_GL_PLATFORM_TYPE_CORE )

	class GLDebugOutputAMDExt final : public GLDebugOutput
	{
	public:
		GLDebugOutputAMDExt();
		virtual ~GLDebugOutputAMDExt();

	private:
		virtual void setCallbackActive( bool pEnable ) override;

		void handleEvent( GLuint pEventID, GLenum pEventCategory, GLenum pEventSeverity, const GLchar * pMessage );

		static void GLAPIENTRY eventCallback( GLuint pEventID,
                                              GLenum pEventCategory,
                                              GLenum pEventSeverity,
                                              GLsizei pLength,
                                              const GLchar * pMessage,
                                              GLvoid * pUserParam );
	};

	class GLDebugOutputARBExt final : public GLDebugOutput
	{
	public:
		GLDebugOutputARBExt();
		virtual ~GLDebugOutputARBExt();

		virtual void enableSync( bool pEnable ) override;

	private:
		virtual void setCallbackActive( bool pEnable ) override;

		void handleEvent( GLuint pEventID, GLenum pEventCategory, GLenum pEventType, GLenum pEventSeverity, const GLchar * pMessage );

		static void GLAPIENTRY eventCallback( GLuint pEventID,
                                              GLenum pEventSource,
                                              GLenum pEventType,
                                              GLenum pEventSeverity,
                                              GLsizei pLength,
                                              const GLchar * pMessage,
                                              const GLvoid * pUserParam );
	};

	class GLDebugOutputKHRCore final : public GLDebugOutput
	{
	public:
		GLDebugOutputKHRCore();
		virtual ~GLDebugOutputKHRCore();

		virtual void enableSync( bool pEnable ) override;

	private:
		virtual void setCallbackActive( bool pEnable ) override;

		void handleEvent( GLuint pEventID, GLenum pEventCategory, GLenum pEventType, GLenum pEventSeverity, const GLchar * pMessage );

		static void GLAPIENTRY eventCallback( GLuint pEventID,
                                              GLenum pEventSource,
                                              GLenum pEventType,
                                              GLenum pEventSeverity,
                                              GLsizei pLength,
                                              const GLchar * pMessage,
                                              const GLvoid * pUserParam );
	};

#endif

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_DEBUG_OUTPUT_H__
