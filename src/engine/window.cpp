#include "engine/window.hpp"

/*�E�B���h�E����*/

static HWND g_hWnd = nullptr;
static bool g_isRunning = true;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY || msg == WM_CLOSE)
    {
        g_isRunning = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace engine
{

    bool InitWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, const wchar_t* title)
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WndProc;                           // �E�B���h�E�v���V�[�W���ւ̃|�C���^
        wc.hInstance = hInstance;                           // �A�v���P�[�V�����C���X�^���X hlnstance�p�����[�^����wWinMain()�ɓn�����
        wc.lpszClassName = L"MiniGameEngineWindowClass";    // �E�B���h�E�N���X��
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        if (!RegisterClass(&wc)) return false;

        g_hWnd = CreateWindowExW(
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
        if (g_hWnd) DestroyWindow(g_hWnd);
        UnregisterClass(L"MiniGameEngineClass", GetModuleHandle(nullptr));
    }
}