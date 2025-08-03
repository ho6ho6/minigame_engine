/****************************************
 *���t���[���̕`�揈�������s��             *
 ****************************************/

#include "engine/render.hpp"
#include <Windows.h>
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;

// �E�B���h�E�n���h�����L���b�V�����Ă���
static HWND      g_hWnd = nullptr;
static ULONG_PTR g_gdiplusToken = 0;


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
        g_hWnd = hwnd;
        GdiplusStartupInput gdiplusInput;
        if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Ok)
            return false;
        return true;
    }

    void Render_Shutdown()
    {
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

}