
#@FunDef ebsUtilListAppend
#@DocStr Appends specified content to the list.
#@DocStr -- Optional modifiers:
#@DocStr -- XUNIQUE: appends value only if it iss not already on the list.
#@DocStr -- XSORT:   after all contant is added, the list is sorted.
#@DocStr -- XCACHE:  the output list is set in 'CACHE INTERNAL' mode.
function( ebsUtilListAppend pListName pContent )

    if( NOT pContent )
        return()
    endif()

    set( modifierList "XUNIQUE" "XSORT" "XCACHE" )
    cmake_parse_arguments( MODIFIER "${modifierList}" "" "" "${ARGN}" )
	
	# Do branching here and use two separate fors. Makes difference - tested for Exeos and TessLine projects.
    if( MODIFIER_XUNIQUE )
		foreach( contentItem ${pContent} )
			# Check if current item is already on the list.
			list( FIND ${pListName} "${contentItem}" itemPos )
			# If it is (we are in XUNIQUE branch), skip it.
			if( NOT itemPos EQUAL -1 )
				continue()
			endif()
			# Append the item to the list.
			list( APPEND ${pListName} "${contentItem}" )
		endforeach()
	else()
		foreach( contentItem ${pContent} )
			list( APPEND ${pListName} "${contentItem}" )
		endforeach()
    endif()

    if( MODIFIER_XSORT )
        list( SORT ${pListName} )
    endif()

    if( MODIFIER_XCACHE )
        set( ${pListName} "${${pListName}}" CACHE INTERNAL "${pListName}" )
    else()
        set( ${pListName} "${${pListName}}" PARENT_SCOPE )
    endif()

endfunction()

#@FunDef ebsUtilListRemove
#@DocStr Removes specified content from the list.
#@DocStr -- Optional modifiers:
#@DocStr -- XFIRSTONLY: if list contains more than one occurence of a given value, only the first one is removed.
#@DocStr -- XCACHE:     the output list is set in 'CACHE INTERNAL' mode.
function( ebsUtilListRemove pListName pContent )

    if( NOT pContent )
        return()
    endif()

    set( modifierList "XFIRSTONLY" "XCACHE" )
    cmake_parse_arguments( MODIFIER "${modifierList}" "" "" "${ARGN}" )

	foreach( contentItem ${pContent} )
		while( TRUE )
			# Find the item on the list
			list( FIND ${pListName} "${contentItem}" itemPos )
			# If there is no such value, break the internal loop and continue with next item to remove.
			if( itemPos EQUAL -1 )
				break()
			endif()
			# Otherwise, remove it from the list.
			list( REMOVE_AT ${pListName} ${itemPos} )
			# If XFIRSTONLY was specified, break the internal loop after first occurence was removed.
			if( MODIFIER_XFIRSTONLY )
				break()
			endif()
		endwhile()
	endforeach()

    if( MODIFIER_XCACHE )
        set( ${pListName} "${${pListName}}" CACHE INTERNAL "${pListName}" )
    else()
        set( ${pListName} "${${pListName}}" PARENT_SCOPE )
    endif()

endfunction()

#@FunDef ebsUtilListCheckContent
#@DocStr Checks if specified content is on the list.
#@DocStr -- Optional modifiers:
#@DocStr -- XLOGICALOR: check is done in logical OR mode - TRUE is returned if at least one of specified items is found.
function( ebsUtilListCheckContent pListName pContent outResult )
    if( NOT pContent )
        return()
    endif()

    set( modifierList "XLOGICALOR" )
    cmake_parse_arguments( MODIFIER "${modifierList}" "" "" "${ARGN}" )

	foreach( contentItem ${pContent} )
		# Check if item is on the list.
		if( ${contentItem} IN_LIST ${pListName} )
			# In case of XLOGICALOR, we can return TRUE on the first occurence.
			if( MODIFIER_XLOGICALOR )
				set( ${outResult} TRUE PARENT_SCOPE )
				return()
			endif()
		else()
			# If at least one item is not there and XLOGICALOR is not set, we have a negative result.
			if( NOT MODIFIER_XLOGICALOR )
				return()
			endif()
		endif()
	endforeach()

	# Whole loop went successfuly, so all items must have been present.
	set( ${outResult} TRUE PARENT_SCOPE )
endfunction()

#@FunDef ebsUtilStringReplace
#@DocStr Replaces matched string content with another string.
function( ebsUtilStringReplace pString pMatch pReplace outResult )
    if( NOT pString )
        return()
    endif()
    string( REPLACE "${pMatch}" "${pReplace}" result ${pString} )
    set( ${outResult} "${result}" PARENT_SCOPE )
endfunction()

#@FunDef ebsUtilStringSplit
#@DocStr Splits string into a list of substrings using specified separator.
function( ebsUtilStringSplit pString pSeparator outList )
    if( NOT pString )
        return()
    endif()
	# Use the fact, that ';' is treated as a list item separator.
    string( REPLACE "${pSeparator}" ";" result ${pString} )
	# Set the list-string as a content of the output variable.
    set( ${outList} "${result}" PARENT_SCOPE )
endfunction()

#@FunDef ebsUtilPlatformQuerySystemName
#@DocStr Retrieves the name of either host or target system name. The default mode is target.
#@DocStr Possible result: WinDesktop, Linux, MacOS, Solaris, Android, iOS.
#@DocStr -- Optional modifiers:
#@DocStr -- XHOST:   retrieves the host system name.
#@DocStr -- XTARGET: retrieves the target system name. This is the default mode.
function( ebsUtilSysQueryCMakeSystemName outResult )

    set( modifierList "XHOST" "XTARGET" )
    cmake_parse_arguments( MODIFIER "${modifierList}" "" "" "${ARGN}" )

    if( MODIFIER_XHOSTOS )
        set( CMAKE_VAR_PREFIX "CMAKE_HOST_" )
    endif()

    if( ${CMAKE_VAR_PREFIX}ANDROID )
        set( systemName "Android" )
    elseif( ${CMAKE_VAR_PREFIX}SOLARIS )
        set( systemName "Solaris" )
    elseif( ${CMAKE_VAR_PREFIX}APPLE )
        if( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
            set( systemName "MacOS" )
        else()
            set( systemName "iOS" )
        endif()
    elseif( ${CMAKE_VAR_PREFIX}UNIX )
        if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
            set( systemName "Linux" )
        endif()
    elseif( ${CMAKE_VAR_PREFIX}WIN32 )
        if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
            set( systemName "WinDesktop" )
        endif()
    endif()

	if( systemName )
		set( ${outResult} "${systemName}" PARENT_SCOPE )
	endif()
    
endfunction()
