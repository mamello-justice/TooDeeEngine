cmake_minimum_required(VERSION 3.28)

project(LuaCppIntegration CXX)

# Get LuaBinaries and paste here
# e.g. https://sourceforge.net/projects/luabinaries/files/

# Options
set(LUA_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LUA_LIB_FILE     "${CMAKE_CURRENT_SOURCE_DIR}/lua54.lib")
set(LUA_DLL_FILE     "${CMAKE_CURRENT_SOURCE_DIR}/lua54.dll")

## Add Library
add_library(lua SHARED IMPORTED GLOBAL)

set_target_properties(lua PROPERTIES
    # Header directory that will automatically propagate to anything linking 'lua'
    INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
    
    # Path to the MinGW import/static library (.lib file)
    IMPORTED_IMPLIB "${LUA_LIB_FILE}"
    
    # Path to the actual runtime binary (.dll file)
    IMPORTED_LOCATION "${LUA_DLL_FILE}"
)