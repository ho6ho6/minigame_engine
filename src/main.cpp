/**************************************************************************
 * WinMain とメインループの呼び出し、各サブシステムの初期化と終了処理を行う        *
 **************************************************************************/

#include "include/window.hpp"
#include "include/input.hpp"
#include "include/game.hpp"
#include "include/time.hpp"
#include "include/render.hpp"

#include "include/window_editor/window_manager.hpp" // window_manager
#include "include/assets/assets_manager/texture_manager.hpp" // texture_manager
#include "include/window_editor/window_assets.hpp"
#include "include/assets/assets_manager/assets_manager.hpp"
#include "include/assets/util.hpp"

 /*ImGui は「既存の OS ウィンドウ」のクライアント領域内に GUI を即時モードで描画するライブラリ*/
 // Begin/End で開くウィンドウ
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <Windows.h>
#include <iostream>
#include <filesystem>

#ifdef PIX_CAPTURE_WAIT
Sleep(1000);
#endif

/*
  WinMain の引数:
    HINSTANCE   hInstance     : このアプリケーションのインスタンスハンドル
    HINSTANCE   hPrevInstance : 常に NULL
    LPSTR       lpCmdLine     : コマンドライン引数（ANSI）
    int         nCmdShow      : ウィンドウの初期表示方法
*/

n_assetsmanager::assets_manager g_AssetsManager(n_texturemanager::instance_texmag()); // グローバルなアセットマネージャ


// exeファイルの場所から候補を探して返す
static std::filesystem::path ResolveTexturesDir()
{
    namespace fs = std::filesystem;

    // exeファイルの場所を取得
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
    {
        std::cerr << "[TextureManager] GetModuleFileNameA failed\n";
        return {};
    }
    fs::path exeDir = fs::path(exePath).remove_filename();

    // 候補リスト
    std::vector<fs::path> candidates = {
        exeDir / "Assets" / "textures", // exe直下
        exeDir.parent_path() / "Assets" / "textures", // 1階層上
        fs::current_path() / "Assets" / "textures" // カレントディレクトリ
    };

    for (auto& c : candidates)
    {
        if (fs::exists(c) && fs::is_directory(c))   return c;
    }

    // 見つからなかった
    std::cerr << "[TextureManager] Texture directory not found in candidates\n";
    for (auto& c : candidates)
    {
        std::cerr << "  Candidate: " << c << "\n";
    }
    return {};
}


int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    /*----------------------------------初期化----------------------------------*/

    const int width = 1920;
    const int height = 1000;

	//Sleep(10000); // デバッガ接続用待機 (10秒)

    std::cout << "ゲームエンジンmainがここから始まります。" << "\n" << std::filesystem::current_path() << std::endl;

    if (!n_window::InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine")) return -1;

    //if (!n_game::Game_Start()) return -1;

    if (!n_render::Render_Start(n_window::GetHWND(), width, height)) return -1;

    // ImGui 初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark(); // ImGui のテーマをダークに設定

    // Dock機能は使わない
    io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(n_window::GetHWND());
    ImGui_ImplDX11_Init(n_render::Render_GetDevice(), n_render::Render_GetDeviceContext());

    n_time::Time::Start_Time();

    //input::Input_Start();

    //game::Game_Start();

	// テクスチャマネージャ初期化
	n_texturemanager::instance_texmag().Initialize(ResolveTexturesDir());


    /*----------------------------------初期化----------------------------------*/


    /*-----------------------------ウィンドウの登録------------------------------*/

    n_windowmanager::window_manager wm;
    wm.Register_SceneWindow();           // ImGui,DX11の初期化
    wm.Register_GameWindow();
    //wm.Register_InputWindow();    //別ウィンドウを生成して操作する可能性もある。
    wm.Register_Hierarchywindow();
    wm.Register_Assetswindow();


    /*-----------------------------ウィンドウの登録------------------------------*/


    /*------------------------------Assetsの登録-------------------------------*/

	// テクスチャを読み込む (同期ロード)
    n_texturemanager::instance_texmag().LoadAllTextures();

    /*------------------------------Assetsの登録-------------------------------*/

    /*----------------------------------メイン----------------------------------*/

    while (n_window::IsRunning()) //window.cpp
    {
        n_window::PollEvents();

        // マウス座標を取得して ImGui に渡す
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(n_window::GetHWND(), &p);

        // ImGui にマウス位置を設定
        ImVec2 fbScale = io.DisplayFramebufferScale;
        io.MousePos = ImVec2((float)p.x / fbScale.x, (float)p.y / fbScale.y);

        // 各サブシステム更新
        float deltaTime = n_time::Time::Update_Time();
        uint64_t frameTime = n_time::Time::GetFrameCount();
        //input::Input_Update();
        //game::Game_Update(deltaTime);


        // ImGui フレーム開始
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //Assets テクスチャの描画
        g_AssetsManager.assets_Show();

        static const float clear_col[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
        n_render::Render_Frame(clear_col, deltaTime, frameTime);

        //ImGui::End();

        wm.RenderAll();

        // ImGui 描画確定
        ImGui::Render();

        auto ctx = n_render::Render_GetDeviceContext();
        auto g_rtv = n_render::Render_GetRenderTargetView();

        ctx->OMSetRenderTargets(1, &g_rtv, nullptr);
        ctx->ClearRenderTargetView(g_rtv, clear_col);

        //game::Game_Render(); // game.cpp

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // 最後に DirectX の Present
        n_render::Render_Present();
    }

    /*----------------------------------メイン----------------------------------*/


    // 5. 終了処理
    ImGui_ImplDX11_Shutdown();        // ImGuiのDirectX11レンダリングを終了
    ImGui_ImplWin32_Shutdown();       // ImGuiのWin32プラットフォームを終了
    ImGui::DestroyContext();            // ImGuiコンテキストを破棄
	n_texturemanager::instance_texmag().Shutdown(); // テクスチャマネージャ終了
    n_input::Input_Shutdown();             // input.cpp
    n_render::Render_Shutdown();        // render.cpp
    //n_game::Game_Shutdown();            // game.cpp
    n_window::ShutdownWindow();        // window.cpp

    return 0;
}