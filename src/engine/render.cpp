/****************************************
 *      毎フレームの描画処理だけ行う        *
 ****************************************/

#include "../include/render.hpp"
#include "../include/game.hpp"

#include <gdiplus.h>
#include <string>
#include <d3d11.h>
#include <dxgi.h>
#include <stdexcept>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>


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

namespace n_render
{

    bool Render_Start(HWND hwnd, int width, int height)
    {
        // 例: スワップチェーン／デバイス生成
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = 1000;
        sd.BufferDesc.Height = 1000;
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

        ID3D11Texture2D* pBackBuffer = nullptr;
        g_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        g_device->CreateRenderTargetView(pBackBuffer, nullptr, &g_rtv);

        D3D11_VIEWPORT vpInit{};
        vpInit.TopLeftX = 0.0f;
        vpInit.TopLeftY = 0.0f;
        vpInit.Width = static_cast<float>(width);   // 引数 width
        vpInit.Height = static_cast<float>(height);  // 引数 height
        vpInit.MinDepth = 0.0f;
        vpInit.MaxDepth = 1.0f;
        g_context->RSSetViewports(1, &vpInit);

        pBackBuffer->Release();

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



    void Render_Frame(const float clearColor[4], float dt, uint64_t frameTime)
    {


        g_context->OMSetRenderTargets(1, &g_rtv, nullptr);
        // 1) RenderTargetView のクリア
        g_context->ClearRenderTargetView(g_rtv, clearColor);

        // 2) DepthStencilView のクリア
        // g_context->ClearDepthStencilView(g_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



        ImGui::Begin("Stats");
		ImGui::Text("Frame: %llu", frameTime * 1000.0f);
		ImGui::Text("Delta Time: %.3f ms", dt * 1000.0f);
		ImGui::End();


        // Viewport 設定など
        RECT rc{};
        GetClientRect(g_hWnd, &rc);
        D3D11_VIEWPORT vp{};
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = static_cast<float>(rc.right - rc.left);
        vp.Height = static_cast<float>(rc.bottom - rc.top);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        g_context->RSSetViewports(1, &vp);
		g_swapChain->Present(1, 0); // VSync あり

    }

    void Render_EndFrame()
    {
        // （必要に応じてコマンドバッファのフェンス設定など）
    }

    void Render_Present()
    {
        g_swapChain->Present(1, 0);
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

    ID3D11RenderTargetView* Render_GetRenderTargetView()
    {
        return g_rtv;
    }

    ID3D11ShaderResourceView* Render_GetSceneSRV()
    {
        return nullptr;
    }

}