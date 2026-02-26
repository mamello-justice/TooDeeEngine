add_subdirectory(core)
add_subdirectory(inih)
add_subdirectory(sfml)

if(TOO_DEE_ENGINE_BUILD_EDITOR)
    add_subdirectory(imgui)
    add_subdirectory(imgui-sfml)
endif()
