
#ifndef __TS3_ENGINE_PREREQUISITES_H__
#define __TS3_ENGINE_PREREQUISITES_H__

#include <ts3/core/prerequisites.h>
#include <ts3/math/vector.h>
#include <ts3/math/vectorOps.h>
#include <ts3/math/rect.h>
#include <ts3/math/matrix.h>
#include <ts3/math/matrixOps.h>
#include <ts3/math/matrixUtils.h>
#include <ts3/stdext/arrayView.h>

#include <map>
#include <set>

#if( TS3_BUILD_STATIC )
#  define TS3_ENGINE_API
#  define TS3_ENGINE_CLASS
#  define TS3_ENGINE_OBJ    extern
#else
#  if( TS3_ENGINE_BUILD )
#    define TS3_ENGINE_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_ENGINE_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_ENGINE_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3_ENGINE_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_ENGINE_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_ENGINE_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/gpuapiCommon.h"
#include "prerequisites/geometryCommon.h"
#include "prerequisites/coreEngineState.h"

namespace ts3
{

	class CoreEngineState;

	namespace gpa
	{

		constexpr uint32 MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM = 8;

		constexpr uint32 MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM = gpuapi::gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM;

	}

	template <typename TData>
	using GeometryVertexComponentDataArray = std::array<TData, gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM>;

	class CoreEngineObject : public DynamicInterface
	{
	public:
		const CoreEngineState & mCES;

	public:
		explicit CoreEngineObject( const CoreEngineState & pCES )
		: mCES( pCES )
		{}

		virtual ~CoreEngineObject() = default;
	};

}

#endif // __TS3_ENGINE_PREREQUISITES_H__
