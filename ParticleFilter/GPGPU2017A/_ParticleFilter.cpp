// ParticleFilter.cpp: define el punto de entrada de la aplicación de consola.
// Gustavo Alan Espindola Perez
// ITESO 2017
//

#include "stdafx.h"
#include "DxManager.h"
#include <math.h>
#include <random>



/* Implementation headers */
#include "PF_types.h"
#include "PF_cfg.h"

             /* user double or float ?? */

/* Particle filter configuration constants */
/* Sensor measurement uncertainty m,m,rad */
const float_t sigma_meas[] = {
    PF_SIGMA_MEAS_X, /* X */
    PF_SIGMA_MEAS_Y, /* Y */
    PF_SIGMA_MEAS_TH  /* Theta */
};
const float_t sigma_landmark[] = {
    PF_SIGMA_LNDMRK_X, /*x*/
    PF_SIGMA_LNDMRK_Y, /*y*/
};

#define MAX_LOADSTRING 100
#define IDS_APP_TITLE           103
#define IDC_GPGPU2017A 12457
#define IDI_GPGPU2017A 103
#define IDI_SMALL               108
#define IDD_ABOUTBOX            103
#define IDM_ABOUT               104
#define IDM_EXIT                105
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CDXManager Manager;
/* The structure for the particle filter */
ID3D11ComputeShader* g_pCSParticleFilter;
/* Constant buffer
** This buffer will contain:
**   - control input
**   - current speed
**   - Current yaw
**   - Current yaw rate 
**   - Observation for the measurement
*/
ID3D11Buffer* g_pCBStepInfo;
/*RO Structured buffer which stores the landmark/map information*/
ID3D11Buffer* g_pSBMapInfo;
/*RW structured buffer which holds the information of the particles per step*/
ID3D11Buffer* g_pSBParticles;
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GPGPU2017A));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;// (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GPGPU2017A);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);

}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)   
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GPGPU2017A, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GPGPU2017A));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }
    auto pAdapter = Manager.ChooseDevice(NULL);
    Manager.Initialize(pAdapter, false, hWnd);
    if (pAdapter) pAdapter->Release();
    g_pCSParticleFilter = Manager.CompileComputeShader(L".\\hlsl\\P_F.hlsl", "main");
    g_pCBStepInfo = Manager.CreateConstantBuffer(sizeof(PF_STEP_DATA));

    Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(PF_STEP_DATA));


    MessageBox(NULL, L"Unable to load image", L"Error", MB_ICONERROR);
