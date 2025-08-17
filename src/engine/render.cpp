/****************************************
 *���t���[���̕`�揈�������s��             *
 ****************************************/

#include "../include/render.hpp"
#include <gdiplus.h>
#include <string>
#include <d3d11.h>
#include <dxgi.h>
#include <stdexcept>


using namespace Gdiplus;

// DirectX 11 �̃f�o�C�X�ƃR���e�L�X�g
static ID3D11Device*                    g_pd3dDevice = nullptr;
static ID3D11DeviceContext*            g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*                 g_pSwapChain = nullptr;

// �V�[���`��p�e�N�X�`�� + RTV + SRV
static ID3D11Texture2D*                 g_pSceneTex = nullptr;
static ID3D11RenderTargetView*          g_pSceneRTV = nullptr;
static ID3D11ShaderResourceView*        g_pSceneSRV = nullptr;


// �E�B���h�E�n���h�����L���b�V�����Ă���
static HWND      g_hWnd = nullptr;
static ULONG_PTR g_gdiplusToken = 0;

static int viewport_width = 0;
static int viewport_height = 0;


// (1) �e�X�g�`��p�֐���ǉ�
static void DrawTest(Graphics& g)
{
    // �w�i�� Render() ���ŃN���A�ς�
    //Pen pen(Color(255, 0, 0, 0), 3.0f);
    //g.DrawRectangle(&pen, 50, 50, 200, 100);

    //SolidBrush brush(Color(255, 255, 0, 0));
    //g.FillEllipse(&brush, 300, 50, 100, 100);

    FontFamily family(L"���˂���Ղ����");
    Font font(&family, 24, FontStyleBold, UnitPixel);
    PointF pt(50.0f, 200.0f);
    SolidBrush textBrush(Color(255, 0, 0, 255));
    g.DrawString(L"�~�j�Q�[���G���W�����쐬���Ă���", -1, &font, pt, &textBrush);
}


namespace render
{

    bool Render_Start(HWND hwnd, int width, int height)
    {

        // ��: �X���b�v�`�F�[���^�f�o�C�X����
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
            /* out */ nullptr,
            &g_pd3dDevice,
            &featureLevel,
            &g_pd3dDeviceContext
        );
        if (FAILED(hr)) return false;



        g_hWnd = hwnd;
        GdiplusStartupInput gdiplusInput;
        if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Ok)
            return false;
        return true;
    }

    void Render_Shutdown()
    {

        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->ClearState(); }
        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); }
        g_pd3dDevice = nullptr;
        g_pd3dDeviceContext = nullptr;

        GdiplusShutdown(g_gdiplusToken);
    }


    //--- �ǉ��FRender ������ ---
    void Render_Update(float deltaTime, uint64_t frameCount)
    {
        // 1) �N���C�A���g�̈�T�C�Y�擾
        RECT rc;
        GetClientRect(g_hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // 2) ��� DC / ������ DC / �o�b�N�o�b�t�@����
        HDC hdc = GetDC(g_hWnd);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

        // 3) GDI+ �`��F������ DC �ɑ΂��ăN���A���e�L�X�g�`��
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

		DrawTest(Graphics(memDC)); // �e�X�g�`��֐����Ăяo��

        // 4) ��ʂɈꊇ�]��
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // 5) ��Еt��
        SelectObject(memDC, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(memDC);
        ReleaseDC(g_hWnd, hdc);
    }

    void Render_Resizeview(int width, int height)
    {
        viewport_width = width;
        viewport_height = height;
    }

    int Render_PickObject(int x, int y)
    {
        return -1;
    }

    // �A�N�Z�T����
    ID3D11Device* Render_GetDevice()
    {
        return g_pd3dDevice;
    }

    ID3D11DeviceContext* Render_GetDeviceContext()
    {
        return g_pd3dDeviceContext;
    }

    ID3D11ShaderResourceView* Render_GetSceneSRV()
    {
        return g_pSceneSRV;
    }

}


// �����w���p�FSceneView �p�e�N�X�`�����쐬
static void Render_GetSceneSRV(int width, int height)
{
    // �������\�[�X�j��
    if (g_pSceneSRV) { g_pSceneSRV->Release();     g_pSceneSRV = nullptr; }
    if (g_pSceneRTV) { g_pSceneRTV->Release();     g_pSceneRTV = nullptr; }
    if (g_pSceneTex) { g_pSceneTex->Release();     g_pSceneTex = nullptr; }

    // �e�N�X�`���쐬
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, nullptr, &g_pSceneTex);
    if (FAILED(hr)) throw std::runtime_error("Failed to create scene texture");

    // RTV
    hr = g_pd3dDevice->CreateRenderTargetView(g_pSceneTex, nullptr, &g_pSceneRTV);
    if (FAILED(hr)) throw std::runtime_error("Failed to create scene RTV");

    // SRV
    hr = g_pd3dDevice->CreateShaderResourceView(g_pSceneTex, nullptr, &g_pSceneSRV);
    if (FAILED(hr)) throw std::runtime_error("Failed to create scene SRV");
}