cmake_minimum_required(VERSION 3.20)
project(game_core VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Phase 1~2에서 파일을 하나씩 추가한다
set(CORE_SOURCES
    src/status_effect.cpp
    src/deck.cpp
    src/combatant.cpp
    src/player_combatant.cpp
    src/enemy_combatant.cpp
    src/effect_handler.cpp
    src/combat_manager.cpp
)

# 소스가 없어도 빌드 가능한 INTERFACE 타겟 (Phase 0)
# 파일이 추가되면 STATIC으로 변경한다
add_library(game_core STATIC ${CORE_SOURCES})

target_include_directories(game_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
