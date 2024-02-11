// stdafx.cpp : source file that includes just the standard includes
//	wer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "resource.h"
bool InitD3D(int w,int h,int& rfrhrate,HWND hwnd,bool windowed)
{
	IDirect3D9* d3d9;
	d3d9=Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d9)return 0;

	D3DDISPLAYMODE dispMode;
	d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dispMode);
	rfrhrate=dispMode.RefreshRate;

	D3DCAPS9 caps;
	DWORD dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	if(caps.VertexShaderVersion<D3DVS_VERSION(2,0)||
	   caps.PixelShaderVersion <D3DPS_VERSION(2,0))
    {
		char descend[MAX_PATH],error[MAX_PATH];
		LoadString(NULL, IDS_STR_DECLINE, descend, MAX_PATH);
		LoadString(NULL, IDS_STR_ERROR, error, MAX_PATH);
		MessageBox(hwnd,descend,error,MB_OK);
		return 0;
	}
	BehaviorFlags = dwBehaviorFlags;

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth=w;
	d3dpp.BackBufferHeight=h;
	d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount=1;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality=0;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hwnd;
	d3dpp.Windowed=windowed;
	d3dpp.EnableAutoDepthStencil=true;
	d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;
	d3dpp.Flags=0;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE;

	//IDirect3DDevice9* device=0;
	HRESULT hr=d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		dwBehaviorFlags,//D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&device);
	if(FAILED(hr))
	{
		MessageBox(0,"device failed",0,0);
		return false;
	}
	return true;
}

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
