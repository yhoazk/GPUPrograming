#include "stdafx.h"
#include "DXManager.h"


CDXManager::CDXManager()
{
	m_pDevice = NULL;
	m_pContext = NULL;
}
CDXManager::~CDXManager()
{
}

IDXGIAdapter* CDXManager::ChooseDevice(HWND hWnd)
{
	IDXGIFactory* pFactory = NULL;
	CreateDXGIFactory(IID_IDXGIFactory, (void**)&pFactory);
	IDXGIAdapter* pAdapter = NULL;
	int iAdapter = 0;
	do
	{
		pAdapter = NULL;
		if (FAILED(pFactory->EnumAdapters(iAdapter, &pAdapter)))
			break;
		DXGI_ADAPTER_DESC dad;
		pAdapter->GetDesc(&dad);
		wchar_t szMessage[1024];
		wsprintf(szMessage, L"¿Desea utilizar éste dispositivo?\r\nDescripción:%s",
			dad.Description);
		switch (MessageBox(hWnd, szMessage, L"Selección de dispositivo",
			MB_ICONQUESTION | MB_YESNOCANCEL))
		{
		case IDYES:
			pFactory->Release();
			return pAdapter;
		case IDCANCEL:
			pFactory->Release();
			pAdapter->Release();
			return NULL;
		case IDNO:
			pAdapter->Release();
			break;
		}
		iAdapter++;
	} while (1);
	pFactory->Release();
	return NULL;
}
bool CDXManager::Initialize(IDXGIAdapter* pAdapter, bool bUseCPU, HWND hWnd)
{
	D3D_FEATURE_LEVEL Requested=D3D_FEATURE_LEVEL_11_0, Detected;
    /* Definir salida grafica */
    DXGI_SWAP_CHAIN_DESC  dscd;// tipo de informacion, para mostrar
    memset(&dscd, 0, sizeof(dscd));

    dscd.BufferCount = 2; // dos buffers uno para mostrar y otro para dibujar
    dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS; // le dice que puede garficar, el acceso a la cadena es solo de escritura

    dscd.OutputWindow = hWnd;
    dscd.Windowed = true;
    dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // cambia el backbuffer con el front pero se descarta el front, no conserva el cuadro anterior
    dscd.SampleDesc.Count = 1; // relacion 1:1 back  yfront buffer
    dscd.SampleDesc.Quality = 0;
    dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dscd.BufferDesc.Width = 1280;
    dscd.BufferDesc.Height = 720;
    dscd.BufferDesc.RefreshRate.Numerator = 0;      // define la tasa de intercambio en Hz 0/0 es automatica
    dscd.BufferDesc.RefreshRate.Denominator = 0;
    dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED; // como ajusta el buffer a la ventana
    dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;  //orden en que la informacion viaja hacia el monitor interleaved or progressive
    // progressive linea por linea, interleaved nones primero y pares despues
    dscd.Flags = 0;                                                                     // short cut: 



	if (bUseCPU)
	{
		HRESULT hr=!hWnd?
		D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
			&Requested, 1, D3D11_SDK_VERSION, &m_pDevice, &Detected, 
			&m_pContext):
            D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                &Requested, 1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain,
                &m_pDevice, &Detected,
                &m_pContext);
		if (FAILED(hr))
			return false;
	}
	else if (pAdapter)
	{
		HRESULT hr = !hWnd ?
            D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                &Requested, 1, D3D11_SDK_VERSION, &m_pDevice, &Detected,
                &m_pContext) :
            D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                &Requested, 1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain,
                &m_pDevice, &Detected,
                &m_pContext);
		if (FAILED(hr)) return false;
	}
	else
	{
		HRESULT hr = !hWnd ?
            D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                &Requested, 1, D3D11_SDK_VERSION, &m_pDevice, &Detected,
                &m_pContext) :
            D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
                &Requested, 1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain,
                &m_pDevice, &Detected,
                &m_pContext);
		if (FAILED(hr)) return false;
	}
	return true;
}
ID3D11ComputeShader* CDXManager::CompileComputeShader(wchar_t* pszFileName,
	char* pszEntryPoint)
{
	ID3DBlob* pDXIL = 0;  //DirectX Intermediate Language
	ID3DBlob* pErrors = 0;
	HRESULT hr=D3DCompileFromFile(pszFileName, NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, pszEntryPoint, "cs_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
	    0, &pDXIL, &pErrors);
	if (pErrors)
	{
		MessageBoxA(NULL, (char*)pErrors->GetBufferPointer(),
			"Errores o advertencias", MB_ICONEXCLAMATION);
		pErrors->Release();
	}
	if (pDXIL)
	{
		ID3D11ComputeShader* pCS = 0;
		HRESULT hr=m_pDevice->CreateComputeShader(
			pDXIL->GetBufferPointer(), pDXIL->GetBufferSize(),
			NULL, &pCS);
		pDXIL->Release();
		if (FAILED(hr)) return NULL; else return pCS;
	}
	return NULL;
}
void CDXManager::Uninitialize()
{
}

