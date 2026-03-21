include(FetchContent)

option(MC_OFFLINE_FETCHCONTENT "Use cached FetchContent dependencies only." ON)
set(FETCHCONTENT_UPDATES_DISCONNECTED ${MC_OFFLINE_FETCHCONTENT} CACHE BOOL "" FORCE)

set(MC_GLFW_GIT_TAG "3.3.8")
set(MC_GLM_GIT_TAG "1.0.1")
set(MC_JSON_GIT_TAG "v3.11.2")
set(MC_GL3W_GIT_TAG "d5ba9340cdeb9154323817f5c87e5a5c377fdef7")
set(MC_RAPIDCHECK_GIT_TAG "ff6af6fc683159deb51c543b065eba14dfcf329b")

function(mc_require_cached_dep dep_name)
    if(NOT MC_OFFLINE_FETCHCONTENT)
        return()
    endif()

    set(dep_path "${CMAKE_BINARY_DIR}/_deps/${dep_name}-src")
    if(NOT EXISTS "${dep_path}")
        message(FATAL_ERROR
            "Offline dependency mode is enabled, but '${dep_name}' is not cached at '${dep_path}'. "
            "Run CMake once with -DMC_OFFLINE_FETCHCONTENT=OFF on a machine with network access.")
    endif()
endfunction()

message(STATUS "MC_OFFLINE_FETCHCONTENT=${MC_OFFLINE_FETCHCONTENT}")

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG ${MC_GLFW_GIT_TAG}
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
mc_require_cached_dep(glfw)
FetchContent_MakeAvailable(glfw)

# GLM (header-only)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG ${MC_GLM_GIT_TAG}
)
mc_require_cached_dep(glm)
FetchContent_MakeAvailable(glm)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG ${MC_JSON_GIT_TAG}
)
mc_require_cached_dep(json)
FetchContent_MakeAvailable(json)

# OpenGL loader (gl3w)
FetchContent_Declare(
    gl3w
    GIT_REPOSITORY https://github.com/skaslev/gl3w.git
    GIT_TAG ${MC_GL3W_GIT_TAG}
)
mc_require_cached_dep(gl3w)
FetchContent_MakeAvailable(gl3w)

# rapidcheck (property-based testing)
FetchContent_Declare(
    rapidcheck
    GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
    GIT_TAG ${MC_RAPIDCHECK_GIT_TAG}
)
set(RC_ENABLE_GTEST OFF CACHE BOOL "" FORCE)
set(RC_ENABLE_GMOCK OFF CACHE BOOL "" FORCE)
mc_require_cached_dep(rapidcheck)
FetchContent_MakeAvailable(rapidcheck)
