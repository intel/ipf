# ============================================================================
# IPF-EF Client Library
#
# Builds two targets:
#   ClientApiLib  - static library
#   ClientApi     - shared library (the loadable ClientApi DLL)
# ============================================================================
cmake_minimum_required(VERSION 3.15)
project(IpfEfClientApi CXX)

if(NOT MSVC)
    message(FATAL_ERROR "The IPF-EF ClientApi supports the MSVC (Windows) toolchain only.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_QSPECTRE "Enable /Qspectre mitigation" ON)

# MSVC hardening flags
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /ZH:SHA_256 /W4 /WX /Zi /FS /GS /sdl /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /ZH:SHA_256 /W4 /WX /Zi /FS /GS /sdl /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
if(ENABLE_QSPECTRE)
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /Qspectre")
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /Qspectre")
endif()
set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} /CETCOMPAT /NXCOMPAT /DYNAMICBASE /HIGHENTROPYVA /guard:cf /debug:full")
set(CMAKE_MODULE_LINKER_FLAGS  "${CMAKE_MODULE_LINKER_FLAGS} /CETCOMPAT /NXCOMPAT /DYNAMICBASE /HIGHENTROPYVA /guard:cf /debug:full")

# Always use the dynamic release runtime for framework compatibility
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# Get nlohmann/json library.
# Reuse an nlohmann_json target already provided by the embedding project (e.g. OpenVINO's
# thirdparty/json) to avoid a duplicate target definition (CMP0002). Only fetch a bundled
# copy when no such target exists (standalone IPF builds).
if(NOT TARGET nlohmann_json AND NOT TARGET nlohmann_json::nlohmann_json)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03  # v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()
set(CLIENTAPI_SOURCES
    extras/c_wrapper/ClientApiCJson.cpp
    src/clientapi/ClientApi.cpp
    src/shared/DllProperties.cpp
    src/shared/win/StringUtils.cpp
)

set(CLIENTAPI_INCLUDE_DIRS
    extras/c_wrapper
    src/clientapi
    src/shared
)

# ClientApiLib - static library
add_library(ClientApiLib STATIC ${CLIENTAPI_SOURCES})
target_include_directories(ClientApiLib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extras/c_wrapper>
                                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/clientapi>
                                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/shared>)
target_link_libraries(ClientApiLib PRIVATE nlohmann_json::nlohmann_json)

# ClientApi - shared library (DLL)
add_library(ClientApi SHARED ${CLIENTAPI_SOURCES})
set_target_properties(ClientApi PROPERTIES CXX_VISIBILITY_PRESET hidden)
# Use BUILD_INTERFACE absolute paths so in-tree consumers get the headers while
# the export/install of ClientApi carries no relative include dirs (they are
# automatically stripped from the install interface). These headers are internal
# to IPF and are intentionally not published as an install interface.
target_include_directories(ClientApi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extras/c_wrapper>
                                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/clientapi>
                                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/shared>)
target_link_libraries(ClientApi PRIVATE nlohmann_json::nlohmann_json)
