
set( _EBS_CDEF_MATCHSTR_ARCHLIST "arm32v7a|arm64v8a|aarch64|x86|IA32|Win32|x64|x86-64|IA64|Win64" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_ARMF32 "^arm32v7a" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_ARMF64 "^arm64v8a|^aarch64" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_X86F32 "^x86$|^IA32$|^Win32$" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_X86F64 "^x64$|^x86-64$|^IA64$|^Win64$" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_32 "${_EBS_CDEF_MATCHSTR_ARCH_ARMF32}${_EBS_CDEF_MATCHSTR_ARCH_X86F32}" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_64 "${_EBS_CDEF_MATCHSTR_ARCH_ARMF64}${_EBS_CDEF_MATCHSTR_ARCH_X86F64}" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_ARMF "${_EBS_CDEF_MATCHSTR_ARCH_ARMF32}${_EBS_CDEF_MATCHSTR_ARCH_ARMF64}" )
#
set( _EBS_CDEF_MATCHSTR_ARCH_X86F "${_EBS_CDEF_MATCHSTR_ARCH_X86F32}${_EBS_CDEF_MATCHSTR_ARCH_X86F64}" )


macro( ebsSysconfCmdGet pSysconfVarRefName outValue )
	ebsVmsCmdGet( Sysconf.${pSysconfVarRefName} IGNORE ${outValue} )
endmacro()

macro( ebsSysconfCmdIsSet pSysconfVarRefName outIsSet )
	ebsVmsCmdIsSet( Sysconf.${pSysconfVarRefName} IGNORE ${outIsSet} )
endmacro()

macro( _ebsSysconfCmdSetInternal pSysconfVarRefName pValue )
	_ebsVmsCmdSetInternal( Sysconf.${pSysconfVarRefName} IGNORE "${pValue}" )
endmacro()

macro( _ebsSysconfCmdAppendInternal pSysconfVarRefName pValue )
	_ebsVmsCmdAppendInternal( Sysconf.${pSysconfVarRefName} IGNORE "${pValue}" )
endmacro()


#@FunDef ebsSysconfQueryPlatformSetup
#@DocStr
function( ebsSysconfQueryPlatformSetup optCompiler optSystemName optArch optBuildcfg )
	if( optCompiler )
		ebsSysconfCmdGet( Host.Compiler compiler )
		set( ${optCompiler} ${compiler} PARENT_SCOPE )
	endif()
	if( optSystemName )
		ebsSysconfCmdGet( Target.System system )
		set( ${optSystemName} ${system} PARENT_SCOPE )
	endif()
	if( optArch )
		ebsSysconfCmdGet( Target.Arch arch )
		set( ${optArch} ${arch} PARENT_SCOPE )
	endif()
	if( optBuildcfg )
		ebsSysconfCmdGet( Target.Buildcfg buildcfg )
		set( ${optBuildcfg} ${buildcfg} PARENT_SCOPE )
	endif()
endfunction()


#@FunDef _ebsSysconfAnalyzeCMakeEnv
#@DocStr 
function( _ebsSysconfAnalyzeCMakeEnv )
    if( MSVC OR MSVC_IDE OR XCODE_VERSION )
        _ebsSysconfCmdSetInternal( Host.FlagGenMultitarget TRUE )
    endif()
endfunction()

