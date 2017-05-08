// GPGPU2017A.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ParticleHelper.h"
#include "ParticleFilter.h"
#include "DXManager.h"
#include <math.h>
#include <random>
#include <iostream>
#include <vector>
#include <iomanip>
/* Implementation headers */
#include "PF_types.h"
#include "PF_cfg.h"

#define MAX_LOADSTRING 100



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

/* The structure for the particle filter */
ID3D11ComputeShader* g_pCSParticleFilter_Predict;
ID3D11ComputeShader* g_pCSParticleFilter_UpdateWeight;
ID3D11ComputeShader* g_pCSParticleFilter_Resample;
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

struct PF_STEP_DATA
{
    float velocity; /* Control input speed */
    float yaw_rate; /* Control input yaw rate */
    float x_noise;  /* Current measurement noise */
    float y_noise;  /* Current measurement noise */
    float th_noise; /* Current measurement noise */
    float sensor_range;
    float delta_t; // 0.1
    int32_t t; /* Time stamp */
    int32_t obs_n;    // number of observations in this step
    int32_t landmark_n;
    landmark_t obs[MAX_OBS_POINTS]; /* Array of observations */
}g_pf_step_data;




// Global Variables:
Map map;
HWND hWnd;
std::vector<control_t> position_meas;
particle_t* pParticles = new particle_t[MAX_PARTICLES];
std::vector<particle_t> gt;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CDXManager Manager;
ID3D11ComputeShader* g_pCSDefault;
ID3D11ComputeShader* g_pCSPostProcess;
ID3D11Texture2D*     g_pT2DSource;
ID3D11ShaderResourceView* g_pSRVSource;
ID3D11Buffer*        g_pCB;
ID3D11ComputeShader*  g_pCSSparkAnimate;
ID3D11ComputeShader*  g_pCSSparkRender;
ID3D11Buffer*         g_pCBSpark;
ID3D11Buffer*         g_pSBSparks;
ID3D11ComputeShader* g_pCSHBloor;
ID3D11ComputeShader* g_pCSVBloor;

ID3D11ComputeShader* g_pCSNBody;

struct SPARK_PARAMS
{
    union { float g[4]; }g;
    float dt;
    float t;
    float left;
    float right;
    float top;
    float bottom;
    // Particula con gravedad (gravity sources)
    float r; // radio de influencia de la esfera
    float kg; // constante gravitatoria
    float Position[4]; // posicion de la particula mayor
    unsigned long nParticles;
    //union { float Position[4]; } Position; // posicion de la particula mayor
}g_SparkParams;
struct PARAMS
{
    float OffsetX, OffsetY;
}g_Params;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
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
std::vector<landmark_t> observations;
void StartParticles()
{
    std::default_random_engine eng;
    /* Read the data files to get the information */
    if (!read_map_data("data/map_data.txt", map)) {
        MessageBox(hWnd, L"Error: no map file", L"No MapFile", 0);
        return;
    }

    // Read position data
    if (!read_control_data("data/control_data.txt", position_meas)) {
        MessageBox(hWnd, L"Error: Could not open position/control measurement file", L"No control file", 0);
        return;
    }

    // Read ground truth data
    if (!read_gt_data("data/gt_data.txt", gt)) {
        MessageBox(hWnd, L"Error: Could not open ground truth data file", L"No base data file", 0);
        return;
    }

    std::ostringstream obs_file;
    obs_file << "data/observation/observations_" << std::setfill('0') << std::setw(6) << g_pf_step_data.t + 1 << ".txt";

    if (!read_landmark_data(obs_file.str(), observations)) {
        MessageBox(hWnd, L"Error: Could not open observation file ", L"No observation file", 0);
        return;
    }

    std::normal_distribution<float> x_noise(gt[0].x, sigma_meas[0]);
    std::normal_distribution<float> y_noise(gt[0].y, sigma_meas[1]);
    std::normal_distribution<float> th_noise(gt[0].th, sigma_meas[2]);

    if (g_pSBParticles) g_pSBParticles->Release();
    //Crear las particulas y sus valores iniciales

    memset(pParticles, 0, sizeof(particle_t) * MAX_PARTICLES);
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        pParticles[i].x = x_noise(eng);
        pParticles[i].y = y_noise(eng);
        pParticles[i].th = th_noise(eng);
        pParticles[i].id = -1;
        pParticles[i].w = 1.0f;
    }
    g_pSBParticles = Manager.LoadBuffer(pParticles, sizeof(particle_t), MAX_PARTICLES);
    g_pf_step_data.t = 0;
    
}

