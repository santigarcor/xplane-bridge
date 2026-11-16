# Native X-Plane 12 plugin for the Arduino panel. Windows only, built with MinGW-w64 GCC
# (MSYS2), following the layout of winctrl-xplane-plugin. Run build.ps1 rather than calling
# cmake directly: it reads SDK_VERSION from the SDK headers and installs the result.
CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g1")
PROJECT(xplane_bridge C CXX)

IF(NOT WIN32)
    MESSAGE(FATAL_ERROR "Only Windows is supported")
ENDIF()

IF(NOT DEFINED SDK_VERSION)
    MESSAGE(FATAL_ERROR "SDK_VERSION not defined (build.ps1 reads it from SDK/CHeaders/XPLM/XPLMDefs.h)")
ENDIF()

IF(SDK_VERSION LESS 420)
    MESSAGE(FATAL_ERROR "XPLM420 or newer is required (X-Plane 12.3.0+), got ${SDK_VERSION}")
ENDIF()

SET(DIRECTORY_PREFIX "win_x64")
SET(XPLANE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDK/Libraries/Win/")
SET(XPLANE_INCLUDES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDK/CHeaders/")

FIND_LIBRARY(XPLM_LIBRARY XPLM_64 PATHS "${XPLANE_LIBRARY_PATH}" NO_DEFAULT_PATH REQUIRED)

# Every folder under src/ that holds a header becomes an include path, so includes are flat ("bridge.h")
FUNCTION(HEADER_DIRECTORIES return_list base_dir)
    FILE(GLOB_RECURSE header_files "${base_dir}/*.h" "${base_dir}/*.hpp")
    SET(dir_list "")
    FOREACH(file_path ${header_files})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        LIST(APPEND dir_list ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(FIND_SOURCE_FILES return_list base_dir)
    FILE(GLOB_RECURSE source_files CONFIGURE_DEPENDS "${base_dir}/*.c" "${base_dir}/*.cpp")
    SET(${return_list} ${source_files} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(add_xplane_sdk_definitions library_name library_version)
    TARGET_COMPILE_DEFINITIONS(${library_name} PUBLIC APL=0 IBM=1 LIN=0)

    # XPLM200 ... XPLMxxx up to the SDK version, as the SDK headers expect
    FOREACH(version 200 210 300 301 400 410 411 420 421 430 440)
        IF(NOT version GREATER library_version)
            TARGET_COMPILE_DEFINITIONS(${library_name} PUBLIC XPLM${version}=1)
        ENDIF()
    ENDFOREACH()

    TARGET_INCLUDE_DIRECTORIES(${library_name} PUBLIC "${XPLANE_INCLUDES_PATH}/XPLM" "${XPLANE_INCLUDES_PATH}/Wrappers")
ENDFUNCTION()

FUNCTION(add_xplane_plugin library_name library_version src_dir)
    MESSAGE(STATUS "Preparing X-Plane plugin '${library_name}' (SDK ${library_version}) at ${src_dir}")
    FIND_SOURCE_FILES(FILES ${src_dir})

    ADD_LIBRARY(${library_name} MODULE ${FILES})
    add_xplane_sdk_definitions(${library_name} ${library_version})

    # X-Plane 12 layout: <plugin folder>/win_x64/<name>.xpl. Set through the output directory
    # rather than a slash in OUTPUT_NAME, which Ninja rejects as a duplicate rule.
    SET_TARGET_PROPERTIES(${library_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY_PREFIX}"
        OUTPUT_NAME "${library_name}"
        PREFIX ""
        SUFFIX ".xpl"
        LINKER_LANGUAGE CXX)

    TARGET_COMPILE_DEFINITIONS(${library_name} PRIVATE _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
    TARGET_COMPILE_OPTIONS(${library_name} PRIVATE -Wall -Wextra)
    # Static runtime: the .xpl must not depend on MinGW DLLs that X-Plane can't find
    TARGET_LINK_OPTIONS(${library_name} PRIVATE -static-libstdc++ -static-libgcc -static)

    TARGET_LINK_LIBRARIES(${library_name} PRIVATE ${XPLM_LIBRARY} setupapi advapi32)

    HEADER_DIRECTORIES(header_dir_list ${src_dir})
    TARGET_INCLUDE_DIRECTORIES(${library_name} PRIVATE ${header_dir_list})
ENDFUNCTION()

add_xplane_plugin(xplane_bridge ${SDK_VERSION} "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Host test: the bridge and serial code in a console exe, against a stub XPLM (tests/xplm-stub.cpp).
# Runs without X-Plane and talks to the real Arduino when one is plugged in. Not shipped.
OPTION(XPLANE_BRIDGE_HOST_TESTS "Build tests/host-test.exe" OFF)
IF(XPLANE_BRIDGE_HOST_TESTS)
    FILE(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.cpp")
    # Need the real menu and plugin SDK, which the stub doesn't provide
    LIST(FILTER TEST_SOURCES EXCLUDE REGEX ".*/(appstate|plugins-menu)\\.cpp$")

    ADD_EXECUTABLE(host-test tests/host-test.cpp tests/xplm-stub.cpp ${TEST_SOURCES})
    add_xplane_sdk_definitions(host-test ${SDK_VERSION})
    # XPLM=1 makes the SDK headers declare the API dllexport instead of dllimport, so the stub
    # can define it in the same executable
    TARGET_COMPILE_DEFINITIONS(host-test PRIVATE XPLM=1 _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
    TARGET_COMPILE_OPTIONS(host-test PRIVATE -Wall -Wextra)
    TARGET_LINK_OPTIONS(host-test PRIVATE -static-libstdc++ -static-libgcc -static)
    TARGET_LINK_LIBRARIES(host-test PRIVATE setupapi advapi32)
    HEADER_DIRECTORIES(test_header_dirs "${CMAKE_CURRENT_SOURCE_DIR}/src")
    TARGET_INCLUDE_DIRECTORIES(host-test PRIVATE ${test_header_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/tests")
ENDIF()