/*Load from CPU to GPU*/
ID3D11Buffer* CDXManager::LoadBuffer(void* pData, unsigned long ulElementSize, unsigned long ulElements)
{
    D3D11_BUFFER_DESC dbd;
    //memset(&dbd, 0, sizeof);
        dbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    dbd.ByteWidth = ulElements*ulElementSize;
    /*                       GPU        CPU
    D3D11_USAGE_DEFAULT      WR         n/a
    D3D11_USAGE_IMMUTABLE    R          W once
    D3D11_USAGE_DYNAMIC      R          W
    D3D11_USAGE_STAGING      n/a        RW
    */
    dbd.Usage = D3D11_USAGE_DEFAULT;
    dbd.StructureByteStride = ulElementSize;
    dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    dbd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA dsd;
    dsd.pSysMem = pData;
    dsd.SysMemPitch = 0;// para buffers tamaño de cada fila 
    dsd.SysMemSlicePitch = 0;
    ID3D11Buffer* pNewBuffer = 0;
    m_pDevice->CreateBuffer(&dbd, pData?&dsd:0, &pNewBuffer);
    return pNewBuffer;

                //DXGICreateS
}
/*From GPU to CPU*/
void CDXManager::StoreBuffer(ID3D11Buffer* pISB, unsigned long ulElemenSize, unsigned long ulElements, void* pOutData) //isb interface structured buffer
{
    ID3D11Buffer* pStage = 0;
    D3D11_BUFFER_DESC dbd;

    dbd.BindFlags = 0;
    dbd.ByteWidth = ulElemenSize * ulElements;
    dbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    dbd.StructureByteStride = ulElemenSize;
    dbd.Usage = D3D11_USAGE_STAGING;

    m_pDevice->CreateBuffer(&dbd, NULL, &pStage);
    m_pContext->CopyResource(pStage, pISB);

    D3D11_MAPPED_SUBRESOURCE ms;
    // missing code here?
    m_pContext->Map(pStage, 0, D3D11_MAP_READ, 0, &ms);  //<- error ms?
    memcpy(pOutData, ms.pData, ulElemenSize*ulElements);
    /*Release kernel memory*/
    m_pContext->Unmap(pStage, 0);
    /* release the resource in the ssytem  */
    pStage->Release();


}

ID3D11Buffer* CDXManager::CreateConstantBuffer(unsigned long ulBytes) {
    ID3D11Buffer* pCB;
    D3D11_BUFFER_DESC dbd;
    memset(&dbd, 0, sizeof(dbd));
    dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dbd.ByteWidth = 16 * ((ulBytes + 15) / 16);
    dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dbd.Usage = D3D11_USAGE_DYNAMIC; // cpu:w gpu:r

    m_pDevice->CreateBuffer(&dbd, 0, &pCB);
    return pCB;
         


}
void CDXManager::UpdateConstantBuffer(ID3D11Buffer* pCB, void* pSource, unsigned long ulBytes) {
    D3D11_MAPPED_SUBRESOURCE ms;
    m_pContext->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, pSource, ulBytes);
    m_pContext->Unmap(pCB, 0);
}

