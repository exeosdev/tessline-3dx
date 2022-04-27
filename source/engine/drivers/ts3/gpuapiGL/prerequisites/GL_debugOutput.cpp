
#include "../GL_prerequisites.h"
#include "../GL_coreAPIProxy.h"

namespace ts3::gpuapi
{

#if( TS3_SYSTEM_GL_PLATFORM_TYPE == TS3_SYSTEM_GL_PLATFORM_TYPE_DESKTOP )

	static const size_t sEventInfoBufferSize = 1024;

	GLDebugOutput::GLDebugOutput( GLDebugOutputVersion pVersion )
	: _apiVersion( pVersion )
	, _processedEventsNum( 0 )
	, _stateFlags( 0 )
	{}

	GLDebugOutput::~GLDebugOutput()
	{}

	void GLDebugOutput::enableDebugOutput( bool pEnable )
	{
		if( pEnable )
		{
			glEnable( GL_DEBUG_OUTPUT );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_DEBUG_OUTPUT );
			ts3OpenGLHandleLastError();
		}

		setCallbackActive( pEnable );
		ts3DebugOutputFmt( "Debug output has been %s.", pEnable ? "enabled" : "disabled" );
	}

	void GLDebugOutput::enableSync( bool pEnable )
	{
		// By default - nothing. AMD extension, for example, does not have this concept.
	}

	void GLDebugOutput::enableBreakOnEvent( bool pEnable )
	{
		_stateFlags.setOrUnset( STATE_FLAG_ENABLE_BREAK_ON_EVENT, pEnable );
	}

	void GLDebugOutput::enableEventFilter( bool pEnable )
	{
		_stateFlags.setOrUnset( STATE_FLAG_ENABLE_EVENT_FILTER, pEnable );
	}

	void GLDebugOutput::setEventFilter( GLuint pEventID, bool pIgnored )
	{
		if( pIgnored )
		{
			_ignoredEventSet.insert( pEventID );
		}
		else
		{
			_ignoredEventSet.erase( pEventID );
		}
	}

	void GLDebugOutput::resetEventFilters()
	{
		_ignoredEventSet.clear();
	}

	const char * GLDebugOutput::getExtensionName() const
	{
		return GLCoreAPIProxy::translateGLDebugOutputExtensionName( _apiVersion );
	}

	uint64 GLDebugOutput::getEventsCounter() const
	{
		return _processedEventsNum;
	}

	GLDebugOutputVersion GLDebugOutput::getVersion() const
	{
		return _apiVersion;
	}

	bool GLDebugOutput::isDebugOutputActive() const
	{
		return _stateFlags.isSet( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
	}

	bool GLDebugOutput::isEventIgnored( GLuint pEventID ) const
	{
		if( !_stateFlags.isSet( STATE_FLAG_ENABLE_EVENT_FILTER ) || _ignoredEventSet.empty() )
		{
			return false;
		}
		auto eventIter = _ignoredEventSet.find( pEventID );
		return eventIter != _ignoredEventSet.end();
	}

	std::unique_ptr<GLDebugOutput> GLDebugOutput::createInterface( GLDebugOutputVersion pHint )
	{
		GLDebugOutputVersion selectedVersion = GLDebugOutputVersion::Unknown;

		if( checkAPISupport( GLDebugOutputVersion::KHRCore ) )
		{
			selectedVersion = GLDebugOutputVersion::KHRCore;
		}
		else if( checkAPISupport( GLDebugOutputVersion::ARBExt ) )
		{
			selectedVersion = GLDebugOutputVersion::ARBExt;
		}
		else if( checkAPISupport( GLDebugOutputVersion::AMDExt ) )
		{
			selectedVersion = GLDebugOutputVersion::AMDExt;
		}

		// If selected version remains Unknown, neither version of
		// the DebugOutput API is supported by the current runtime.
		if( selectedVersion == GLDebugOutputVersion::Unknown )
		{
			return nullptr;
		}

		// Preferred API version overwrites the selected one, as long
		// as it is a valid version and is supported by the runtime.
		if( validateVersion( pHint ) && checkAPISupport( pHint ) )
		{
			selectedVersion = pHint;
		}

		std::unique_ptr<GLDebugOutput> debugOutputInterface;
		switch( selectedVersion )
		{
			case GLDebugOutputVersion::AMDExt:
				debugOutputInterface = std::make_unique<GLDebugOutputAMDExt>();
				break;

			case GLDebugOutputVersion::ARBExt:
				debugOutputInterface = std::make_unique<GLDebugOutputARBExt>();
				break;

			case GLDebugOutputVersion::KHRCore:
				debugOutputInterface = std::make_unique<GLDebugOutputKHRCore>();
				break;
		}

		if( debugOutputInterface )
		{
			auto apiVersion = debugOutputInterface->getVersion();
			const char * extensionName = debugOutputInterface->getExtensionName();

			ts3DebugOutputFmt( "GLDebugOutput object has been initialized (version %u: %s)", apiVersion, extensionName );

		}

		return debugOutputInterface;
	}

	void GLDebugOutput::processEvent( GLenum pEventSeverity, const char * pEventInfo )
	{
		++_processedEventsNum;

		ts3DebugOutput( pEventInfo );

		if( ( pEventSeverity == GL_DEBUG_SEVERITY_HIGH_ARB ) && _stateFlags.isSet( STATE_FLAG_ENABLE_BREAK_ON_EVENT ) )
		{
			ts3DebugInterrupt();
		}
	}

	bool GLDebugOutput::checkAPISupport( GLDebugOutputVersion pVersion )
	{
		return true;
	}

	bool GLDebugOutput::validateVersion( GLDebugOutputVersion pVersion )
	{
		return ( pVersion == GLDebugOutputVersion::AMDExt ) ||
		       ( pVersion == GLDebugOutputVersion::ARBExt ) ||
		       ( pVersion == GLDebugOutputVersion::KHRCore );
	}


	GLDebugOutputAMDExt::GLDebugOutputAMDExt()
	: GLDebugOutput( GLDebugOutputVersion::AMDExt )
	{}

	GLDebugOutputAMDExt::~GLDebugOutputAMDExt() = default;

	void GLDebugOutputAMDExt::setCallbackActive( bool pEnable )
	{
		if( pEnable )
		{
			glDebugMessageCallbackAMD( eventCallback, this );
			_stateFlags.set( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
		else
		{
			glDebugMessageCallbackAMD( nullptr, nullptr );
			_stateFlags.unset( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
	}

	void GLDebugOutputAMDExt::handleEvent( GLuint pEventID, GLenum pEventCategory, GLenum pEventSeverity, const GLchar * pMessage )
	{
		if( isEventIgnored( pEventID ) )
		{
			return;
		}

		char eventInfoBuffer[sEventInfoBufferSize];
		snprintf( eventInfoBuffer,
		          sEventInfoBufferSize,
		          "\n----------------------------------------------------------------------\n"\
					"[GL Debug Event (via AMD_debug_output)]\n\n"\
					"- ID: %u\n"\
					"- Category: %s\n"\
					"- Severity: %s\n\n"\
					"- Description: %s\n"\
					"----------------------------------------------------------------------\n",
		          pEventID,
		          GLCoreAPIProxy::translateGLDebugEventCategoryStrAMD( pEventCategory ),
		          GLCoreAPIProxy::translateGLDebugEventSeverityStr( pEventSeverity ),
		          pMessage );

		processEvent( pEventSeverity, eventInfoBuffer );
	}

	void GLDebugOutputAMDExt::eventCallback( GLuint pEventID,
	                                         GLenum pEventCategory,
	                                         GLenum pEventSeverity,
	                                         GLsizei pLength,
	                                         const GLchar * pMessage,
	                                         GLvoid * pUserParam )
	{
		auto * debugOutputInterface = reinterpret_cast<GLDebugOutputAMDExt *>( pUserParam );
		debugOutputInterface->handleEvent( pEventID, pEventCategory, pEventSeverity, pMessage );
	}


	GLDebugOutputARBExt::GLDebugOutputARBExt()
	: GLDebugOutput( GLDebugOutputVersion::ARBExt )
	{}

	GLDebugOutputARBExt::~GLDebugOutputARBExt() = default;

	void GLDebugOutputARBExt::enableSync( bool pEnable )
	{
		if( pEnable )
		{
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
			ts3OpenGLHandleLastError();
		}
	}

	void GLDebugOutputARBExt::setCallbackActive( bool pEnable )
	{
		if( pEnable )
		{
			glDebugMessageCallbackARB( eventCallback, this );
			_stateFlags.set( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
		else
		{
			glDebugMessageCallbackARB( nullptr, nullptr );
			_stateFlags.unset( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
	}

	void GLDebugOutputARBExt::handleEvent( GLuint pEventID, GLenum pEventSource, GLenum pEventType, GLenum pEventSeverity, const GLchar * pMessage )
	{
		if( isEventIgnored( pEventID ) )
		{
			return;
		}

		char eventInfoBuffer[sEventInfoBufferSize];
		snprintf( eventInfoBuffer,
		          sEventInfoBufferSize,
		          "\n----------------------------------------------------------------------\n"\
					"[GL Debug Event (via ARB_debug_output)]\n\n"\
					"- ID: %u\n"\
					"- Source: %s\n"\
					"- Type: %s\n"\
					"- Severity: %s\n\n"\
					"- Description: %s\n"\
					"----------------------------------------------------------------------\n",
		          pEventID,
		          GLCoreAPIProxy::translateGLDebugEventSourceStr( pEventSource ),
		          GLCoreAPIProxy::translateGLDebugEventTypeStr( pEventType ),
		          GLCoreAPIProxy::translateGLDebugEventSeverityStr( pEventSeverity ),
		          pMessage );

		processEvent( pEventSeverity, eventInfoBuffer );
	}

	void GLDebugOutputARBExt::eventCallback( GLuint pEventID,
	                                         GLenum pEventSource,
	                                         GLenum pEventType,
	                                         GLenum pEventSeverity,
	                                         GLsizei pLength,
	                                         const GLchar * pMessage,
	                                         const GLvoid * pUserParam )
	{
		auto * debugOutputInterface = reinterpret_cast<GLDebugOutputARBExt *>( const_cast<void *>( pUserParam ) );
		debugOutputInterface->handleEvent( pEventID, pEventSource, pEventType, pEventSeverity, pMessage );
	}


	GLDebugOutputKHRCore::GLDebugOutputKHRCore()
	: GLDebugOutput( GLDebugOutputVersion::KHRCore )
	{}

	GLDebugOutputKHRCore::~GLDebugOutputKHRCore() = default;

	void GLDebugOutputKHRCore::enableSync( bool pEnable )
	{
		if( pEnable )
		{
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			ts3OpenGLHandleLastError();
		}
	}

	void GLDebugOutputKHRCore::setCallbackActive( bool pEnable )
	{
		if( pEnable )
		{
			glDebugMessageCallback( eventCallback, this );
			_stateFlags.set( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
		else
		{
			glDebugMessageCallback( nullptr, nullptr );
			_stateFlags.unset( STATE_FLAG_DEBUG_CALLBACK_ACTIVE );
		}
	}

	void GLDebugOutputKHRCore::handleEvent( GLuint pEventID, GLenum pEventSource, GLenum pEventType, GLenum pEventSeverity, const GLchar * pMessage )
	{
		if( isEventIgnored( pEventID ) )
		{
			return;
		}

		char eventInfoBuffer[sEventInfoBufferSize];
		snprintf( eventInfoBuffer,
		          sEventInfoBufferSize,
		          "\n----------------------------------------------------------------------\n"\
		           "[GL Debug Event (via KHR_debug)]\n\n"\
		           "- ID: %u\n"\
		           "- Source: %s\n"\
		           "- Type: %s\n"\
		           "- Severity: %s\n\n"\
		           "- Description: %s\n"\
		           "----------------------------------------------------------------------\n",
		          pEventID,
		          GLCoreAPIProxy::translateGLDebugEventSourceStr( pEventSource ),
		          GLCoreAPIProxy::translateGLDebugEventTypeStr( pEventType ),
		          GLCoreAPIProxy::translateGLDebugEventSeverityStr( pEventSeverity ),
		          pMessage );

		processEvent( pEventSeverity, eventInfoBuffer );
	}

	void GLDebugOutputKHRCore::eventCallback( GLuint pEventID,
	                                          GLenum pEventSource,
	                                          GLenum pEventType,
	                                          GLenum pEventSeverity,
	                                          GLsizei pLength,
	                                          const GLchar * pMessage,
	                                          const GLvoid * pUserParam )
	{
		auto * debugOutputInterface = reinterpret_cast<GLDebugOutputKHRCore *>( const_cast<void *>( pUserParam ) );
		debugOutputInterface->handleEvent( pEventID, pEventSource, pEventType, pEventSeverity, pMessage );
	}

#elif( TS3_SYSTEM_GL_PLATFORM_TYPE == TS3_SYSTEM_GL_PLATFORM_TYPE_ES )

	GLDebugOutput::GLDebugOutput( GLDebugOutputVersion /* pVersion */ )
	: _apiVersion( GLDebugOutputVersion::Unknown )
	, _processedEventsNum( 0 )
	, _stateFlags( 0 )
	{}

	GLDebugOutput::~GLDebugOutput()
	{}

	void GLDebugOutput::enableDebugOutput( bool pEnable )
	{}

	void GLDebugOutput::enableSync( bool pEnable )
	{}

	void GLDebugOutput::enableBreakOnEvent( bool pEnable )
	{}

	void GLDebugOutput::enableEventFilter( bool pEnable )
	{}

	void GLDebugOutput::setEventFilter( GLuint pEventID, bool pIgnored )
	{}

	void GLDebugOutput::resetEventFilters()
	{}

	const char * GLDebugOutput::getExtensionName() const
	{
		return GLCoreAPIProxy::translateGLDebugOutputExtensionName( GLDebugOutputVersion::Unknown );
	}

	uint64 GLDebugOutput::getEventsCounter() const
	{
		return 0;
	}

	GLDebugOutputVersion GLDebugOutput::getVersion() const
	{
		return GLDebugOutputVersion::Unknown;
	}

	bool GLDebugOutput::isDebugOutputActive() const
	{
		return false;
	}

	bool GLDebugOutput::isEventIgnored( GLuint pEventID ) const
	{
		return true;
	}

	std::unique_ptr<GLDebugOutput> GLDebugOutput::createInterface( GLDebugOutputVersion pHint )
	{
		return nullptr;
	}

	void GLDebugOutput::processEvent( GLenum pEventSeverity, const char * pEventInfo )
	{}

	bool GLDebugOutput::checkAPISupport( GLDebugOutputVersion pVersion )
	{
		return pVersion == GLDebugOutputVersion::Unknown;
	}

	bool GLDebugOutput::validateVersion( GLDebugOutputVersion pVersion )
	{
		return pVersion == GLDebugOutputVersion::Unknown;
	}

#endif

} // namespace ts3::gpuapi
