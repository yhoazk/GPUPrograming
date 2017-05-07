#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <Windows.h>
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(X) if((X)) { (X)->Release(); (X)=NULL;}
#endif
class CDXManager
{
protected:
	ID3D11Device* m_pDevice; //La abstracción de dispositivo. (Factory)
	ID3D11DeviceContext* m_pContext; //Contexto de dispositivo. (Context)
	IDXGISwapChain*      m_pSwapChain; //Cadena de intercambio. (Front-Back Buffers)
public:
	IDXGIAdapter* ChooseDevice(HWND hWnd);
	bool Initialize(IDXGIAdapter* pAdapter = NULL, bool bUseCPU = false,HWND hWnd=0);
	ID3D11ComputeShader* CompileComputeShader(wchar_t* pszFileName,
		char* pszEntryPoint);
	void Uninitialize();
	ID3D11Device* GetDevice() { return m_pDevice; };
	ID3D11DeviceContext* GetContext() { return m_pContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	//CPU->GPU
	ID3D11Buffer* LoadBuffer(
		void* pInData,
		unsigned long ulElementSize,
		unsigned long ulElements);
	//GPU->CPU
	void StoreBuffer(ID3D11Buffer* pISB,
		unsigned long ulElementSize,
		unsigned long ulElements,
		void* pOutData);
	ID3D11Buffer* CreateConstantBuffer(unsigned long ulBytes);
	void UpdateConstantBuffer(ID3D11Buffer* pCB, void* pSource, unsigned long ulBytes);
	ID3D11Texture2D* CDXManager::LoadTexture(
		char* pszFileName,		//The file name
		unsigned long MipMapLevels, //Number of mipmaps to generate, -1 automatic (int)log2(min(sizeX,sizeY))+1 levels
		ID3D11ShaderResourceView** ppSRV, //The Shader Resource View
		float(*pAlphaFunction)(float r, float g, float b)//Operates pixel's rgb channels to build an alpha channel (normalized), can be null
		);
	//Cambia el tamaño de la cadena de intercambio a los valores especificados.
	void Resize(int cx, int cy)
	{
		if (m_pSwapChain) m_pSwapChain->ResizeBuffers(2, cx, cy,
			DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	}
	CDXManager();
	~CDXManager();
};

