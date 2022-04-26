
set( TS3_TOOLS_LINK_INPUT
        ts3-core
        ts3-engine
        ts3-system
        )

set( TS3_TOOLS_LIST
        "scf-cli-builder"
        )

foreach( module ${TS3_TOOLS_LIST} )
    #add_subdirectory( "${module}" )
endforeach()
