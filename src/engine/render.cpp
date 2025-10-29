/****************************************
 *      ���t���[���̕`�揈�������s��        *
 ****************************************/

#include "include/render.hpp"

#include <gdiplus.h>
#include <string>
#include <d3d11.h>
#include <dxgi.h>
#include <stdexcept>

#include <imgui.h>
#include <imgui_internal.h> // �ǉ�: ImGuiWindow�^�̒�`
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>


using namespace Gdiplus;

// DirectX 11 �̃f�o�C�X�ƃR���e�L�X�g
static ID3D11Device*                    g_device = nullptr;
static ID3D11DeviceContext*            g_context = nullptr;
static IDXGISwapChain*                 g_swapChain = nullptr;

// �V�[���`��p�e�N�X�`�� + RTV + SRV
static ID3D11RenderTargetView*          g_rtv = nullptr;    
static ID3D11DepthStencilView*          g_dsv = nullptr;
static ID3D11ShaderResourceView*        g_srv = nullptr;




// �E�B���h�E�n���h�����L���b�V�����Ă���
static HWND      g_hWnd = nullptr;
static ULONG_PTR g_gdiplusToken = 0;

static int viewport_width = 0;
static int viewport_height = 0;

namespace n_render
{

    bool Render_Start(HWND hwnd, int width, int height)
    {
        // ��: �X���b�v�`�F�[���^�f�o�C�X����
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
		sd.BufferDesc.Width = width;     //�}�E�X�̍��W������Ȃ��̂͂�����ς��Ă��Ȃ�����
		sd.BufferDesc.Height = height;    //�{����1920x1280�ɂ��������A���̐��l���Ƒ����̃Y������������
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDeviceAndSwapChain( // Microsoft �̃h�L�������g�ɂ��΁A���̊֐��́u�����_�����O�Ɏg�p�����X���b�v�`�F�[���ƕ\���A�_�v�^��\���f�o�C�X�v
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
        vpInit.Width = static_cast<float>(width);   // ���� width
        vpInit.Height = static_cast<float>(height);  // ���� height
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

        Render_CreateSceneTexture(width, height);

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
        // RenderTargetView �̃N���A
        g_context->ClearRenderTargetView(g_rtv, clearColor);


        // Viewport �ݒ�Ȃ�
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

    }

    void Render_EndFrame()
    {
        // �i�K�v�ɉ����ăR�}���h�o�b�t�@�̃t�F���X�ݒ�Ȃǁj
    }

    void Render_Present()
    {
        g_swapChain->Present(1, 0);
    }

    void Render_Resizeviewport(int win_w, int win_h, int fb_w, int fb_h)
    {
        if (win_w <= 0 || win_h <= 0)
            return;

        // �R���e�L�X�g��������Ώ����������O
        if (ImGui::GetCurrentContext() == nullptr) {
            // ���O�o��: CreateContext ���Ă΂�Ă��Ȃ�/�R���e�L�X�g����������
            printf("Render_Resizeviewport: ImGui context is null. Skipping.\n");
            return;
        }

        ImGuiIO& io = ImGui::GetIO(); // CreateContext() �����ɌĂ΂�Ă��邱�Ƃ�O��Ƃ���

        io.DisplaySize = ImVec2((float)win_w, (float)win_h);

        float sx = 1.0f, sy = 1.0f;
        if (win_w > 0) sx = (float)fb_w / (float)win_w;
        if (win_h > 0) sy = (float)fb_h / (float)win_h;
        if (!isfinite(sx) || !isfinite(sy) || sx <= 0.0f || sy <= 0.0f) { sx = sy = 1.0f; }

        io.DisplayFramebufferScale = ImVec2(sx, sy);
    }

    int Render_PickObject(int x, int y)
    {
        return -1;
    }
    
    bool Render_CreateSceneTexture(int width, int height)
    {
        if (g_rtv) { g_rtv->Release(); g_rtv = nullptr; }
        if (g_srv) { g_srv->Release(); g_srv = nullptr; }

        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        ID3D11Texture2D* pTexture = nullptr;
        HRESULT hr = g_device->CreateTexture2D(&texDesc, nullptr, &pTexture);
        if (FAILED(hr)) return false;

        // RTV�쐬
        hr = g_device->CreateRenderTargetView(pTexture, nullptr, &g_rtv);
        if (FAILED(hr)) { pTexture->Release(); return false; }

        // SRV�쐬
        hr = g_device->CreateShaderResourceView(pTexture, nullptr, &g_srv);
        if (FAILED(hr)) { pTexture->Release(); return false; }

        pTexture->Release();
        return true;
	}



    // �A�N�Z�T����
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
        return g_srv;
    }

}