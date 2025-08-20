cmake_minimum_required(VERSION 3.10)
project(minigame_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ImGui 実装ソース一覧
set(IMGUI_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_win32.cpp
    ${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_dx11.cpp
)

# src 以下の .cpp を全自動収集
file(GLOB_RECURSE ENGINE_SOURCES
    "${PROJECT_SOURCE_DIR}/src/engine/*.cpp"
)


# 収集したソースでターゲットを一度だけ定義
add_executable(minigame_engine
    WIN32
    src/main.cpp
    ${ENGINE_SOURCES}
    ${IMGUI_SOURCES}        
)

# windows固有
add_definitions(-DUNICODE -D_UNICODE)
target_include_directories(minigame_engine PUBLIC
  ${PROJECT_SOURCE_DIR}            
  ${PROJECT_SOURCE_DIR}/include    
  ${PROJECT_SOURCE_DIR}/include/window_editor
  ${PROJECT_SOURCE_DIR}/third_party/imgui
  ${PROJECT_SOURCE_DIR}/third_party/imgui/backends
)

target_link_libraries(minigame_engine PRIVATE
    user32
    gdi32
    gdiplus
    d3d11
    dxgi
)

add_custom_target(run
    COMMAND $<TARGET_FILE:minigame_engine>
    DEPENDS minigame_engine
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