/*****************************************************************************
Updates the data from the files and configurations to pass it to the constant
buffer.
*****************************************************************************/
void updatePfInputData()
{
    g_pf_step_data.sensor_range = SENSOR_RANGE;
    g_pf_step_data.delta_t = 0.1f;
    g_pf_step_data.x_noise = sigma_landmark[0];
    g_pf_step_data.y_noise = sigma_landmark[1];
    g_pf_step_data.th_noise = sigma_meas[2];

    std::default_random_engine eng;
    std::normal_distribution<float> x_obs_noise(0, sigma_landmark[0]);
    std::normal_distribution<float> y_obs_noise(0, sigma_landmark[1]);

    g_pf_step_data.velocity = position_meas[g_pf_step_data.t].velocity;
    g_pf_step_data.yaw_rate = position_meas[g_pf_step_data.t].yaw_rate;
    g_pf_step_data.obs_n = observations.size();
    g_pf_step_data.landmark_n = map.landmark_list.size();
    for (size_t i = 0; i < observations.size() && i < MAX_OBS_POINTS; i++)
    {
        /* Add noise to the measurements of the landmarks */
        //g_pf_step_data.obs[i] = observations[i];
        g_pf_step_data.obs[i] = landmark_t{ observations[i].x + x_obs_noise(eng), observations[i].y + y_obs_noise(eng), observations[i].id };
    }
    g_pf_step_data.t += 1;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }
    auto pAdapter = Manager.ChooseDevice(NULL);
    Manager.Initialize(pAdapter, false, hWnd);
    if (pAdapter) pAdapter->Release();
    /* Given the control input update the position for each particle */
    g_pCSParticleFilter_Predict = Manager.CompileComputeShader(L"..\\Shaders\\P_F.hlsl", "predict");
    /* Associate each particle with a landmark and get the multivariate probability */
    //  g_pCSParticleFilter_UpdateWeight = Manager.CompileComputeShader(L"..\\Shaders\\P_F.hlsl", "update_weight");
    /* Apply the metropolis resample strategy */
    //  g_pCSParticleFilter_Resample = Manager.CompileComputeShader(L"..\\Shaders\\P_F.hlsl", "resample");

    /* fill the data for this step */
    g_pCBStepInfo = Manager.CreateConstantBuffer(sizeof(PF_STEP_DATA));
    Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(PF_STEP_DATA));

    StartParticles();
    SetTimer(hWnd, 1, 1, 0);
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
    case WM_MOUSEMOVE:
        g_SparkParams.Position[0] = (float)LOWORD(lParam);
        g_SparkParams.Position[1] = (float)HIWORD(lParam);
        g_SparkParams.Position[2] = 0.0f;
        g_SparkParams.Position[3] = 1.0f;
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
        case VK_UP: g_Params.OffsetY += 0.005f; break;
        case VK_DOWN: g_Params.OffsetY -= 0.005f; break;
        case VK_LEFT: g_Params.OffsetX -= 0.005f; break;
        case VK_RIGHT: g_Params.OffsetX += 0.005f; break;
        case 'R': StartParticles(); break;
        }
        Manager.UpdateConstantBuffer(g_pCB, &g_Params, sizeof(PARAMS));
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
#if 1
        if (g_pf_step_data.t < 2444) {            // run only for the available observations
            auto pISBMapData = Manager.LoadBuffer(&map.landmark_list[0], sizeof(Map::single_landmark_s), map.landmark_list.size());
            auto pISBParticleIn = Manager.LoadBuffer(pParticles, sizeof(particle_t), MAX_PARTICLES);
            auto pCtx = Manager.GetContext();
            //auto pISBParticleOut = Manager.LoadBuffer(NULL, sizeof(Map::single_landmark_s), MAX_PARTICLES);
            auto pISBParticleOut = Manager.LoadBuffer(NULL, sizeof(particle_t), MAX_PARTICLES);
            ID3D11ShaderResourceView* pSRV = NULL;
            ID3D11ShaderResourceView* pSRVMapData = NULL;
            ID3D11UnorderedAccessView* pUAV = NULL;
            Manager.GetDevice()->CreateShaderResourceView(pISBParticleIn, NULL, &pSRV);
            Manager.GetDevice()->CreateShaderResourceView(pISBMapData, NULL, &pSRVMapData);
            Manager.GetDevice()->CreateUnorderedAccessView(pISBParticleOut, NULL, &pUAV);

            //Manager.GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);

            ID3D11Texture2D* pBackBuffer = 0;
            Manager.GetSwapChain()->GetBuffer(0,/* Numero ordinal del buffer */
                IID_ID3D11Texture2D,
                (void**)&pBackBuffer);
            //        ID3D11UnorderedAccessView* pUAV = 0;
            //Manager.GetDevice()->CreateUnorderedAccessView(pBackBuffer, NULL, &pUAV); // crear la vista
            updatePfInputData();
            Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(g_pf_step_data));

            //Manager.GetDevice()->CreateUnorderedAccessView(pISBParticleOut, NULL, &pUAV); // crear la vista
            Manager.GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL); //conectar la vista
            Manager.GetContext()->CSSetShader(g_pCSParticleFilter_Predict, NULL, NULL);
            Manager.GetContext()->CSSetShaderResources(0, 1, &pSRV);
            Manager.GetContext()->CSSetShaderResources(1, 1, &pSRVMapData);
            pCtx->CSSetConstantBuffers(0, 1, &g_pCBStepInfo);        // set the constant buffer
            Manager.GetContext()->Dispatch(MAX_PARTICLES, 1, 1); //
            //Manager.StoreBuffer(pISBParticleOut, sizeof(Map::single_landmark_s), MAX_PARTICLES, pParticles);
            Manager.StoreBuffer(pISBParticleOut, sizeof(particle_t), MAX_PARTICLES, pParticles);
            std::ofstream out_txt;
            out_txt.open("pf_output.txt", std::ofstream::out | std::ofstream::app);

            /* Find the best particle: */
            particle_t best;
            float highest_weight = 0.0;
            for (size_t i = 0; i < MAX_PARTICLES; i++)
            {
                if (pParticles[i].w > highest_weight) {
                    highest_weight = pParticles[i].w;
                    best = pParticles[i];
                }
            }
            /* Resampling the data */
            std::vector<particle_t> resampled_part(pParticles, pParticles + MAX_PARTICLES);
            std::default_random_engine eng;
            std::uniform_int_distribution<int> indx_rnd(0, MAX_PARTICLES - 1);
            int index = indx_rnd(eng);
            double mw = best.w;//*(std::max_element(weights.begin(), weights.end()));
            std::uniform_real_distribution<double> beta_rnd(0, 2.0f * mw);
            double beta = 0.0;
            for (int i = 0; i < MAX_PARTICLES; ++i) {
                beta += beta_rnd(eng);
                while (beta > pParticles[index].w)
                {
                    beta -= pParticles[index].w;
                    index = (index + 1) % MAX_PARTICLES;
                }
                resampled_part.push_back(particle_t{pParticles[index].x,pParticles[index].y, pParticles[index].th, 1.0f, -1 });
            }
            //weights.clear();
            pParticles = &resampled_part[0];
            /*******************************************************/

            std::cout << best.x;
            std::cout << gt[g_pf_step_data.t].x;
            out_txt << g_pf_step_data.t << ", " << best.x << ", " << best.y << ", " << best.th << "\n";
            out_txt.close();
            pUAV->Release();
            pBackBuffer->Release();
            Manager.GetSwapChain()->Present(1, 0);
            ValidateRect(hWnd, NULL);
        }
        else
        {
            MessageBox(hWnd, L"Done", L"No MapFile",1);
        }
