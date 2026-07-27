cmake_minimum_required(VERSION 3.16)
project(StarVoidAlienSignal
    VERSION 1.0.0
    DESCRIPTION "Star Void: Alien Signal - A 2D space shooter"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find raylib - try multiple approaches
find_package(PkgConfig REQUIRED)
pkg_check_modules(raylib REQUIRED raylib IMPORTED_TARGET)

if(NOT raylib_FOUND)
    # Fallback: try direct path
    find_library(raylib_LIBRARY raylib
        PATHS /opt/homebrew/lib /usr/local/lib
        NO_DEFAULT_PATH
    )
    find_path(raylib_INCLUDE_DIR raylib.h
        PATHS /opt/homebrew/include /usr/local/include
        NO_DEFAULT_PATH
    )
    if(raylib_LIBRARY AND raylib_INCLUDE_DIR)
        set(raylib_FOUND TRUE)
        set(raylib_LIBRARIES ${raylib_LIBRARY})
        set(raylib_INCLUDE_DIRS ${raylib_INCLUDE_DIR})
    endif()
endif()

# Source files
set(SOURCES
    src/main.c
    src/game.c
    src/player.c
    src/enemy.c
    src/bullet.c
    src/collision.c
    src/weapon.c
    src/particles.c
    src/level.c
    src/audio.c
    src/ui.c
    src/powerup.c
    src/story.c
)

# Include directories
set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${raylib_INCLUDE_DIRS}
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})

if(raylib_FOUND)
    if(TARGET PkgConfig::raylib)
        target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::raylib)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${raylib_LIBRARIES})
    endif()
else()
    # Last resort: link directly
    target_link_libraries(${PROJECT_NAME} PRIVATE
        /opt/homebrew/lib/libraylib.dylib
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        /opt/homebrew/include
    )
endif()

# Platform-specific settings
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.starvoid.aliensignal"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

# Android support (requires Android NDK)
if(ANDROID)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-landroid -lEGL -lGLESv2 -lOpenSLES"
    )
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)