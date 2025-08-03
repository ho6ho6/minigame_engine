cmake_minimum_required(VERSION 3.10)
project(minigame_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# src 以下の .cpp を全自動収集
file(GLOB_RECURSE ENGINE_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# 収集したソースでターゲットを一度だけ定義
add_executable(minigame_engine ${ENGINE_SOURCES})
add_definitions(-DUNICODE -D_UNICODE)

target_include_directories(minigame_engine PUBLIC
  ${PROJECT_SOURCE_DIR}            # ルートを追加
  ${PROJECT_SOURCE_DIR}/include    # これも残す
)

target_link_libraries(minigame_engine PRIVATE
    user32
    gdi32
    gdiplus
)

add_custom_target(run
    COMMAND $<TARGET_FILE:minigame_engine>
    DEPENDS minigame_engine
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(minigame_engine PROPERTIES
    WIN32_EXECUTABLE YES
)