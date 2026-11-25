cmake_minimum_required(VERSION 3.20)

project(SPACE_SURVIVOR VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SPACE_SURVIVOR_BUILD_TESTS "Build the test executables" ON)
option(SPACE_SURVIVOR_FETCH_SFML "Download SFML 2.6.1 when it is not installed" ON)

find_package(SFML 2.6.1 EXACT COMPONENTS graphics window system audio QUIET)

if(NOT SFML_FOUND)
    if(NOT SPACE_SURVIVOR_FETCH_SFML)
        message(FATAL_ERROR "SFML 2.6.1 is required. Install it or enable SPACE_SURVIVOR_FETCH_SFML.")
    endif()

    include(FetchContent)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SFML shared libraries" FORCE)
    set(SFML_BUILD_GRAPHICS ON CACHE BOOL "Build SFML graphics" FORCE)
    set(SFML_BUILD_WINDOW ON CACHE BOOL "Build SFML window" FORCE)
    set(SFML_BUILD_AUDIO ON CACHE BOOL "Build SFML audio" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "Skip SFML network module" FORCE)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Skip SFML examples" FORCE)
    set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "Skip SFML tests" FORCE)

    # AUDIT-019: Fetch the exact SFML release instead of accepting an incompatible major version.
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SFML)
endif()

if(TARGET sfml-graphics)
    set(SPACE_SURVIVOR_SFML_LIBS sfml-graphics sfml-window sfml-system sfml-audio)
    set(SPACE_SURVIVOR_SFML_RUNTIME_TARGETS
        sfml-graphics sfml-window sfml-system sfml-audio)
elseif(TARGET SFML::Graphics)
    set(SPACE_SURVIVOR_SFML_LIBS
        SFML::Graphics SFML::Window SFML::System SFML::Audio)
    set(SPACE_SURVIVOR_SFML_RUNTIME_TARGETS
        SFML::Graphics SFML::Window SFML::System SFML::Audio)
else()
    message(FATAL_ERROR "SFML 2.6.1 was found, but its expected CMake targets are missing.")
endif()

# AUDIT-018: Keep warnings fatal for both the game and every test target.
add_library(space_survivor_options INTERFACE)
target_compile_features(space_survivor_options INTERFACE cxx_std_17)
if(MSVC)
    target_compile_options(space_survivor_options INTERFACE /W4 /WX /permissive-)
else()
    target_compile_options(space_survivor_options INTERFACE
        -Wall -Wextra -Wpedantic -Werror)
endif()

set(SPACE_SURVIVOR_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Projectile.cpp
    src/Asteroid.cpp
    src/PowerUp.cpp
    src/Boss.cpp
    src/ParticleSystem.cpp
    src/Collision.cpp
    src/HUD.cpp
    src/Menu.cpp
    src/SaveSystem.cpp
    src/AudioManager.cpp
)

add_executable(SPACE_SURVIVOR ${SPACE_SURVIVOR_SOURCES})
target_include_directories(SPACE_SURVIVOR PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(SPACE_SURVIVOR PRIVATE
    space_survivor_options ${SPACE_SURVIVOR_SFML_LIBS})

add_custom_command(TARGET SPACE_SURVIVOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:SPACE_SURVIVOR>/assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:SPACE_SURVIVOR>/saves"
    VERBATIM
)

# AUDIT-026: Support both SFML target naming schemes when copying Windows runtime files.
foreach(SFML_RUNTIME_TARGET IN LISTS SPACE_SURVIVOR_SFML_RUNTIME_TARGETS)
    if(TARGET ${SFML_RUNTIME_TARGET})
        add_custom_command(TARGET SPACE_SURVIVOR POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${SFML_RUNTIME_TARGET}>"
                    "$<TARGET_FILE_DIR:SPACE_SURVIVOR>"
            VERBATIM
        )
    endif()
endforeach()

set_property(TARGET SPACE_SURVIVOR PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# AUDIT-020: Register each subsystem test with CTest.
if(SPACE_SURVIVOR_BUILD_TESTS)
    enable_testing()

    function(add_space_survivor_test test_name)
        add_executable(${test_name} ${ARGN})
        target_include_directories(${test_name} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_link_libraries(${test_name} PRIVATE
            space_survivor_options ${SPACE_SURVIVOR_SFML_LIBS})
        add_test(NAME ${test_name} COMMAND ${test_name})
    endfunction()

    add_space_survivor_test(test_collision
        tests/test_collision.cpp src/Collision.cpp src/Asteroid.cpp)
    add_space_survivor_test(test_math tests/test_math.cpp)
    add_space_survivor_test(test_player
        tests/test_player.cpp src/Player.cpp src/Projectile.cpp)
    add_space_survivor_test(test_enemy
        tests/test_enemy.cpp src/Enemy.cpp)
    add_space_survivor_test(test_boss
        tests/test_boss.cpp src/Boss.cpp src/Projectile.cpp)
    add_space_survivor_test(test_powerup
        tests/test_powerup.cpp src/PowerUp.cpp)
    add_space_survivor_test(test_particles
        tests/test_particles.cpp src/ParticleSystem.cpp)
    add_space_survivor_test(test_save_system
        tests/test_save_system.cpp src/SaveSystem.cpp)
endif()

install(TARGETS SPACE_SURVIVOR RUNTIME DESTINATION .)
install(DIRECTORY assets/ DESTINATION assets)
