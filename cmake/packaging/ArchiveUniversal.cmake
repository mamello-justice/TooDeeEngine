# Archives the contents of the current working directory.
# Invoked by PackUniversal.cmake with WORKING_DIRECTORY set to the staging
# directory, so that archive entries stay relative to it.

if(NOT DEFINED ARCHIVE_OUTPUT)
    message(FATAL_ERROR "ARCHIVE_OUTPUT is required")
endif()

if(DEFINED ARCHIVE_FORMAT AND ARCHIVE_FORMAT STREQUAL "tar")
    set(_compression_args COMPRESSION GZip)
    set(_format tar)
else()
    set(_compression_args "")
    set(_format zip)
endif()

file(GLOB_RECURSE _files LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*")

if(NOT _files)
    message(FATAL_ERROR "Nothing to archive in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

file(ARCHIVE_CREATE
    OUTPUT "${ARCHIVE_OUTPUT}"
    FORMAT ${_format}
    ${_compression_args}
    PATHS ${_files})