#@FunDef _ebsSysconfDetectHostCompiler
#@DocStr 
function( _ebsSysconfDetectHostCompiler )

	if( MINGW )
		_ebsSysconfCmdSetInternal( Host.Compiler "MinGW" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "armCC" )
		_ebsSysconfCmdSetInternal( Host.Compiler "armCC" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
		_ebsSysconfCmdSetInternal( Host.Compiler "Clang" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
		_ebsSysconfCmdSetInternal( Host.Compiler "GCC" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
		_ebsSysconfCmdSetInternal( Host.Compiler "ICC" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "NVIDIA" )
        _ebsSysconfCmdSetInternal( Host.Compiler "NVCUDA" )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
		_ebsSysconfCmdSetInternal( Host.Compiler "MSVC" )
		if( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "16\\.*"  )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2010 )
		elseif( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "17\\.*" )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2012 )
		elseif( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "18\\.*" )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2013 )
		elseif( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "19\\.0[0-9]*" )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2015 )
		elseif( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "19\\.1[1-9]*" )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2017 )
		elseif( "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "19\\.2[0-4]*" )
			_ebsSysconfCmdSetInternal( Host.CompilerVersion 2019 )
		endif()
	endif()

	ebsSysconfCmdGet( Host.CompilerVersion compilerVersion )
	if( NOT compilerVersion )
		_ebsSysconfCmdSetInternal( Host.CompilerVersion ${CMAKE_CXX_COMPILER_VERSION} )
	endif()

	if( CMAKE_GENERATOR_PLATFORM )
		if( "${CMAKE_GENERATOR}" MATCHES "Visual Studio" )
			if( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "^arm$" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch arm )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "arm64" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch arm64 )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "IA32|^x86$|^X86$" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch x86 )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "Win64|x86_64|X86_64" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch x86-64 )
			endif()
		endif()
	else()
		if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" )
			if( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "^arm$" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch arm )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "arm64" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch arm64 )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "x64|X64" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch x86-64 )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "x86|X86" )
				_ebsSysconfCmdSetInternal( Host.ToolsetArch x86 )
			endif()
		endif()
	endif()

    if( "${EBSVAR_Sysconf_Host_ToolsetArch}" MATCHES "${_EBS_CDEF_MATCH_STR_ARCH_32}" )
        _ebsSysconfCmdSetInternal( "Host.ToolsetArchBit" "32" )
    elseif( "${EBSVAR_Sysconf_Host_ToolsetArch}" MATCHES "${_EBS_CDEF_MATCH_STR_ARCH_64}" )
        _ebsSysconfCmdSetInternal( "Host.ToolsetArchBit" "64" )
    endif()

endfunction()

#@FunDef _ebsSysconfDetectHostSystemAndVersion
#@DocStr 
function( _ebsSysconfDetectHostPlatformSetup )
    # Get name of the host system.
	ebsCoreUtilSysQueryCMakeSystemName( hostSystemName XHOSTSystem )
	if( NOT hostSystemName )
		ebsVmsLogError( "Error: host system could not be determined." )
		return()
	endif()
	# Store the host name in the VMS registry.
    _ebsSysconfCmdSetInternal( Host.SystemName ${hostSystemName} )
	# Handle Windows family first. It's really optional, but I found the ability to check the exact version quite useful.
	if( ${hostSystemName} STREQUAL "WinDesktop" )
		if( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "10\\.0\\.[0-9]+(\\.[0-9]+)?" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 10.0 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.3\\.96[0-9]+" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 8.1U1 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.3\\.92[0-9]+" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 8.1 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.2\\.92[0-9]+" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 8.0 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.1\\.7601" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 7SP1 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.1\\.7600" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion 7 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.0\\.6002" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion VistaSP2 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.0\\.6001" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion VistaSP1 )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\\.0\\.6000" )
			_ebsSysconfCmdSetInternal( Host.SystemVersion Vista )
		endif()
	endif()

	ebsSysconfCmdIsSet( Host.SystemVersion isSystemVersionSet )
	if( NOT isSystemVersionSet )
		_ebsSysconfCmdSetInternal( Host.SystemVersion "${CMAKE_HOST_SYSTEM_VERSION}" )
	endif()

	if( ${hostSystemName} MATCHES "Android|Linux" )
		_ebsSysconfCmdSetInternal( Host.SharedLibExtension  .so )
		_ebsSysconfCmdSetInternal( Host.SharedLibNamePrefix lib )
		_ebsSysconfCmdSetInternal( Host.StaticLibExtension  .a )
		_ebsSysconfCmdSetInternal( Host.StaticLibNamePrefix lib )
	elseif( ${hostSystemName} MATCHES "WinDesktop" )
		_ebsSysconfCmdSetInternal( Host.SharedLibExtension  .dll )
		_ebsSysconfCmdSetInternal( Host.SharedLibNamePrefix OFF )
		_ebsSysconfCmdSetInternal( Host.StaticLibExtension  .lib )
		_ebsSysconfCmdSetInternal( Host.StaticLibNamePrefix OFF )
	elseif( ${hostSystemName} MATCHES "macOS" )
		_ebsSysconfCmdSetInternal( Host.SharedLibExtension  .dylib )
		_ebsSysconfCmdSetInternal( Host.SharedLibNamePrefix lib )
		_ebsSysconfCmdSetInternal( Host.StaticLibExtension  .a )
		_ebsSysconfCmdSetInternal( Host.StaticLibNamePrefix lib )
	endif()