#include <stdio.h>
#include <fstream>
using namespace std;
ID3D11Texture2D* CDXManager::LoadTexture(
    char* pszFileName,		//The file name
    unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
    ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
    float(*pAlphaFunction)(float r, float g, float b))  //Operates pixel's rgb channels to build an alpha channel (normalized), can be null
{
    ID3D11Device* pDev = GetDevice();
    ID3D11DeviceContext* pCtx = GetContext();
    printf("Loading %s...\n", pszFileName);
    fstream bitmap(pszFileName, ios::in | ios::binary);
    if (!bitmap.is_open())
    {
        printf("Error: Unable to open file %s\n", pszFileName);
        return NULL;
    }
    //Verificar el numeo magico de un bmp
    BITMAPFILEHEADER bfh;
    bitmap.read((char*)&bfh.bfType, sizeof(bfh.bfType));
    if (!(bfh.bfType == 'MB'))
    {
        printf("Error: Not a DIB File\n");
        return NULL;
    }
    //Leerel resto de los datos
    bitmap.read((char*)&bfh.bfSize, sizeof(bfh) - sizeof(bfh.bfType));

    BITMAPINFOHEADER bih;
    bitmap.read((char*)&bih.biSize, sizeof(bih.biSize));
    if (bih.biSize != sizeof(BITMAPINFOHEADER))
    {
        printf("Error: Unsupported DIB file format.");
        return NULL;
    }
    bitmap.read((char*)&bih.biWidth, sizeof(bih) - sizeof(bih.biSize));
    RGBQUAD Palette[256];
    unsigned long ulRowlenght = 0;
    unsigned char* pBuffer = NULL;

    ID3D11Texture2D* pTemp; //CPU memory
    D3D11_TEXTURE2D_DESC dtd;
    memset(&dtd, 0, sizeof(D3D11_TEXTURE2D_DESC));
    dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dtd.ArraySize = 1;
    dtd.BindFlags = 0;
    dtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    dtd.Height = bih.biHeight;
    dtd.Width = bih.biWidth;
    dtd.Usage = D3D11_USAGE_STAGING;
    dtd.MipLevels = min(MipMapLevels, 1 + (unsigned long)floor(log(min(dtd.Width, dtd.Height)) / log(2)));
    dtd.SampleDesc.Count = 1;
    dtd.SampleDesc.Quality = 0;

    printf("Width %d, Height:%d, %dbpp\n", bih.biWidth, bih.biHeight, bih.biBitCount);
    fflush(stdout);
    m_pDevice->CreateTexture2D(&dtd, NULL, &pTemp);
    struct PIXEL
    {
        unsigned char r, g, b, a;
    };
    D3D11_MAPPED_SUBRESOURCE ms;
    pCtx->Map(pTemp, 0, D3D11_MAP_READ_WRITE, 0, &ms);
    char *pDestStart = (char*)ms.pData + (bih.biHeight - 1)*ms.RowPitch;
    PIXEL *pDest = (PIXEL*)pDestStart;
    switch (bih.biBitCount)
    {
    case 1: //Tarea 1bpp 2 colores
        if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
            bitmap.read((char*)Palette, 2 * sizeof(RGBQUAD));
        else
            bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));
        ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
        pBuffer = new unsigned char[ulRowlenght];
        for (int y = (bih.biHeight - 1); y >= 0; y--)
        {
            bitmap.read((char*)pBuffer, ulRowlenght);
            int x = 0;
            for (unsigned long iByte = 0; iByte<ulRowlenght; iByte++)
            {
                unsigned long iColorIndex;
                unsigned char c = pBuffer[iByte];
                for (int iBit = 0; iBit<8; iBit++)
                {
                    iColorIndex = ((c & 0x80) != 0);
                    c <<= 1;
                    pDest->r = Palette[iColorIndex].rgbRed;
                    pDest->g = Palette[iColorIndex].rgbGreen;
                    pDest->b = Palette[iColorIndex].rgbBlue;
                    if (pAlphaFunction)
                        pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
                    else
                        pDest->a = 0xff;
                    x++;
                    pDest++;
                    if (x<bih.biWidth)
                        continue;
                }
            }
            pDestStart -= ms.RowPitch;
            pDest = (PIXEL*)pDestStart;
        }
        delete[] pBuffer;
        break;
    case 4: //Aquí 4 bpp 16 colores
        if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
            bitmap.read((char*)Palette, 16 * sizeof(RGBQUAD));
        else
            bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));
        //Leer el bitmap
        ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
        pBuffer = new unsigned char[ulRowlenght];
        for (int y = (bih.biHeight - 1); y >= 0; y--)
        {
            bitmap.read((char*)pBuffer, ulRowlenght);
            for (int x = 0; x<bih.biWidth; x++)
            {
                //Desempacar pixeles así
                unsigned char nibble = (x & 1) ? (pBuffer[x >> 1] & 0x0f) : (pBuffer[x >> 1] >> 4);
                pDest->r = Palette[nibble].rgbRed;
                pDest->b = Palette[nibble].rgbBlue;
                pDest->g = Palette[nibble].rgbGreen;
                if (pAlphaFunction)
                    pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
                else
                    pDest->a = 0xff;
                pDest++;
            }
            pDestStart -= ms.RowPitch;
            pDest = (PIXEL*)pDestStart;
        }
        delete[] pBuffer;
        break;
    case 8: //Tarea 8 bpp 256 colores
        if (bih.biClrUsed == 0)//Si se usan todos los colores, ese miembro es cero
            bitmap.read((char*)Palette, 256 * sizeof(RGBQUAD));
        else
            bitmap.read((char*)Palette, bih.biClrUsed * sizeof(RGBQUAD));

        ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
        pBuffer = new unsigned char[ulRowlenght];

        for (int y = (bih.biHeight - 1); y >= 0; y--)
        {
            bitmap.read((char*)pBuffer, ulRowlenght);
            for (int x = 0; x<bih.biWidth; x++)
            {
                //Desempacar pixeles así
                unsigned char nibble = (x & 1) ? (pBuffer[x] & 0xff) : (pBuffer[x]);
                pDest->r = Palette[nibble].rgbRed;
                pDest->b = Palette[nibble].rgbBlue;
                pDest->g = Palette[nibble].rgbGreen;
                if (pAlphaFunction)
                    pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
                else
                    pDest->a = 0xff;
                pDest++;
            }
            pDestStart -= ms.RowPitch;
            pDest = (PIXEL*)pDestStart;
        }
        delete[] pBuffer;
        break;
    case 24://Aquí 16777216 colores (True Color)
            //Leer el bitmap
        ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
        pBuffer = new unsigned char[ulRowlenght];
        for (int y = (bih.biHeight - 1); y >= 0; y--)
        {
            bitmap.read((char*)pBuffer, ulRowlenght);
            for (int x = 0; x<bih.biWidth; x++)
            {
                //Desempacar pixeles así
                pDest->b = pBuffer[3 * x + 0];
                pDest->g = pBuffer[3 * x + 1];
                pDest->r = pBuffer[3 * x + 2];
                if (pAlphaFunction)
                    pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
                else
                    pDest->a = 0xff;
                pDest++;
            }
            pDestStart -= ms.RowPitch;
            pDest = (PIXEL*)pDestStart;
        }
        delete[] pBuffer;
        break;
    case 32:
        ulRowlenght = 4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
        pBuffer = new unsigned char[ulRowlenght];
        for (int y = (bih.biHeight - 1); y >= 0; y--)
        {
            bitmap.read((char*)pBuffer, ulRowlenght);
            for (int x = 0; x<bih.biWidth; x++)
            {
                //Desempacar pixeles así
                pDest->b = pBuffer[4 * x + 0];
                pDest->g = pBuffer[4 * x + 1];
                pDest->r = pBuffer[4 * x + 2];
                if (pAlphaFunction)
                    pDest->a = (unsigned char)max(0, min(255, (int)(255 * pAlphaFunction(pDest->r / 255.0f, pDest->g / 255.0f, pDest->b / 255.0f))));
                else
                    pDest->a = 0xff;
                pDest++;
            }
            pDestStart -= ms.RowPitch;
            pDest = (PIXEL*)pDestStart;
        }
        delete[] pBuffer;
        break;
    }
    //transfer cpu mem to gpu memory
    pCtx->Unmap(pTemp, 0);
    //Crear buffer en GPU
    dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    dtd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    dtd.Usage = D3D11_USAGE_DEFAULT;
    dtd.CPUAccessFlags = 0;
    ID3D11Texture2D* pTexture;
    pDev->CreateTexture2D(&dtd, NULL, &pTexture);
    //copy gpu mem to gpu mem for RW capable surface
    pCtx->CopyResource(pTexture, pTemp);
    if (ppSRV)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
        srvd.Texture2D.MipLevels = dtd.MipLevels;
        srvd.Texture2D.MostDetailedMip = 0;
        srvd.Format = dtd.Format;
        srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        pDev->CreateShaderResourceView(pTexture, &srvd, ppSRV);
        printf("Generating %d mipmaps levels... ", dtd.MipLevels);
        fflush(stdout);
        pCtx->GenerateMips(*ppSRV);
        printf("done.\n");
        fflush(stdout);
    }
    SAFE_RELEASE(pTemp);
    printf("Load success.\n");
    return pTexture;
}