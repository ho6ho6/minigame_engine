/**************************************************************************
 * WinMain とメインループの呼び出し、各サブシステムの初期化と終了処理を行う        *
 **************************************************************************/

#include "include/window.hpp"    // InitWindow(), IsRunning(), PollEvents(), ShutdownWindow()
#include "include/input.hpp"     // InitInput(), ShutdownInput(), ProcessInput()
#include "include/game.hpp"      // Game_Start(), Game_Update(), Game_Render(), Game_Shutdown()
#include "include/time.hpp"      // Time::Start(), Time::Update(), Time::GetDeltaTime()
#include "include/render.hpp"    // InitRenderer(), BeginFrame(), RenderFrame(), EndFrame(), ShutdownRenderer()

#include "include/window_editor/window_manager.hpp" // window_manager

/*ImGui は「既存の OS ウィンドウ」のクライアント領域内に GUI を即時モードで描画するライブラリ*/
// Begin/End で開くウィンドウ
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <Windows.h>
#include <iostream>

/*
  WinMain の引数:
    HINSTANCE   hInstance     : このアプリケーションのインスタンスハンドル
    HINSTANCE   hPrevInstance : 常に NULL
    LPSTR       lpCmdLine     : コマンドライン引数（ANSI）
    int         nCmdShow      : ウィンドウの初期表示方法
*/

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{

    /*----------------------------------初期化----------------------------------*/

    const int width = 1920;
    const int height = 1080;

    if (!window::InitWindow(hInstance, nCmdShow, width, height, L"minigame_engine")) return -1;

	if (!game::Game_Start()) return -1;

    if (!n_render::Render_Start(window::GetHWND(), width, height)) return -1;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark(); // ImGui のテーマをダークに設定

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Docking機能をON
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // （任意）別ウィンドウへの展開も可能に

    
    ImGui_ImplWin32_Init(window::GetHWND());
    ImGui_ImplDX11_Init(n_render::Render_GetDevice(), n_render::Render_GetDeviceContext());

    frame::Time::Start_Time();

    //input::Input_Start();

    //game::Game_Start();




    /*----------------------------------初期化----------------------------------*/


    /*-----------------------------ウィンドウの登録------------------------------*/

    n_windowmanager::window_manager wm;
    wm.Register_SceneWindow();           // ImGui,DX11の初期化
    wm.Register_GameWindow();
    //wm.Register_InputWindow();    //別ウィンドウを生成して操作する可能性もある。
    wm.Register_Hierarchywindow();
    wm.Register_Assetswindow();


    /*-----------------------------ウィンドウの登録------------------------------*/



    /*----------------------------------メイン----------------------------------*/

    while (window::IsRunning()) //window.cpp
    {
        window::PollEvents();

        // 2) 各サブシステム更新
        float deltaTime = frame::Time::Update_Time();
        uint64_t frameTime = frame::Time::GetFrameCount();
        //input::Input_Update();
        //game::Game_Update(deltaTime);


        // 3) ImGui フレーム開始
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static const float clear_col[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
        n_render::Render_Frame(clear_col, deltaTime, frameTime);

        // ── ここから DockSpace ──
        // ホスト用フルスクリーンウィンドウを立てて
        ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("##MainDockHost", nullptr, hostFlags);

        // DockSpace 本体
        ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(
            dockspaceID, ImVec2(0, 0),
            ImGuiDockNodeFlags_PassthruCentralNode
        );
        ImGui::End();

        wm.RenderAll();

        // ImGui 描画確定
        ImGui::Render();
        
        auto ctx = n_render::Render_GetDeviceContext();
        auto rtv = n_render::Render_GetRenderTargetView();

        ctx->OMSetRenderTargets(1, &rtv, nullptr);
        ctx->ClearRenderTargetView(rtv, clear_col);

        //game::Game_Render(); // game.cpp
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


		// デフォルトで DockSpace を作成する場合は以下のようにコメントアウトを外す
        //static bool firstTime = true;
        //if (firstTime)
        //{
        //    firstTime = false;
        //    ImGuiID dockMain = dockspaceID;
        //    ImGui::DockBuilderRemoveNode(dockMain);
        //    ImGui::DockBuilderAddNode(dockMain, ImGuiDockNodeFlags_DockSpace);
        //    ImGui::DockBuilderSetNodeSize(dockMain, ImGui::GetIO().DisplaySize);

        //    // 左にヒエラルキー、右にインスペクタ用に縦分割
        //    ImGuiID leftID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
        //    ImGuiID rightID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.2f, nullptr, &dockMain);

        //    // 下部にコンソール用の横分割
        //    ImGuiID bottomID = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain);

        //    // パネルをそれぞれ配置
        //    ImGui::DockBuilderDockWindow("Hierarchy", leftID);
        //    ImGui::DockBuilderDockWindow("Inspector", rightID);
        //    ImGui::DockBuilderDockWindow("Console", bottomID);
        //    ImGui::DockBuilderDockWindow("Scene", dockMain);

        //    ImGui::DockBuilderFinish(dockMain);
        //}

        // ── DockSpace 設置ここまで ──

        // 4) 各パネルを描画する
        //    ※Begin()/End() のタイトルが "Hierarchy" など一致していること
        //DrawHierarchy();   // 例: void DrawHierarchy() { ImGui::Begin("Hierarchy"); … ImGui::End(); }
        //DrawInspector();   // 例:
        //wm.RenderAll();    // Scene View (window_manager経由)
        //DrawConsole();     // 例:



        // 6) 最後に DirectX の Present
        n_render::Render_Present();
    }

    /*----------------------------------メイン----------------------------------*/


    // 5. 終了処理
	ImGui_ImplDX11_Shutdown();        // ImGuiのDirectX11レンダリングを終了
	ImGui_ImplWin32_Shutdown();       // ImGuiのWin32プラットフォームを終了
	ImGui::DestroyContext();            // ImGuiコンテキストを破棄

    input::Input_Shutdown();             // input.cpp
    n_render::Render_Shutdown();        // render.cpp
	game::Game_Shutdown();            // game.cpp
	window::ShutdownWindow();        // window.cpp

    return 0;
}

//void DrawHierarchy()
//{
//    ImGui::Begin("Hierarchy");
//    // ツリー表示など
//    ImGui::Text("GameObject A");
//    ImGui::Text("GameObject B");
//    ImGui::End();
//}

//void DrawConsole()
//{
//    ImGui::Begin("Console");
//    // ログ出力
//    for (auto& msg : logBuffer)
//        ImGui::TextUnformatted(msg.c_str());
//    ImGui::End();
//}