endfunction()

_ebsVmsRawSet( EBS_CONFIG_ENABLE_VMS_VARTRACE TRUE )

# CM_EBS_SYSCONF_HOST_COMPILER
# CM_EBS_SYSCONF_HOST_COMPILER_VERSION
# CM_EBS_SYSCONF_HOST_PROP_SHARED_LIB_EXT
# CM_EBS_SYSCONF_HOST_PROP_SHARED_LIB_PREFIX
# CM_EBS_SYSCONF_HOST_PROP_STATIC_LIB_EXT
# CM_EBS_SYSCONF_HOST_PROP_STATIC_LIB_PREFIX
# CM_EBS_SYSCONF_HOST_SYSTEM_NAME
# CM_EBS_SYSCONF_HOST_SYSTEM_VERSION
# CM_EBS_SYSCONF_HOST_TOOLSET_ARCH
# CM_EBS_SYSCONF_HOST_TOOLSET_ARCH_BIT


# CM_EBS_SYSCONF_TARGET_ARCH
# CM_EBS_SYSCONF_TARGET_BUILDCONFIG
# CM_EBS_SYSCONF_TARGET_SYSTEM
# CM_EBS_SYSCONF_TARGET_PLATFORM
# CM_EBS_SYSCONF_TARGET_FLAG_ENABLE_DEBUGINFO
# CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_ANDROID
# CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_APPLE
# CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_POSIX
# CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE
# CM_EBS_SYSCONF_TARGET_PROP_SHARED_LIB_EXT
# CM_EBS_SYSCONF_TARGET_PROP_SHARED_LIB_PREFIX
# CM_EBS_SYSCONF_TARGET_PROP_STATIC_LIB_EXT
# CM_EBS_SYSCONF_TARGET_PROP_STATIC_LIB_PREFIX

