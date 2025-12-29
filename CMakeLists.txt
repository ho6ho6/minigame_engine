cmake_minimum_required(VERSION 3.10)
project(minigame_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 実行ファイルの出力先
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

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
    #"${PROJECT_SOURCE_DIR}/src/engine/assets/*.cpp"
    #"${PROJECT_SOURCE_DIR}/src/engine/window_editor/*.cpp"
)

# シェーダーファイルも収集
file(GLOB_RECURSE SHADER_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.hlsl"
)



# Assets/フォルダを出力先へコピー　(これはテクスチャフォルダを見つけるため)
#add_custom_target(
#    CopyAssets ALL
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${PROJECT_SOURCE_DIR}/Assets
#    ${CMAKE_RUNTIME_OUTPUT_PATH}/Assets
#    COMMENT "Copying Assets folder to output directory"
#)


# 重複排除
list(REMOVE_DUPLICATES ENGINE_SOURCES)


# 収集したソースでターゲットを一度だけ定義
add_executable(minigame_engine
    WIN32
    src/main.cpp
    src/external/stb_image_impl.cpp
    ${ENGINE_SOURCES}
    ${IMGUI_SOURCES} "src/engine/component/component_api.cpp" "src/engine/component/component_config.cpp" "src/engine/component/component_keycapture.cpp" "src/engine/assets/scene.cpp")


# debug用の定義
foreach(src ${ENGINE_SOURCES})
    message(STATUS "Source File: ${src}")
    endforeach()

# windows固有
add_definitions(-DUNICODE -D_UNICODE)

target_include_directories(minigame_engine PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/external
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

# minigame_engine をビルドする前に CopyAssets を実行
#add_dependencies(minigame_engine CopyAssets)