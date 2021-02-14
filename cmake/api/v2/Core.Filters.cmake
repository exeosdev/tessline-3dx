
#@FunDef ebsCoreFilterParseGroup
#@DocStr Parses single filter group string (key and values). Translates it to a separate key string and list of specified values.
#@DocStr Example: ebsCoreFilterParseGroup( "Compiler=GCC|MSVC" K V ) -> K="Compiler", V="GCC;MSVC".
function( ebsCoreFilterParseGroup pFilterGroup outFilterKey outFilterValueList )
	# Split the filter name and its values.
	ebsCoreUtilStringSplit( "${pFilterGroup}" "=" tempList )

	list( LENGTH tempList tempListLength )

	if( ( NOT tempListLength ) OR ( ${tempListLength} LESS 2 ) )
		return()
	endif()

	list( GET tempList 0 filterKey )
	list( GET tempList 1 filterValueStr )

	ebsCoreUtilStringSplit( "${filterValueStr}" "|" filterValueList )

	list( LENGTH filterValueList filterValueListLength )

	if( ( NOT filterValueListLength ) OR ( ${filterValueListLength} LESS 1 ) )
		return()
	endif()
	
    set( ${outFilterKey} "${filterKey}" PARENT_SCOPE )
    set( ${outFilterValueList} "${filterValueList}" PARENT_SCOPE )

endfunction()

#@FunDef ebsCoreFilterParseVarargInput
#@DocStr 
#@DocStr Example: ebsCoreFilterParseVarargInput( "Compiler=GCC|MSVC System=Windows" R ) -> R="Compiler:GCC;Compiler:MSVC;System:Windows".
function( ebsCoreFilterParseVarargInput outFilterStringList )
    
    if( ${ARGC} GREATER 0 )
		foreach( filterGroup ${ARGN} )
			#
			ebsCoreFilterParseGroup( "${filterGroup}" filterKey filterValueList )
			
			foreach( filterValue ${filterValueList} )
				ebsCoreUtilListAppend( resultList "${filterKey}:${filterValue}" XUNIQUE )
			endforeach()

		endforeach()
    endif()

    set( ${outFilterStringList} "${resultList}" PARENT_SCOPE )

endfunction()

#@FunDef ebsCoreFilterCheckPlatformMatch
#@DocStr Checks if the filter, specified as $pFilterList, matches the current environment, described by $pCurrentEnvList.
#@DocStr Example1:
#@DocStr Example2:
function( ebsCoreFilterCheckPlatformMatch pFilterStringList outMatchResult )
	list( LENGTH pFilterStringList filtersNum )
	# Single identifier means group (Config|Global already checked).

	if( NOT ${filtersNum} EQUAL 0 )
		#
		foreach( filterStr ${pFilterStringList} )
			if( "${filterStr}" MATCHES "(Compiler|System|Arch|BuildCfg)\\:([a-zA-Z][a-zA-Z0-9_-]+)" )
				set( ${CMAKE_MATCH_1}FilterSet TRUE )
				set( ${CMAKE_MATCH_1}FilterList "${filterStr};${${CMAKE_MATCH_1}FilterList}" )
			else()
				ebsCoreLogWarningGeneric( "ebsCoreFilterCheckCurrentPlatformMatch: Unknown filter '${filterStr}'" )
			endif()
		endforeach()

		# CompilerFilterSet is the ${CMAKE_MATCH_1}FilterSet set if at least one filter for the compiler has been found.
		if( CompilerFilterSet AND NOT ( "Compiler:${EBS_AUX_HOST_COMPILER}" IN_LIST CompilerFilterList ) )
			return()
		endif()
		# SystemFilterSet is the ${CMAKE_MATCH_1}FilterSet set if at least one filter for the system has been found.
		if( SystemFilterSet AND NOT ( "System:${EBS_AUX_TARGET_SYSTEM}" IN_LIST SystemFilterList ) )
			return()
		endif()
		# ArchFilterSet is the ${CMAKE_MATCH_1}FilterSet set if at least one filter for the arch has been found.
		if( ArchFilterSet AND NOT ( "Arch:${EBS_AUX_TARGET_ARCH}" IN_LIST ArchFilterList ) )
			return()
		endif()
		# BuildCfgFilterSet is the ${CMAKE_MATCH_1}FilterSet set if at least one filter for the compiler has been found.
		if( BuildCfgFilterSet AND NOT ( "Buildcfg:${EBS_AUX_TARGET_BUILDCFG}" IN_LIST BuildcfgFilterList ) )
			return()
		endif()
	endif()
	
	set( ${outMatchResult} TRUE PARENT_SCOPE )

endfunction()