#@FunDef _ebsSysconfDetectTargetPlatform
#@DocStr 
function( _ebsSysconfDetectTargetPlatform )

	ebsSysconfCmdGet( "Target.System" targetSystemName )
	ebsSysconfCmdGet( "Target.SysAPIOverride" sysAPIOverride )

	# Target system may be set via config for cross-compiling, check.
	if( NOT targetSystemName )
		ebsCoreUtilSysQueryCMakeSystemName( targetSystemName XTARGET )
		_ebsSysconfCmdSetInternal( "Target.System" ${targetSystemName} )
	endif()

	if( NOT targetSystemName )
		message( FATAL_ERROR "Current platform (${CMAKE_SYSTEM_NAME}) is not supported!" )
	endif()

	if( "${targetSystemName}" STREQUAL "Android" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Android" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "arm32v7a" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "arm64v8a" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemAndroid" TRUE )
	elseif( "${targetSystemName}" STREQUAL "Linux" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Linux" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "x86" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( "${targetSystemName}" STREQUAL "macOS" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Apple" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "x86" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemApple" TRUE )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( "${targetSystemName}" STREQUAL "iOS" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Apple" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "" ) # 32 bit deprecated by Apple, not supported
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "arm64v8a" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemApple" TRUE )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( "${targetSystemName}" STREQUAL "WinDesktop" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Windows" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "x86" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( "${targetSystemName}" STREQUAL "WinPhone" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Windows" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "x86" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsSysconfCmdSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( "${targetSystemName}" STREQUAL "WinRT" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Windows" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "arm32v7a" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "" ) # WinRT is specifically desgined for 32-bit armv7
		_ebsSysconfCmdSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( "${targetSystemName}" STREQUAL "WinUWP" )
		_ebsSysconfCmdSetInternal( "Target.Platform" "Windows" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault32" "x86" )
		_ebsSysconfCmdSetInternal( "Target.ArchDefault64" "x86_64" )
        _ebsSysconfCmdSetInternal( "Target.FlagSystemWinFamily" TRUE )
	endif()

	if( "${targetSystemName}" MATCHES "Android|Linux" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibExtension" ".so" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibNamePrefix" "lib" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibExtension" ".a" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibNamePrefix" "lib" )
	elseif( "${targetSystemName}" MATCHES "WinDesktop|WinPhone|WinRT|WinUWP" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibExtension" ".dll" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibNamePrefix" "" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibExtension" ".lib" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibNamePrefix" "" )
	elseif( "${targetSystemName}" MATCHES "iOS|macOS" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibExtension" ".dylib" )
		_ebsSysconfCmdSetInternal( "Target.SharedLibNamePrefix" "lib" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibExtension" ".a" )
		_ebsSysconfCmdSetInternal( "Target.StaticLibNamePrefix" "lib" )
	endif()

endfunction()

#@FunDef _ebsSysconfConfigureArch
#@DocStr 
#@DocStr Options for cross-compiling:
#@DocStr - EBS_SYSCONF_TARGET_ARCH - explicit arch definition. Allowed: arm32v7a, arm64v8a/aarch64, x86/IA32/Win32, x86-64/x64/Win64, IA64
#@DocStr - EBS_SYSCONF_TARGET_ARCHBIT - either 32 or 64, sets the architecture automatically from 32/64-bit defaults for the target platform
#@DocStr - EBS_SYSCONF_TARGET_ARCHEIS - enables NEON/SSE/AVX  intrinsics for arm. Allowed: neon, neon2, sse, sse2, sse4, avx, avx2
function( _ebsSysconfConfigureArch )

	ebsSysconfCmdGet( "Host.ToolsetArchBit"   toolsetArchBit )
	ebsSysconfCmdGet( "Target.ArchDefault32"  targetDefaultArch32 )
	ebsSysconfCmdGet( "Target.ArchDefault64"  targetDefaultArch64 )

	if( EBS_SYSCONF_TARGET_ARCH AND ( NOT ${EBS_SYSCONF_TARGET_ARCH} MATCHES "${_EBS_CDEF_MATCHSTR_ARCH_ARMF}|${_EBS_CDEF_MATCHSTR_ARCH_X86F}" ) )
		ebsVmsLogWarning( "Invalid EBS_SYSCONF_TARGET_ARCH was specified and will not be used. Value: ${EBS_SYSCONF_TARGET_ARCH}. Allowed: ${_EBS_CDEF_MATCHSTR_ARCHLIST}" )
		unset( EBS_SYSCONF_TARGET_ARCH )
	endif()
	if( EBS_SYSCONF_TARGET_ARCHBIT AND ( NOT "${EBS_SYSCONF_TARGET_ARCHBIT}" MATCHES "32|64" ) )
		ebsVmsLogWarning( "Invalid EBS_SYSCONF_TARGET_ARCHBIT was specified and will not be used. Value: ${EBS_SYSCONF_TARGET_ARCHBIT}. Allowed: 32|64" )
		unset( EBS_SYSCONF_TARGET_ARCHBIT )
	endif()

	if( NOT EBS_SYSCONF_TARGET_ARCH )
		if( NOT EBS_SYSCONF_TARGET_ARCHBIT )
			set( EBS_SYSCONF_TARGET_ARCHBIT ${toolsetArchBit} )
		endif()
		set( EBS_SYSCONF_TARGET_ARCH ${targetDefaultArch${EBS_SYSCONF_TARGET_ARCHBIT}} )
	endif()

	_ebsSysconfCmdSetInternal( "Target.Arch" ${EBS_SYSCONF_TARGET_ARCH} )
	_ebsSysconfCmdSetInternal( "Target.ArchBit" ${EBS_SYSCONF_TARGET_ARCHBIT} )

endfunction()

#@FunDef _ebsSysconfConfigureBuildcfg
#@DocStr 
function( _ebsSysconfConfigureBuildcfg )

	if( EBS_CONFIG_SYSCONF_TARGET_BUILDCFG )
		if( NOT "${EBS_CONFIG_SYSCONF_TARGET_BUILDCFG}" MATCHES "Release|Debug" )
			unset( EBS_CONFIG_SYSCONF_TARGET_BUILDCFG )
		endif()
    endif()
    
    if( NOT EBS_CONFIG_SYSCONF_TARGET_BUILDCFG )
        set( EBS_CONFIG_SYSCONF_TARGET_BUILDCFG "${CMAKE_BUILD_TYPE}" )
    endif()

	string( TOLOWER "${EBS_CONFIG_SYSCONF_TARGET_BUILDCFG}" buildTypeStr )
	string( FIND "${buildTypeStr}" "deb" debStrPos )
	string( FIND "${buildTypeStr}" "rel" relStrPos )

	if( NOT ${debStrPos} EQUAL -1 )
		set( hasDebStr TRUE )
	endif()

	if( NOT ${relStrPos} EQUAL -1 )
		set( hasRelStr TRUE )
	endif()

	if( hasRelStr )
		_ebsSysconfCmdSetInternal( Target.Buildcfg Release )
		if( hasDebStr )
			_ebsSysconfCmdSetInternal( Target.FlagEnableDebugInfo TRUE )
		endif()
	else()
		_ebsSysconfCmdSetInternal( Target.Buildcfg Debug )
		_ebsSysconfCmdSetInternal( "Target.FlagEnableDebugInfo" TRUE )
	endif()

endfunction()


#@FunDef ebsSysconfInitialize
#@DocStr
function( ebsSysconfInitialize pWorkspaceRootDir )
	# Reset the registry. Clears *ALL* EBS variables, including cached ones.
	ebsVmsBaseRegistryResetSystem()
	# Basic init for CMake-specific stuff like detecting multi-target generators.
	_ebsSysconfAnalyzeCMakeEnv()
	# Detection part. Must be done in the following order:
	# 1. Host platform & compiler, preferably in this order (compiler setup may depend on the platform).
	_ebsSysconfDetectHostPlatformSetup()
	_ebsSysconfDetectHostCompiler()
	# 2. Target platform (system and its version).
	_ebsSysconfDetectTargetPlatform()
	# 3. Architecture configuration.
	_ebsSysconfConfigureArch()
	# 4. Target build config (debug, release, etc.).
	_ebsSysconfConfigureBuildcfg()
	# Now, query configured values.
	ebsSysconfQueryPlatformSetup( compiler targetSystem targetArch targetBuildcfg )
	# Set EBS internal values used for further processing.
	_ebsSysconfCmdSetInternal( "Core.InitStatus" TRUE )
	_ebsSysconfCmdSetInternal( "Core.WorkspaceRootDir" "${pWorkspaceRootDir}" )
	_ebsSysconfCmdSetInternal( "Core.PlatformConfigString" "Compiler:${compiler};System:${targetSystem};Arch:${targetArch};Buildcfg:${targetBuildcfg}" )
	# Auxiliary variables for EBS to avoid getting them when filters are matched (happens very often).
	_ebsVmsRawSet( EBS_AUX_HOST_COMPILER   "${compiler}" )
	_ebsVmsRawSet( EBS_AUX_TARGET_SYSTEM   "${targetSystem}" )
	_ebsVmsRawSet( EBS_AUX_TARGET_ARCH     "${targetArch}" )
	_ebsVmsRawSet( EBS_AUX_TARGET_BUILDCFG "${targetBuildcfg}" )
	# This is quite useful for development process.
	# ebsRegistryDumpGlobal()
endfunction()
