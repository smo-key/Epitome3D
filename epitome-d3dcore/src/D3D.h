#ifndef EPITOME_D3D
#define EPITOME_D3D

//linking
#include "globals.h"
#include "D3Ddesc.h"

namespace EPITOME
{
	class D3D
	{
	public:
		D3D();
		D3D(const D3D&);
		~D3D();

		bool Initialize(int screenWidth, int screenHeight, bool vsync,
			HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
		void Shutdown();
	
		void BeginScene(float, float, float, float);
		void EndScene();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void GetProjectionMatrix(D3DXMATRIX&);
		void GetWorldMatrix(D3DXMATRIX&);
		void GetOrthoMatrix(D3DXMATRIX&);

		void GetVideoCardInfo(char*, int&);

		void TurnZBufferOn();
		void TurnZBufferOff();

		void SetRasterizer(D3DDesc::Rasterizer raster);
		D3DDesc::Rasterizer GetRasterizer() { return _raster; }

		ID3D11DepthStencilView* GetDepthStencilView();
		void SetBackBufferRenderTarget();

	private:
		D3DDesc::Rasterizer _raster; //last rasterizer state

		bool m_vsync_enabled;
		int m_videoCardMemory;
		char m_videoCardDescription[128];

		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RasterizerState* m_rasterState;

		D3DXMATRIX m_projectionMatrix;
		D3DXMATRIX m_worldMatrix;
		D3DXMATRIX m_orthoMatrix;

		ID3D11DepthStencilState* m_depthDisabledStencilState;
		D3D11_TEXTURE2D_DESC m_depthBufferDesc;
		ID3D11BlendState* m_alphaEnableBlendingState;
		ID3D11BlendState* m_alphaDisableBlendingState;
		D3D11_RASTERIZER_DESC m_rasterDesc;
	};
}

#endif