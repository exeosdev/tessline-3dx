
cmake_minimum_required( VERSION 3.8 )

cmake_policy(SET CMP0054 NEW)

include( "ExeosBuildSystem/cmake/Core.Util.cmake" )
include( "ExeosBuildSystem/cmake/Core.Log.cmake" )
include( "ExeosBuildSystem/cmake/Core.Registry.cmake" )
include( "ExeosBuildSystem/cmake/Core.Filters.cmake" )
include( "ExeosBuildSystem/cmake/Vms.Common.cmake" )
include( "ExeosBuildSystem/cmake/Vms.Impl.Sysconf.cmake" )
include( "ExeosBuildSystem/cmake/Vms.Impl.Workspace.cmake" )

ebsVmsBaseRegistryResetSystem()

ebsSysconfInitialize( "." )
