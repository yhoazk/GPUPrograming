// GPGPU2017A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GPGPU2017A.h"
#include "DXManager.h"
#include <math.h>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CDXManager Manager;
ID3D11ComputeShader* g_pCSDefault;
ID3D11ComputeShader* g_pCSPostProcess;
ID3D11Texture2D* g_pT2dSource;
ID3D11Buffer* g_pCB;
ID3D11ShaderResourceView* g_SRVSource;
ID3D11ComputeShader* g_pCSSparkAnimate;
ID3D11ComputeShader* g_pCSSparkRender;
ID3D11Buffer* g_pCBSpark;
ID3D11Buffer* g_pSBSparks;

struct SPARK
{
    float Position[4];
    float Velocidad[4];
    union { float Color[4]; } Color;
    float MaxAge;

};


/* Siemrpe alinea los elementos del struct a un multiplo de 16 bytes */
struct SPARK_PARAMS
{
    float g[4];
    float dt[4]; // rellenar con 3 elementos extras
    float t;
};



struct PARAMS
{
    float OffsetX, OffsetY;

}g_Params;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
struct  VECTOR
{
    float x, y, z, w;
};
VECTOR xInput = { 1,2,3,4 };
VECTOR yOutput = { 0,0,0,0 };
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
 /*
	auto pAdapter = Manager.ChooseDevice(NULL);
	Manager.Initialize(pAdapter);
	pCSDefault = Manager.CompileComputeShader(
		L"..\\Shaders\\Default.hlsl", "main");
    auto pISBInput = Manager.LoadBuffer(&xInput, sizeof(VECTOR), 1);
    auto pISBOutput = Manager.LoadBuffer(NULL, sizeof(VECTOR), 1);
   // Manager.StoreBuffer(pISB, sizeof(VECTOR), 1, &yOutput);
    ID3D11ShaderResourceView* pSRV = NULL;
    ID3D11UnorderedAccessView* pUAV = NULL;
    Manager.GetDevice()->CreateShaderResourceView(pISBInput, NULL, &pSRV);
    Manager.GetDevice()->CreateUnorderedAccessView(pISBOutput, NULL, &pUAV);
    Manager.GetContext()->CSSetShaderResources(0, 1, &pSRV );
    Manager.GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);


	if (pCSDefault)
	{
		Manager.GetContext()->CSSetShader(pCSDefault,0,0);
		Manager.GetContext()->Dispatch(1, 1, 1);    // dispatch groups
        Manager.StoreBuffer(pISBOutput, sizeof(VECTOR), 1, &yOutput);
	}

	if (pAdapter) pAdapter->Release();
    pISBInput->Release();
    pISBOutput->Release();
    pSRV->Release();
    pUAV->Release();*/
    
    
   
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GPGPU2017A, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
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
    
    return (int) 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GPGPU2017A));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GPGPU2017A);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
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
   g_pCSDefault = Manager.CompileComputeShader(
       L"..\\Shaders\\FractalJulia.hlsl", "main");
   g_pCB = Manager.CreateConstantBuffer(sizeof(PARAMS));
   Manager.UpdateConstantBuffer(g_pCB, &g_Params, sizeof(PARAMS));
   g_pCSPostProcess = Manager.CompileComputeShader(
       L"..\\Shaders\\PostProcess.hlsl", "main");
   g_pT2dSource = Manager.LoadTexture("C:\\Users\\uidw7238\\Pictures\\test.bmp", 1, &g_SRVSource, NULL);
   if (!g_pT2dSource)
       MessageBox(NULL, L"Unable to load image", L"Error", MB_ICONERROR);
   g_pCSSparkAnimate = Manager.CompileComputeShader(
       L"..\\Shaders\\Fireworks.hlsl", "Animate");
   g_pCSSparkRender = Manager.CompileComputeShader(
       L"..\\Shaders\\Fireworks.hlsl", "Render");
   g_pCBSpark = Manager.CreateConstantBuffer(sizeof(SPARK_PARAMS));
   // crear las particulas y sus valores iniciales
   struct SPARK
   {
       float Position[4];
       float Velocity[4];
    union { float Color[4]; } Color;
       float MaxAge;
   };
   SPARK* pSparks = new SPARK[32000];
   memset(pSparks, 0, sizeof(SPARK) * 32000);
   for (int i = 0; i < 32000; i++)
   {
       pSparks[i].Position[0] = 512;
       pSparks[i].Position[1] = 512;
       pSparks[i].Position[1] = 1;

       float theta = 2 * 3.141592f*rand() / 32768.0f;
       pSparks[i].Velocity[0] = cos(theta) * 10;
       pSparks[i].Velocity[1] = sin(theta) * 10;
       pSparks[i].MaxAge = 10 * rand() / 327668.0f + 10.0f;
       pSparks[i].Color = { 1,1,1,1 };
   }
   g_pSBSparks = Manager.LoadBuffer(pSparks, sizeof(SPARK) , 32000);
   delete[] pSparks;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        Manager.Resize(rc.right, rc.bottom);
    }
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
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP: g_Params.OffsetY += 0.01; break;
        case VK_DOWN:g_Params.OffsetY -= 0.01; break;
        case VK_LEFT:  g_Params.OffsetX += 0.01; break;
        case VK_RIGHT: g_Params.OffsetX -= 0.01; break;

        default:
            break;
        }
        Manager.UpdateConstantBuffer(g_pCB, &g_Params, sizeof(PARAMS));
        InvalidateRect(hWnd, 0, 0);

        break;

    case WM_PAINT:
        {
          /*  PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps); PARA INTERFAZ DE NEGOCIOS(?)  
        ID3D11Texture2D* pBackBuffer = 0;
        Manager.GetSwapChain()->GetBuffer(0,
            IID_ID3D11Texture2D,
            (void**)&pBackBuffer);
        ID3D11UnorderedAccessView* pUAV = 0;
        Manager.GetDevice()->CreateUnorderedAccessView(pBackBuffer, NULL, &pUAV); // crear la vista
        Manager.GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL); //conectar la vista
        Manager.GetContext()->CSSetShader(g_pCSDefault, NULL, NULL);
        Manager.GetContext()->Dispatch(200, 200, 1);

        pUAV->Release();
        pBackBuffer->Release();
        Manager.GetSwapChain()->Present(1, 0); */

        auto pCtx = Manager.GetContext();
        auto pDev = Manager.GetDevice();
        ID3D11UnorderedAccessView* pUAV = NULL;
        ID3D11Texture2D* pBackBuffer = NULL;
        Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
        pDev->CreateUnorderedAccessView(pBackBuffer, NULL, &pUAV);
        D3D11_TEXTURE2D_DESC dtd;
        pBackBuffer->GetDesc(&dtd);
        pBackBuffer->Release();
        g_SpakParams.dt[0] = 1 / 60.0f;
        Manager.UpdateConstantBuffer(g_pCBSpark, &g_SparkParams, sizeof(g_SparkParams));
           
        ID3D11UnorderedAccessView* pUAVSparks = NULL;
        pDev->CreateUnorderedAccessView(g_pSBSparks, NULL, &pUAVSparks);
        pCtx->CSSetShader(g_pCSSparkAnimate, 0, 0);
        pCtx->CSSetUnorderedAccessViews(0, 1, &pUAVSparks, 0);
        pCtx->CSSetConstantBuffers(0, 1, &g_pCBSpark);
        pCtx->Dispatch((32000 + 63) / 64, 1, 1);
        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAV, 0);
        pCtx->CSSetShader(g_pCSSparkRender, 0, 0);
        pCtx->Dispatch((32000 + 63) / 64, 1, 1);


        pCtx->CSSetShaderResources(0, 1, &g_SRVSource);


        //pCtx->CSSetShader(g_pCSDefault, 0, 0);
        pCtx->CSSetShader(g_pCSPostProcess, 0, 0);
        pCtx->CSSetConstantBuffers(0, 1, &g_pCB);
        pCtx->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);
        pCtx->Dispatch((dtd.Width+15)/16, (dtd.Height+15)/16, 1);
        //pCtx->Dispatch(1024 / 64, 1024 / 16, 1);
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
