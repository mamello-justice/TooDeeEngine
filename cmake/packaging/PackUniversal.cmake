# Builds the all-in-one universal package from every per-platform archive
# found in the dist directory.
#
# Per-platform archives are produced by `cpack` (see the `package-platform`
# recipe) and named:
#   too-dee-engine-<version>-<os>-<arch>-<linkage>.<ext>
# This script collects them into a single distributable package:
#   too-dee-engine-<version>-all-universal.nupkg
#
# Usage (CMake script mode):
#   cmake -DPACKAGE_INFO_FILE=build/TooDeePackageInfo.cmake \
#         -DDIST_DIR=dist \
#         -P cmake/packaging/PackUniversal.cmake
#
# When packaging in CI, download the per-platform archives of every platform
# into DIST_DIR before running this script so the universal package is
# genuinely all-in-one.

cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED PACKAGE_INFO_FILE)
    get_filename_component(_source_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(PACKAGE_INFO_FILE "${_source_root}/build/TooDeePackageInfo.cmake")
endif()

if(NOT DEFINED DIST_DIR)
    set(DIST_DIR "${CMAKE_CURRENT_LIST_DIR}/../../dist")
endif()

# nupkg (default), zip or tar.gz
if(NOT DEFINED OUTPUT_FORMAT)
    set(OUTPUT_FORMAT "nupkg")
endif()

# nupkg: name the file <id>.<version>.nupkg so package managers that validate
# the file name against the nuspec (e.g. nuget push) accept it.
if(NOT DEFINED STRICT_NUGET_NAME)
    set(STRICT_NUGET_NAME OFF)
endif()

if(NOT EXISTS "${PACKAGE_INFO_FILE}")
    message(FATAL_ERROR "Package info file not found: ${PACKAGE_INFO_FILE}")
endif()

include("${PACKAGE_INFO_FILE}")

get_filename_component(_dist_dir "${DIST_DIR}" ABSOLUTE)
if(NOT IS_DIRECTORY "${_dist_dir}")
    message(FATAL_ERROR "Dist directory not found: ${_dist_dir}")
endif()

set(_license_file "${TOO_DEE_ENGINE_LICENSE_FILE}")
if(NOT EXISTS "${_license_file}")
    get_filename_component(_source_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(_license_file "${_source_root}/LICENSE")
endif()

# ===== COLLECT PER-PLATFORM PAYLOADS =====

file(GLOB _candidates
    "${_dist_dir}/${TOO_DEE_ENGINE_PACKAGE_ID}-${TOO_DEE_ENGINE_VERSION}-*")

set(_payloads "")
set(_platforms "")
set(_payload_labels "")
foreach(_candidate IN LISTS _candidates)
    get_filename_component(_name "${_candidate}" NAME)
    if(IS_DIRECTORY "${_candidate}")
        continue()
    endif()
    if(_name MATCHES "^${TOO_DEE_ENGINE_PACKAGE_ID}-${TOO_DEE_ENGINE_VERSION}-(windows|linux|macos)-([a-z0-9_]+)-([a-z]+)\\.(tar\\.gz|zip|dmg|msi|deb)$")
        set(_platform "${CMAKE_MATCH_1}-${CMAKE_MATCH_2}-${CMAKE_MATCH_3}")
        list(APPEND _payloads "${_candidate}")
        list(APPEND _platforms "${_platform}")
        list(APPEND _payload_labels "tools/${_platform}/${_name}")
    endif()
endforeach()

list(LENGTH _payloads _payload_count)
if(_payload_count EQUAL 0)
    message(FATAL_ERROR
        "No per-platform archives matching "
        "'${TOO_DEE_ENGINE_PACKAGE_ID}-${TOO_DEE_ENGINE_VERSION}-<os>-<arch>-<linkage>.<ext>' "
        "were found in ${_dist_dir}")
endif()

message(STATUS "Universal package payloads (${_payload_count}):")
foreach(_label IN LISTS _payload_labels)
    message(STATUS "  ${_label}")
endforeach()

# ===== STAGE PACKAGE CONTENT =====

set(_staging "${_dist_dir}/.universal-staging")
file(REMOVE_RECURSE "${_staging}")
file(MAKE_DIRECTORY "${_staging}/tools")
file(MAKE_DIRECTORY "${_staging}/_rels")

math(EXPR _payload_last "${_payload_count} - 1")
foreach(_i RANGE 0 ${_payload_last})
    list(GET _payloads ${_i} _payload)
    list(GET _platforms ${_i} _platform)
    file(MAKE_DIRECTORY "${_staging}/tools/${_platform}")
    file(COPY "${_payload}" DESTINATION "${_staging}/tools/${_platform}")
endforeach()

if(EXISTS "${_license_file}")
    file(COPY "${_license_file}" DESTINATION "${_staging}")
    set(_license_element "    <license type=\"file\">LICENSE</license>\n")
    set(_license_entry "    <file src=\"LICENSE\" target=\"\" />\n")
else()
    set(_license_element "")
    set(_license_entry "")
endif()

# ===== NUSPEC / OPC METADATA =====

file(WRITE "${_staging}/${TOO_DEE_ENGINE_PACKAGE_ID}.nuspec"
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<package xmlns=\"http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd\">
  <metadata minClientVersion=\"3.3\">
    <id>${TOO_DEE_ENGINE_PACKAGE_ID}</id>
    <version>${TOO_DEE_ENGINE_VERSION}</version>
    <title>${TOO_DEE_ENGINE_DISPLAY_NAME}</title>
    <authors>${TOO_DEE_ENGINE_VENDOR}</authors>
    <owners>${TOO_DEE_ENGINE_VENDOR}</owners>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <projectUrl>${TOO_DEE_ENGINE_HOMEPAGE_URL}</projectUrl>
    <description>All-in-One universal package of ${TOO_DEE_ENGINE_DESCRIPTION}. Contains one payload per platform under tools/&lt;os&gt;-&lt;arch&gt;-&lt;linkage&gt;/.</description>
    <summary>${TOO_DEE_ENGINE_DESCRIPTION}</summary>
    <releaseNotes>${TOO_DEE_ENGINE_HOMEPAGE_URL}/releases/tag/v${TOO_DEE_ENGINE_VERSION}</releaseNotes>
    <copyright>Copyright (c) 2026 Mamello Justice</copyright>
    <tags>game engine c++ sfml too-dee-engine</tags>
${_license_element}  </metadata>
  <files>
    <file src=\"tools/**\" target=\"tools\" />
${_license_entry}  </files>
</package>
")

file(WRITE "${_staging}/[Content_Types].xml"
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">
  <Default Extension=\"nuspec\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\" />
  <Default Extension=\"xml\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"zip\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"gz\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"dmg\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"msi\" ContentType=\"application/octet-stream\" />
  <Default Extension=\"deb\" ContentType=\"application/octet-stream\" />
</Types>
")

file(WRITE "${_staging}/_rels/.rels"
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">
  <Relationship Id=\"Rd1e2e3e4e5e6e7e8e9\" Type=\"http://schemas.microsoft.com/packaging/2010/07/manifest\" Target=\"/${TOO_DEE_ENGINE_PACKAGE_ID}.nuspec\" />
</Relationships>
")

# ===== ARCHIVE =====

if(OUTPUT_FORMAT STREQUAL "tar.gz")
    set(_extension "tar.gz")
    set(_archive_format "tar")
elseif(OUTPUT_FORMAT STREQUAL "zip")
    set(_extension "zip")
    set(_archive_format "zip")
else()
    set(_extension "nupkg")
    set(_archive_format "zip")
endif()

if(STRICT_NUGET_NAME AND _extension STREQUAL "nupkg")
    set(_output "${_dist_dir}/${TOO_DEE_ENGINE_PACKAGE_ID}.${TOO_DEE_ENGINE_VERSION}.nupkg")
else()
    set(_output "${_dist_dir}/${TOO_DEE_ENGINE_UNIVERSAL_ARTIFACT_NAME}.${_extension}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        "-DARCHIVE_OUTPUT=${_output}.tmp"
        "-DARCHIVE_FORMAT=${_archive_format}"
        -P "${CMAKE_CURRENT_LIST_DIR}/ArchiveUniversal.cmake"
    WORKING_DIRECTORY "${_staging}"
    RESULT_VARIABLE _archive_result
)
if(NOT _archive_result EQUAL 0)
    message(FATAL_ERROR "Failed to archive ${_staging} (exit code ${_archive_result})")
endif()

file(RENAME "${_output}.tmp" "${_output}" RESULT _rename_result)
if(NOT _rename_result EQUAL 0)
    message(FATAL_ERROR "Failed to write ${_output}: ${_rename_result}")
endif()

file(REMOVE_RECURSE "${_staging}")

message(STATUS "Universal package: ${_output}")