#elif 0
        auto pCtx = Manager.GetContext();
        auto pDev = Manager.GetDevice();
        ID3D11UnorderedAccessView* pUAV = NULL;
        ID3D11RenderTargetView*  pRTV = NULL;
        ID3D11Texture2D* pBackBuffer = NULL;

        auto pISB


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
        //ID3D11UnorderedAccessView* pUAVT2 = 0;
        ID3D11ShaderResourceView* pSRVT1 = 0;
        //ID3D11ShaderResourceView* pSRVT2 = 0;
        ID3D11RenderTargetView* pRTVT1 = 0;
        //ID3D11RenderTargetView* pRTVT2 = 0;

        pDev->CreateUnorderedAccessView(pT1, NULL, &pUAVT1);
        pDev->CreateShaderResourceView(pT1, NULL, &pSRVT1);
        pDev->CreateRenderTargetView(pT1, NULL, &pRTVT1);


        //pDev->CreateUnorderedAccessView(pT2, NULL, &pUAVT2);
        //pDev->CreateShaderResourceView(pT2, NULL, &pSRVT2);
        //pDev->CreateRenderTargetView(pT2, NULL, &pRTVT2);
        pCtx->ClearRenderTargetView(pRTVT1, Color);
        //pCtx->ClearRenderTargetView(pRTVT2, Color);
        pBackBuffer->Release();
        //g_SparkParams.dt = 1 / 60.0f;
        //g_SparkParams.t += 1 / 60.0f;
        //g_SparkParams.g = { 0.0f,50.0f,0,0 };
        //g_SparkParams.left = 0;
        //g_SparkParams.top = 0;
        //g_SparkParams.right = dtd.Width-1;
        //g_SparkParams.bottom = dtd.Height-1;
        //g_SparkParams.kg = 80; //pix/s^2
        //g_SparkParams.r = 50;

        std::ostringstream obs_file;
        obs_file << "data/observation/observations_" << std::setfill('0') << std::setw(6) << g_pf_step_data.t + 1 << ".txt";

        if (!read_landmark_data(obs_file.str(), observations)) {
            MessageBox(hWnd, L"Error: Could not open observation file ", L"No observation file", 0);
            return -1;
        }
        g_pf_step_data.t += 1;
        //g_SparkParams.nParticles = MAX_PARTICLES;
        g_pf_step_data.velocity = 0;
        g_pf_step_data.yaw_rate = 0;
        g_pf_step_data.x_noise = 0;
        g_pf_step_data.y_noise = 0;
        g_pf_step_data.th_noise = 0;
        g_pf_step_data.sensor_range = 2;
        g_pf_step_data.t += 1;
        for (int i = 0; i < MAX_OBS_POINTS; i++)
        {
            g_pf_step_data.obs[i] = 0; // TODO: Copy observation data
        }

        Manager.UpdateConstantBuffer(g_pCBStepInfo, &g_pf_step_data, sizeof(PF_STEP_DATA));
        ID3D11UnorderedAccessView* pUAVParticles = NULL;
        pDev->CreateUnorderedAccessView(g_pSBParticles, NULL, &pUAVParticles);
        pCtx->CSSetShader(g_pCSParticleFilter_Predict, 0, 0);

        pCtx->CSSetUnorderedAccessViews(0, 1, &pUAVParticles, 0);
        pCtx->CSSetConstantBuffers(0, 1, &g_pCBStepInfo);
        pCtx->Dispatch((MAX_PARTICLES + 63) / 64, 1, 1);
        /*
        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAVT2,0);
        pCtx->CSSetShaderResources(0, 1, &pSRVT1);
        pCtx->CSSetShader(g_pCSHBloor, 0, 0);
        pCtx->Dispatch((dtd.Width + 63) / 64, dtd.Height, 1);

        pCtx->CSSetShader(g_pCSVBloor, NULL, NULL);
        pCtx->CSSetUnorderedAccessViews(1, 1, &pUAV, NULL);
        pCtx->CSSetShaderResources(0, 1, &pSRVT2);
        pCtx->Dispatch(dtd.Width,( dtd.Height+63)/64, 1);
        */
        pCtx->Flush(); // eliminar las referecias, sin desconectar ni liberar recursos


        pUAVParticles->Release();
        //		pUAVT1->Release();
        //		pUAVT2->Release();
        //		pSRVT1->Release();
        //		pSRVT2->Release();
        //		pRTVT1->Release();
        //		pRTVT2->Release();

        //		pT1->Release();
        //		pT2->Release();
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
#endif
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

