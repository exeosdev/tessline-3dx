
set( APP_SAMPLE_MAIN_ENTRY_POINT_LIB "ts3sample-sysTest" )

set( CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -std=gnu++17 -Wall -Werror" )

set( CMAKE_SHARED_LINKER_FLAGS
		"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate" )

add_library( ts3-native-activity SHARED
		ts3-native-activity.h
		ts3-native-activity.cpp )

# add lib dependencies
target_link_libraries( ts3-native-activity
		android
		log
		EGL
		GLESv3
		ts3-system
		"${APP_SAMPLE_MAIN_ENTRY_POINT_LIB}" )
