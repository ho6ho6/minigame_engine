/****************************************
 *毎フレームの描画処理だけ行う             *
 ****************************************/

#include "../include/render.hpp"
#include <gdiplus.h>
#include <string>
#include <d3d11.h>
#include <dxgi.h>
#include <stdexcept>


using namespace Gdiplus;

// DirectX 11 のデバイスとコンテキスト
static ID3D11Device*                    g_device = nullptr;
static ID3D11DeviceContext*            g_context = nullptr;
static IDXGISwapChain*                 g_swapChain = nullptr;

// シーン描画用テクスチャ + RTV + SRV
static ID3D11RenderTargetView*          g_rtv = nullptr;
static ID3D11DepthStencilView*          g_dsv = nullptr;
static ID3D11ShaderResourceView*        g_srv = nullptr;


// ウィンドウハンドルをキャッシュしておく
static HWND      g_hWnd = nullptr;
static ULONG_PTR g_gdiplusToken = 0;

static int viewport_width = 0;
static int viewport_height = 0;


//// (1) テスト描画用関数を追加
//static void DrawTest(Graphics& g)
//{
//    // 背景は Render() 側でクリア済み
//    //Pen pen(Color(255, 0, 0, 0), 3.0f);
//    //g.DrawRectangle(&pen, 50, 50, 200, 100);
//
//    //SolidBrush brush(Color(255, 255, 0, 0));
//    //g.FillEllipse(&brush, 300, 50, 100, 100);
//
//    FontFamily family(L"しねきゃぷしょん");
//    Font font(&family, 24, FontStyleBold, UnitPixel);
//    PointF pt(50.0f, 200.0f);
//    SolidBrush textBrush(Color(255, 0, 0, 255));
//    g.DrawString(L"ミニゲームエンジンを作成している", -1, &font, pt, &textBrush);
//}


namespace render
{

    bool Render_Start(HWND hwnd, int width, int height)
    {
        // 例: スワップチェーン／デバイス生成
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            nullptr, 0,
            D3D11_SDK_VERSION,
            &sd,
            /* out */ 
            &g_swapChain,
            &g_device,
            &featureLevel,
            &g_context
        );

        if (FAILED(hr)) return false;

        g_hWnd = hwnd;
        GdiplusStartupInput gdiplusInput;
        if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Ok)
            return false;
        OutputDebugStringA("=== Render_Start: D3D11 initialized successfully ===\n");
        return true;
    }

    void Render_Shutdown()
    {

        if (g_rtv)  g_rtv->Release();
        if (g_dsv)  g_dsv->Release();
        if (g_swapChain) g_swapChain->Release();
        if (g_context)   g_context->Release();
        if (g_device)    g_device->Release();

    }

    void Render_BegineFrame(const float clearColor[4])
    {
        // 1) RenderTargetView のクリア
        g_context->ClearRenderTargetView(g_rtv, clearColor);
        // 2) DepthStencilView のクリア（必要なら）
        g_context->ClearDepthStencilView(g_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // Viewport 設定など
    }

    void render::Render_EndFrame()
    {
        // （必要に応じてコマンドバッファのフェンス設定など）
    }

    void render::Render_Present()
    {
        g_swapChain->Present(1, 0);
    }

    //--- 追加：Render を実装 ---
    void Render_Update(float deltaTime, uint64_t frameCount)
    {
        // 1) クライアント領域サイズ取得
        RECT rc;
        GetClientRect(g_hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // 2) 画面 DC / メモリ DC / バックバッファ生成
        HDC hdc = GetDC(g_hWnd);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

        // 3) GDI+ 描画：メモリ DC に対してクリア＆テキスト描画
        {
            Graphics g(memDC);
            g.Clear(Color::White);

            FontFamily ff(L"Consolas");
            Font       font(&ff, 16, FontStyleRegular, UnitPixel);
            SolidBrush brush(Color(255, 0, 0, 0));

            // Frame
            std::wstring txt1 = L"Frame: " + std::to_wstring(frameCount);
            g.DrawString(txt1.c_str(), -1, &font, PointF(10, 10), &brush);

            // Delta
            std::wstring txt2 = L"Delta: " + std::to_wstring(deltaTime).substr(0, 6);
            g.DrawString(txt2.c_str(), -1, &font, PointF(10, 30), &brush);
        }

		//DrawTest(Graphics(memDC)); // テスト描画関数を呼び出す

        // 4) 画面に一括転送
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // 5) 後片付け
        SelectObject(memDC, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(memDC);
        ReleaseDC(g_hWnd, hdc);
    }

    void Render_Resizeviewport(int width, int height)
    {
        viewport_width = width;
        viewport_height = height;
    }

    int Render_PickObject(int x, int y)
    {
        return -1;
    }

    // アクセサ実装
    ID3D11Device* Render_GetDevice()
    {
        if (g_device == nullptr)
            OutputDebugStringA(">>> Render_GetDevice(): g_device is NULL!\n");
        else
            OutputDebugStringA(">>> Render_GetDevice(): g_device is NOT NULL\n");
        return g_device;
    }

    ID3D11DeviceContext* Render_GetDeviceContext()
    {
        return g_context;
    }

    ID3D11ShaderResourceView* Render_GetSceneSRV()
    {
        return nullptr;
    }

}