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

# MSVC hardening flags
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /ZH:SHA_256 /W4 /WX /Zi /FS /GS /sdl /Qspectre /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /ZH:SHA_256 /W4 /WX /Zi /FS /GS /sdl /Qspectre /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} /CETCOMPAT /NXCOMPAT /DYNAMICBASE /HIGHENTROPYVA /guard:cf /debug:full")
set(CMAKE_MODULE_LINKER_FLAGS  "${CMAKE_MODULE_LINKER_FLAGS} /CETCOMPAT /NXCOMPAT /DYNAMICBASE /HIGHENTROPYVA /guard:cf /debug:full")

# Use dynamic runtime and keep it config-aware.
# Respect parent-project policy when already defined.
if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    # Debug -> /MDd, non-Debug -> /MD.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

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
target_include_directories(ClientApiLib PUBLIC ${CLIENTAPI_INCLUDE_DIRS})
target_link_libraries(ClientApiLib PUBLIC nlohmann_json::nlohmann_json)

# ClientApi - shared library (DLL)
add_library(ClientApi SHARED ${CLIENTAPI_SOURCES})
set_target_properties(ClientApi PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_include_directories(ClientApi PUBLIC ${CLIENTAPI_INCLUDE_DIRS})
target_link_libraries(ClientApi PUBLIC nlohmann_json::nlohmann_json)
