/*******************************************************************************
 *engine::InitWindow, PollEvents, IsRunning, ShutdownWindow, SwapBuffers の定義*
 *******************************************************************************/

#include "../include/window.hpp"
#include "../include/window_editor/window_scene.hpp"
#include "../include/window_editor/window_editor.hpp"
#include <Windows.h>


/*ウィンドウ生成*/

//static HWND g_hWnd = nullptr;
//static HINSTANCE g_hInstance = nullptr;
//static bool g_isRunning = false;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace window
{

    bool InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"MiniGameEngineWindowClass";
        RegisterClassEx(&wc);


        g_hWnd = CreateWindowEx(
            0,
            wc.lpszClassName,
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height,
            nullptr, nullptr,
            hInstance,
            nullptr
        );

        if (!g_hWnd) return false;

        ShowWindow(g_hWnd, nCmdShow);
        UpdateWindow(g_hWnd);

		g_isRunning = true;
        return true;
    }


    bool IsRunning()
    {
        return g_isRunning;
    }

    void PollEvents()
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    HWND GetHWND()
    {
        return g_hWnd;
    }

    void ShutdownWindow()
    {
        if (g_hWnd)
        {
            DestroyWindow(g_hWnd);
            g_hWnd = nullptr;
        }
        UnregisterClass(L"MiniGameEngineWindowClass", g_hInstance);
        g_isRunning = false;
    }
}
