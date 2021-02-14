
macro( ebsVmsSysconfGet pVarRefName outValue )
    ebsVmsGet( "Sysconf.${pVarRefName}" "" ${outValue} )
endmacro()

macro( ebsVmsSysconfSet pVarRefName pValue )
	ebsVmsSet( "Sysconf.${pVarRefName}" "" ${pValue} )
endmacro()

macro( ebsVmsSysconfAppend pVarRefName pValue )
	ebsVmsAppend( "Sysconf.${pVarRefName}" "" ${pValue} )
endmacro()

macro( ebsVmsSysconfUnset pVarRefName )
    ebsVmsUnset( "Sysconf.${pVarRefName}" "" )
endmacro()


macro( _ebsVmsSysconfSetInternal pVarRefName pValue )
	_ebsVmsSetInternal( "Sysconf.${pVarRefName}" "" ${pValue} )
endmacro()

macro( _ebsVmsSysconfAppendInternal pVarRefName pValue )
	_ebsVmsAppendInternal( "Sysconf.${pVarRefName}" "" ${pValue} )
endmacro()

macro( _ebsVmsSysconfUnsetInternal pVarRefName )
    _ebsVmsUnsetInternal( "Sysconf.${pVarRefName}" "" )
endmacro()


function( ebsSysconfQueryCurrentPlatformFilterValues outCompiler outSystem outSysAPI outArch outBuildcfg )
	ebsVmsSysconfGet( "Host.Compiler" compiler )
	ebsVmsSysconfGet( "Target.System" system )
	ebsVmsSysconfGet( "Target.SysAPI" sysAPI )
	ebsVmsSysconfGet( "Target.Arch" arch )
	ebsVmsSysconfGet( "Target.BuildConfig" buildcfg )
	set( ${outCompiler} ${compiler} PARENT_SCOPE )
	set( ${outSystem} ${system} PARENT_SCOPE )
	set( ${outSysAPI} ${sysAPI} PARENT_SCOPE )
	set( ${outArch} ${arch} PARENT_SCOPE )
	set( ${outBuildcfg} ${buildcfg} PARENT_SCOPE )
endfunction()


function( ebsSysconfResetSystem )
	#
	ebsVmsBaseRegistryResetSystem()
endfunction()


function( ebsSysconfInitialize pCheckInitStatus pResetRegistry pWorkspaceRootDir )
	#
	if( pCheckInitStatus )
		ebsVmsSysconfGet( "Config.InitStatus" initStatus )
		if( initStatus )
			return()
		endif()
	endif()

	if( pResetRegistry )
		#
		ebsVmsBaseRegistryResetSystem()
	endif()

	#
	_ebsSysconfAnalyzeCMakeEnv()

	#
	_ebsSysconfDetectHostCompiler()
	_ebsSysconfDetectHostSystemAndVersion()
	_ebsSysconfDetectTargetPlatform()
	_ebsSysconfDetectArch()
	_ebsSysconfDetectBuildcfg()

	#
	_ebsVmsSysconfSetInternal( "Config.InitStatus" TRUE )
	_ebsVmsSysconfSetInternal( "Config.WorkspaceRootDir" "${pWorkspaceRootDir}" )

	ebsRegistryDumpGlobal()
endfunction()


#
function( _ebsSysconfAnalyzeCMakeEnv )

    if( MSVC OR MSVC_IDE OR XCODE_VERSION )
        _ebsVmsSysconfSetInternal( "Host.FlagGenMultitarget" TRUE )
    endif()

endfunction()

