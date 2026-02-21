cmake_minimum_required(VERSION 3.20)
project(game_gdextension VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# godot-cpp 빌드
set(GODOT_CPP_SYSTEM_HEADERS ON CACHE BOOL "" FORCE)
add_subdirectory(godot-cpp)

# MinGW 크로스 컴파일 픽스:
# godot-cpp의 windows_generate()가 PUBLIC으로 --no-undefined를 추가한다.
# GDExtension DLL은 Godot 런타임이 gdextension_interface_* 심볼을 제공하므로
# 링크 타임 undefined가 정상이다. --no-undefined 없이 static 링크만 유지한다.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT MSVC)
    set_property(TARGET godot-cpp PROPERTY INTERFACE_LINK_OPTIONS
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# C++ Core (Phase 1~2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core ${CMAKE_BINARY_DIR}/core)

# GDExtension 공유 라이브러리 (Phase 3)
add_library(game_gdextension SHARED
    src/register_types.cpp
    src/gd_card_database.cpp
    src/gd_player_combatant.cpp
    src/gd_enemy_combatant.cpp
    src/gd_combat_manager.cpp
)

target_include_directories(game_gdextension PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(game_gdextension
    godot-cpp
    game_core
)

set_target_properties(game_gdextension PROPERTIES
    PREFIX ""
    OUTPUT_NAME "game_gdextension"
)


# 빌드 결과물을 Godot bin/ 디렉터리로 복사
set(GODOT_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
add_custom_command(TARGET game_gdextension POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GODOT_PROJECT_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:game_gdextension>
        "${GODOT_PROJECT_DIR}/bin/$<TARGET_FILE_NAME:game_gdextension>"
    COMMENT "Copying GDExtension to bin/"
)
