
# @internal
#
function( _ebsCoreFltParseFilterVarargInput outFilterStringList )
    
    if( ${ARGC} GREATER 0 )
        #
        set( varArgList "COMPILER" "SYSTEM" "SYSAPI" "ARCH" "BUILDCFG" )
        # Parse arguments. All filters can be specified with multiple values each.
        cmake_parse_arguments( FLT "" "" "${varArgList}" "${ARGN}" )

        if( FLT_UNPARSED_ARGUMENTS )
            message( WARNING "<:BUILD:> Filters.ParseVararg: unknown filters: ${FLT_UNPARSED_ARGUMENTS}. Allowed: ${varArgList}" )
        endif()

        if( FLT_COMPILER OR FLT_SYSTEM OR FLT_SYSAPI OR FLT_ARCH OR FLT_BUILDCFG )

            foreach( compiler "${FLT_COMPILER}" )
                if( compiler )
                    ebsCommonUtilListAppend( filterStringList "Compiler.${compiler}" XUNIQUE )
                endif()
            endforeach()
            foreach( system "${FLT_SYSTEM}" )
                if( system )
                    ebsCommonUtilListAppend( filterStringList "System.${system}" XUNIQUE )
                endif()
            endforeach()
            foreach( sysAPI "${FLT_SYSAPI}" )
                if( sysAPI )
                    ebsCommonUtilListAppend( filterStringList "SysAPI.${sysAPI}" XUNIQUE )
                endif()
            endforeach()
            foreach( arch "${FLT_ARCH}" )
                if( arch )
                    ebsCommonUtilListAppend( filterStringList "Arch.${arch}" XUNIQUE )
                endif()
            endforeach()
            foreach( buildcfg "${FLT_BUILDCFG}" )
                if( buildcfg )
                    ebsCommonUtilListAppend( filterStringList "Buildcfg.${buildcfg}" XUNIQUE )
                endif()
            endforeach()

            set( ${outFilterStringList} "${filterStringList}" PARENT_SCOPE )
        endif()

    endif()

endfunction()

#
#
function( _ebsCoreFltCheckFilterMatch pFilterStringList pCompiler pSystem pSysAPI pArch pBuildcfg outCheckResult )

    foreach( filterStr "${pFilterStringList}" )
        if( "${filterStr}" MATCHES "Compiler.[a-zA-Z0-9_-]+" )
            set( compilerFilterSet TRUE )
        elseif( "${filterStr}" MATCHES "System.[a-zA-Z0-9_-]+" )
            set( systemFilterSet TRUE )
        elseif( "${filterStr}" MATCHES "SysAPI.[a-zA-Z0-9_-]+" )
            set( sysAPIFilterSet TRUE )
        elseif( "${filterStr}" MATCHES "Arch.[a-zA-Z0-9_-]+" )
            set( archFilterSet TRUE )
        elseif( "${filterStr}" MATCHES "Buildcfg.[a-zA-Z0-9_-]+" )
            set( buildcfgFilterSet TRUE )
        endif()
    endforeach()

    if( NOT compilerFilterSet OR ( "Compiler.${pCompiler}" IN_LIST pFilterStringList ) )
        set( compilerSupport TRUE )
    endif()

    if( NOT systemFilterSet OR ( "System.${pSystem}" IN_LIST pFilterStringList ) )
        set( systemSupport TRUE )
    endif()

    if( NOT sysAPIFilterSet OR ( "SysAPI.${pSysAPI}" IN_LIST pFilterStringList ) )
        set( sysAPISupport TRUE )
    endif()

    if( NOT archFilterSet OR ( "Arch.${pArch}" IN_LIST pFilterStringList ) )
        set( archSupport TRUE )
    endif()

    if( NOT buildcfgFilterSet OR ( "Buildcfg.${pBuildcfg}" IN_LIST pFilterStringList ) )
        set( buildcfgSupport TRUE )
    endif()

    if( compilerSupport AND systemSupport AND sysAPISupport AND archSupport AND buildcfgSupport )
        set( ${outCheckResult} TRUE PARENT_SCOPE )
    endif()

endfunction()
