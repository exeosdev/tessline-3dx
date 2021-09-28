
setInternal( CM_ANDROID_NATIVE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/main/cpp/ts3-native-activity )
setInternal( CM_ANDROID_COMMON_INCLUDE_DIR
        ${CM_ANDROID_NATIVE_DIR} )

add_subdirectory( "main/cpp/ts3-native-activity" )
