add_subdirectory(core)
add_subdirectory(inih)
add_subdirectory(sfml)
add_subdirectory(engine)

if (TOO_DEE_ENGINE_QJS_SCRIPTING)
    add_subdirectory(quickjs)
endif()

if(TOO_DEE_ENGINE_BUILD_EDITOR)
    add_subdirectory(imgui)
    add_subdirectory(imgui-sfml)
endif()