#
function( _ebsSysconfDetectHostCompiler )

	if( MINGW )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "MinGW" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "ARMCC" )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "ARMCC" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "Clang" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "GCC" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "ICC" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
		_ebsVmsSysconfSetInternal( "Host.Compiler" "MSVC" )
		if( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "16\.*"  )
			_ebsVmsSysconfSetInternal( "Host.CompilerVersion" 2010 )
		elseif( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "17\.*" )
			_ebsVmsSysconfSetInternal( "Host.CompilerVersion" 2012 )
		elseif( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "18\.*" )
			_ebsVmsSysconfSetInternal( "Host.CompilerVersion" 2013 )
		elseif( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "19\.0[0-9]*" )
			_ebsVmsSysconfSetInternal( "Host.CompilerVersion" 2015 )
		elseif( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "19\.1[0-9]*" )
			_ebsVmsSysconfSetInternal( "Host.CompilerVersion" 2017 )
		endif()
	elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "NVIDIA" )
        _ebsVmsSysconfSetInternal( "Host.Compiler" "NVCUDA" )
		_ebsVmsSysconfSetInternal( "Host.CompilerVersion" ${CMAKE_CXX_COMPILER_VERSION} )
	endif()

	if( CMAKE_GENERATOR_PLATFORM )
		if( "${CMAKE_GENERATOR}" MATCHES "Visual Studio" )
			if( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "^ARM$" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "ARM" )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "ARM64" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "ARM64" )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "IA32|^x86$" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "x86" )
			elseif( "${CMAKE_GENERATOR_PLATFORM}" MATCHES "Win64|x86_64" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "x86_64" )
			endif()
		endif()
	else()
		if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
			if( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "^ARM$" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "ARM" )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "ARM64" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "ARM64" )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "x64" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "x86_64" )
			elseif( "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "x86" )
				_ebsVmsSysconfSetInternal( "Host.ToolsetArch" "x86" )
			endif()
		endif()
	endif()

    if( "${EBSVAR_Sysconf_Host_ToolsetArch}" MATCHES "${_EBS_VINTERNALDEF_MATCH_STR_ARCH_32}" )
        _ebsVmsSysconfSetInternal( "Host.ToolsetArchBit" "32" )
    elseif( "${EBSVAR_Sysconf_Host_ToolsetArch}" MATCHES "${_EBS_VINTERNALDEF_MATCH_STR_ARCH_64}" )
        _ebsVmsSysconfSetInternal( "Host.ToolsetArchBit" "64" )
    endif()

endfunction()

#
function( _ebsSysconfDetectHostSystemAndVersion )

    # Get name of the host system.
	ebsCommonUtilSystemQueryName( TRUE hostSystemName )
	#
    _ebsVmsSysconfSetInternal( "Host.SystemName" "${hostSystemName}" )

	if( NOT hostSystemName )
		##FATAL_ERROR
	endif()

	if( ${hostSystemName} STREQUAL "WinDesktop" )
		if( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "10\.[0\.[0-9]+" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "10.0" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.3\.96[0-9]+" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "8.1U1" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.3\.92[0-9]+" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "8.1" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.2\.92[0-9]+" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "8.0" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.1\.7601" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "7SP1" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.1\.7600" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "7" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.0\.6002" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "VistaSP2" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.0\.6001" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "VistaSP1" )
		elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "6\.0\.6000" )
			_ebsVmsSysconfSetInternal( "Host.SystemVersion" "Vista" )
		endif()
	else()
		_ebsVmsSysconfSetInternal( "Host.SystemVersion" "${CMAKE_HOST_SYSTEM_VERSION}" )
	endif()

	if( ${hostSystemName} MATCHES "Android|Linux" )
		_ebsVmsSysconfSetInternal( "Host.SharedLibExtension" ".so" )
		_ebsVmsSysconfSetInternal( "Host.SharedLibNamePrefix" "lib" )
		_ebsVmsSysconfSetInternal( "Host.StaticLibExtension" ".a" )
		_ebsVmsSysconfSetInternal( "Host.StaticLibNamePrefix" "lib" )
	elseif( ${hostSystemName} MATCHES "WinDesktop" )
		_ebsVmsSysconfSetInternal( "Host.SharedLibExtension" ".dll" )
		_ebsVmsSysconfSetInternal( "Host.StaticLibExtension" ".lib" )
	elseif( ${hostSystemName} MATCHES "macOS" )
		_ebsVmsSysconfSetInternal( "Host.SharedLibExtension" ".dylib" )
		_ebsVmsSysconfSetInternal( "Host.SharedLibNamePrefix" "lib" )
		_ebsVmsSysconfSetInternal( "Host.StaticLibExtension" ".a" )
		_ebsVmsSysconfSetInternal( "Host.StaticLibNamePrefix" "lib" )
	endif()

endfunction()

#
function( _ebsSysconfDetectTargetPlatform )

	ebsVmsSysconfGet( "Target.System" targetSystemName )
	ebsVmsSysconfGet( "Target.SysAPIOverride" sysAPIOverride )

	# Target system may be set via config for cross-compiling, check.
	if( NOT targetSystemName )
        ebsCommonUtilSystemQueryName( FALSE targetSystemName )
		_ebsVmsSysconfSetInternal( "Target.System" ${targetSystemName} )
	endif()

	if( NOT targetSystemName )
		message( FATAL_ERROR "Current platform (${CMAKE_SYSTEM_NAME}) is not supported!" )
	endif()

	if( ${targetSystemName} STREQUAL "Android" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Android" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "Android" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "ARM" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "ARM64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemAndroid" TRUE )
	elseif( ${targetSystemName} STREQUAL "Linux" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Linux" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "X11" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "x86" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( ${targetSystemName} STREQUAL "iOS" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Apple" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "iOS" )
		# _ebsVmsSysconfSetInternal( "Target.ArchDefault32" "ARM" ) 32 bit deprecated by Apple, not supported
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "ARM64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemApple" TRUE )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( ${targetSystemName} STREQUAL "macOS" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Apple" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "SystemX" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "x86" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemApple" TRUE )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemPOSIX" TRUE )
	elseif( ${targetSystemName} STREQUAL "WinDesktop" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Windows" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "Win32" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "x86" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( ${targetSystemName} STREQUAL "WinPhone" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Windows" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "WinPhone" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "x86" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( ${targetSystemName} STREQUAL "WinRT" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Windows" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "WinRT" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "ARM" )
		# _ebsVmsSysconfSetInternal( "Target.ArchDefault64" "ARM64" ) WinRT is specifically desgined for 32-bit ARMv7
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 32 )
		_ebsVmsSysconfSetInternal( "Target.FlagSystemWinFamily" TRUE )
	elseif( ${targetSystemName} STREQUAL "WinUWP" )
		_ebsVmsSysconfSetInternal( "Target.Platform" "Windows" )
		_ebsVmsSysconfSetInternal( "Target.SysAPI" "WinUWP" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault32" "x86" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefault64" "x86_64" )
		_ebsVmsSysconfSetInternal( "Target.ArchDefaultBit" 64 )
        _ebsVmsSysconfSetInternal( "Target.FlagSystemWinFamily" TRUE )
	endif()

	if( sysAPIOverride )
		if( "${sysAPIOverride}" STREQUAL "SDL" )
			_ebsVmsSysconfSetInternal( "Target.SysAPI" "SDL" )
		endif()
	endif()

endfunction()

#
function( _ebsSysconfDetectArch )

    if( EBSVAR_Sysconf_Host_ToolsetArch AND EBS_CONFIG_SYSCONF_TARGET_ARCH )
    
        set( hostToolsetArch "${EBSVAR_Sysconf_Host_ToolsetArch}" )
        set( targetArch "${EBS_CONFIG_SYSCONF_TARGET_ARCH}" )

        if( ${hostToolsetArch} MATCHES "${_EBS_MATCH_STR_ARCH_ARMF}" AND ${targetArch} MATCHES "${_EBS_MATCH_STR_ARCH_X86F}" )
            set( targetToolsetArchMismatch TRUE )
        endif()

        if( ${hostToolsetArch} MATCHES "${_EBS_MATCH_STR_ARCH_X86F}" AND ${targetArch} MATCHES "${_EBS_MATCH_STR_ARCH_ARMF}" )
            set( targetToolsetArchMismatch TRUE )
        endif()

        if( targetToolsetArchMismatch )
            message( "Warning: Specified target arch (${targetArch}) does not match toolset arch (${hostToolsetArch})" )
            unset( EBS_CONFIG_SYSCONF_TARGET_ARCH )
        endif()

	endif()

	# Check if specified ARCHBIT is supported by the platform.
    # EBSVAR_Sysconf_Target_Archdefault{ARCHBIT} should be defined.
    if( EBS_CONFIG_SYSCONF_TARGET_ARCHBIT AND NOT EBSVAR_Sysconf_Target_Archdefault${EBS_CONFIG_SYSCONF_TARGET_ARCHBIT} )
        # Requested bit-ness is not supported on that platform. Unset
        # the variable to automatically fallback to the default value.
        unset( EBS_CONFIG_SYSCONF_TARGET_ARCHBIT )
    endif()

    #
    if( NOT EBS_CONFIG_SYSCONF_TARGET_ARCH )
        #
        if( NOT EBS_CONFIG_SYSCONF_TARGET_ARCHBIT )
            #
            set( EBS_CONFIG_SYSCONF_TARGET_ARCHBIT ${EBSVAR_Sysconf_Target_ArchDefaultBit} )
        endif()

        #
        set( EBS_CONFIG_SYSCONF_TARGET_ARCH ${EBSVAR_Sysconf_Target_ArchDefault${EBS_CONFIG_SYSCONF_TARGET_ARCHBIT}} )
    endif()
    
    _ebsVmsSysconfSetInternal( "Target.Arch" ${EBS_CONFIG_SYSCONF_TARGET_ARCH} )
        
	if( ${EBSVAR_Sysconf_Target_Arch} MATCHES "${_EBS_VINTERNALDEF_MATCH_STR_ARCH_32}" )
        _ebsVmsSysconfSetInternal( "Target.ArchBit" 32 )
    elseif( ${EBSVAR_Sysconf_Target_Arch} MATCHES "${_EBS_VINTERNALDEF_MATCH_STR_ARCH_64}" )
        _ebsVmsSysconfSetInternal( "Target.ArchBit" 64 )
    endif()

endfunction()

# Validate ARCH setup and unset them if both 32 and 64 is set.
function( _ebsSysconfDetectBuildcfg )

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
		_ebsVmsSysconfSetInternal( "Target.BuildConfig" "Release" )
		if( hasDebStr )
			_ebsVmsSysconfSetInternal( "Target.FlagEnableDebugInfo" TRUE )
		endif()
	else()
		_ebsVmsSysconfSetInternal( "Target.BuildConfig" "Debug" )
		_ebsVmsSysconfSetInternal( "Target.FlagEnableDebugInfo" TRUE )
	endif()

endfunction()