//    g_pCSSparkAnimate = Manager.CompileComputeShader(
//        L"..\\Shaders\\Fireworks.hlsl", "Animate");
//    g_pCSSparkRender = Manager.CompileComputeShader(
//        L"..\\Shaders\\Fireworks.hlsl", "Render");

    //StartParticles();
    SetTimer(hWnd, 1, 10, 0);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void InitParticles(void)
{
    volatile int n = 0;
    n++;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:

        break;
        //g_SparkParams.Position = {LOWORD(lParam), HIWORD(lParam), 00,1}
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            InvalidateRect(hWnd, NULL, false);
            break;
        }
        break;
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        Manager.Resize(rc.right, rc.bottom);
    }
    break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        //case VK_UP: g_Params.OffsetY += 0.005f; break;
        //case VK_DOWN: g_Params.OffsetY -= 0.005f; break;
        //case VK_LEFT: g_Params.OffsetX -= 0.005f; break;
        //case VK_RIGHT: g_Params.OffsetX += 0.005f; break;
        case 'R': InitParticles(); break;
        }
        Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(PF_STEP_DATA));
        InvalidateRect(hWnd, 0, 0);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        auto pCtx = Manager.GetContext();
        auto pDev = Manager.GetDevice();
        ID3D11UnorderedAccessView* pUAV = NULL;
        ID3D11RenderTargetView*  pRTV = NULL;
        ID3D11Texture2D* pBackBuffer = NULL;
        Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
        pDev->CreateUnorderedAccessView(pBackBuffer, NULL, &pUAV);
        pDev->CreateRenderTargetView(pBackBuffer, NULL, &pRTV);
        float Color[4] = { 0,0,0,0 };
        pCtx->ClearRenderTargetView(pRTV, Color);
        pRTV->Release();
        D3D11_TEXTURE2D_DESC dtd;
        pBackBuffer->GetDesc(&dtd);
        ID3D11Texture2D* pT1 = NULL;
        ID3D11Texture2D* pT2 = NULL;
        dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS
            | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        pDev->CreateTexture2D(&dtd, NULL, &pT1);
        pDev->CreateTexture2D(&dtd, NULL, &pT2);
        ID3D11UnorderedAccessView* pUAVT1 = 0;
        ID3D11UnorderedAccessView* pUAVT2 = 0;
        ID3D11ShaderResourceView* pSRVT1 = 0;
        ID3D11ShaderResourceView* pSRVT2 = 0;
        ID3D11RenderTargetView* pRTVT1 = 0;
        ID3D11RenderTargetView* pRTVT2 = 0;

        pDev->CreateUnorderedAccessView(pT1, NULL, &pUAVT1);
        pDev->CreateShaderResourceView(pT1, NULL, &pSRVT1);
        pDev->CreateRenderTargetView(pT1, NULL, &pRTVT1);


        pDev->CreateUnorderedAccessView(pT2, NULL, &pUAVT2);
        pDev->CreateShaderResourceView(pT2, NULL, &pSRVT2);
        //pDev->CreateRenderTargetView(pT2, NULL, &pRTVT2);
        pCtx->ClearRenderTargetView(pRTVT1, Color);
        //pCtx->ClearRenderTargetView(pRTVT2, Color);
        pBackBuffer->Release();
        /*
        g_SparkParams.dt = 1 / 60.0f;
        g_SparkParams.t += 1 / 60.0f;
        g_SparkParams.g = { 0.0f,50.0f,0,0 };
        g_SparkParams.left = 0;
        g_SparkParams.top = 0;
        g_SparkParams.right = dtd.Width - 1;
        g_SparkParams.bottom = dtd.Height - 1;
        g_SparkParams.kg = 80; //pix/s^2
        g_SparkParams.r = 50;
        g_SparkParams.nParticles = MAX_PARTICLES;
        */


        Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(PF_STEP_DATA));
        ID3D11UnorderedAccessView* pUAVSparks = NULL;
        pDev->CreateUnorderedAccessView(g_pSBMapInfo, NULL, &pUAVSparks);
        pCtx->CSSetShader(g_pCSParticleFilter, 0, 0);

        pCtx->CSSetUnorderedAccessViews(0, 1, &pUAVSparks, 0);
        pCtx->CSSetConstantBuffers(0, 1, &g_pCBStepInfo);
        pCtx->Dispatch((PF_PARTICLE_NUMBER + 63) / 64, 1, 1);
        /*
        pCtx->CSSetShader(g_pCSNBody, 0, 0);
        pCtx->Dispatch((MAX_PARTICLES + 63) / 64, 1, 1);

        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAVT1, 0);
        pCtx->CSSetShader(g_pCSSparkRender, 0, 0);
        pCtx->Dispatch((MAX_PARTICLES + 63) / 64, 1, 1);

        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAVT2, 0);
        pCtx->CSSetShaderResources(0, 1, &pSRVT1);
        pCtx->CSSetShader(g_pCSHBloor, 0, 0);
        pCtx->Dispatch((dtd.Width + 63) / 64, dtd.Height, 1);

        pCtx->CSSetShader(g_pCSVBloor, NULL, NULL);
        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAV, NULL);
        pCtx->CSSetShaderResources(0, 1, &pSRVT2);
        pCtx->Dispatch(dtd.Width, (dtd.Height + 63) / 64, 1);
        */
        pCtx->Flush(); // eliminar las referecias, sin desconectar ni liberar recursos


        pUAVSparks->Release();
#if 0
        pUAVT1->Release();
        pUAVT2->Release();
        pSRVT1->Release();
        pSRVT2->Release();
        pRTVT1->Release();
#endif // 0

        //		pRTVT2->Release();

        pT1->Release();
        pT2->Release();
        /*
        pCtx->CSSetShaderResources(0,1,&g_pSRVSource);
        pCtx->CSSetShader(g_pCSDefault,0,0);
        pCtx->CSSetConstantBuffers(0, 1, &g_pCB);
        pCtx->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);
        pCtx->Dispatch((dtd.Width+15)/16, (dtd.Height+15)/16, 1);
        */
        Manager.GetSwapChain()->Present(1, 0);
        pUAV->Release();
        ValidateRect(hWnd, NULL);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/*
int main()
{
    auto pAdapter = Manager.ChooseDevice(NULL);
    Manager.Initialize(pAdapter, false, )
    return 0;
}

*/

