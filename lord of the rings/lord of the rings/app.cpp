#include "stdafx.h"
#include "app.h"
#include <math.h>
#include <stdio.h>

extern int globe;
int W=0,H=0;
float tcon=0,fcon=0;
char txt[50]="Frame rate:0fps";
char txt1[20]="Scale:10";
char txt2[50]="";
char txt3[20]="PhysMode:OFF";
float wscale=-3.0;
int col=0;
POINT pt={0,0};
IDirect3DVertexDeclaration9* Decl=0;
IDirect3DDevice9* device=0;
IDirect3DVertexBuffer9* vb=0;
IDirect3DTexture9* tread=0;
ID3DXEffect* effect=0;
IDirect3DTexture9* texbuf0=0,*texbuf1=0;
IDirect3DSurface9* surfbuf0=0,*surfbuf1=0;
int tindex=0;
IDirect3DTexture9** texbuf[2]={&texbuf0, &texbuf1};
IDirect3DSurface9** surfbuf[2]={&surfbuf0, &surfbuf1};
float scale=1;
D3DXVECTOR4 lp(0,0,-10,1);
D3DXVECTOR4 ep(0,0,-10,1);
D3DXMATRIX view,proj,Rx,Ry,trans,world,step,worldview,lr;
D3DXVECTOR3 xt(1,0,0),yt(0,1,0),zt(0,0,1);
D3DXVECTOR4 screensize(0,0,Width,Height);
D3DVIEWPORT9 vp={0,0,1024,800,0,1};//1024/8,800/8
CD3DFont* font;
bool boutput=false;
int keystate=0,hitstate=(1<<20);
struct vertex
{
	float _x,_y,_z;
	float _nx,_ny,_nz;
	float _u,_v;
	static const DWORD FVF;
	vertex(){}
	vertex(float x,float y,float z,float nx,float ny,float nz,float u,float v)
	{_x=x;_y=y;_z=z;_nx=nx;_ny=ny;_nz=nz;_u=u;_v=v;}
	vertex(D3DXVECTOR3 pos,D3DXVECTOR3 norm,D3DXVECTOR2 tex)
	{_x=pos.x;_y=pos.y;_z=pos.z;_nx=norm.x;_ny=norm.y;_nz=norm.z;_u=tex.x;_v=tex.y;}
};
const DWORD vertex::FVF=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
bool mode=false;
bool Setup()
{
	HRESULT hr;
	hr=device->CreateVertexBuffer(
		6*sizeof(vertex),
		D3DUSAGE_WRITEONLY,
		vertex::FVF,
		D3DPOOL_MANAGED,
		&vb,
		0);
	if(FAILED(hr))
		return false;

	vertex* vertices;
	vb->Lock(0,0,(void**)&vertices,0);
	vertices[0]=vertex(-1,-1,0,0,0,-1,0,1);
	vertices[1]=vertex(-1, 1,0,0,0,-1,0,0);
	vertices[2]=vertex( 1, 1,0,0,0,-1,1,0);
	vertices[3]=vertex( 1, 1,0,0,0,-1,1,0);
	vertices[4]=vertex( 1,-1,0,0,0,-1,1,1);
	vertices[5]=vertex(-1,-1,0,0,0,-1,0,1);
	vb->Unlock();

	ID3DXBuffer* error=0;
#ifdef RESOURCE
	LPVOID pData;
	int nData;
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_FX1), _T("FX")))
		return false;
	hr=D3DXCreateEffect(device, pData, nData
		,NULL,NULL,0,NULL,&effect,&error);
#else
	hr=D3DXCreateEffectFromFile(device, TEXT("effect_lotr.fx"),
		NULL,NULL,0,NULL,&effect,&error);
#endif
	if(FAILED(hr))
	{
		char* e=(char*)error->GetBufferPointer();
		MessageBoxA(0,e,"error",MB_OK);
		error->Release();
		return false;
	}
	D3DXHANDLE handle = effect->GetParameterByName(0, "Mask");
	handle = effect->GetAnnotationByName(handle, "name");
	LPCSTR texfile;
	effect->GetString(handle, &texfile);
#ifdef RESOURCE
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_JPEG1), _T("JPEG")))
		return false;	
	hr=D3DXCreateTextureFromFileInMemoryEx(device,pData,nData,D3DX_DEFAULT,D3DX_DEFAULT,
		1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,
		0,NULL,NULL,&tread);
#else	
	hr=D3DXCreateTextureFromFileEx(device,texfile,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
		1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,
		0,NULL,NULL,&tread);	
#endif
	if(FAILED(hr))
		return false;
	D3DSURFACE_DESC desc;
	tread->GetLevelDesc(0,&desc);
	screensize.x=W=desc.Width;
	screensize.y=H=desc.Height;
	IDirect3DTexture9** texbuf[2]={&texbuf0, &texbuf1};
	IDirect3DSurface9** surfbuf[2]={&surfbuf0, &surfbuf1};
	for(int i=0;i<2;i++)
	{
		hr=D3DXCreateTexture(
			device,
			W,
			H,
			1,
			D3DUSAGE_RENDERTARGET,//|D3DUSAGE_DYNAMIC
			D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			texbuf[i]);
		if(FAILED(hr))
			return false;
		(*texbuf[i])->GetSurfaceLevel(0, surfbuf[i]);
	}
	handle = effect->GetParameterByName(0,"Mask");
	effect->SetTexture(handle, tread);

	device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);//CCW
	device->SetRenderState(D3DRS_LIGHTING,false);
	device->SetRenderState(D3DRS_ZENABLE,false);
	device->SetRenderState(D3DRS_ZWRITEENABLE,false);

	//device->SetViewport(&vp);//注意这条函数调用会将render target设回主交换链

	return true;
}

void Cleanup()
{
	SAFE_RELEASE(device);
	SAFE_RELEASE(vb);
	SAFE_RELEASE(effect);
	SAFE_RELEASE(texbuf0);
	SAFE_RELEASE(texbuf1);
	SAFE_RELEASE(surfbuf0);
	SAFE_RELEASE(surfbuf1);
}

bool Display(float tdelta)
{
	if(!device)
		return false;
	static bool binit=true;
	if(binit)
	{
		device->SetRenderTarget(0,surfbuf0);
		device->Clear(0,0,D3DCLEAR_TARGET,
			0x00000000,0.0f,0);
		binit=false;
	}

	D3DXHANDLE handle=effect->GetParameterByName(0, "screensize");
	effect->SetVector(handle, &screensize);

	device->SetRenderTarget(0,*surfbuf[1-tindex]);

	handle=effect->GetParameterByName(0,"Tex");
	effect->SetTexture(handle, *texbuf[tindex]);

	device->Clear(0,0,D3DCLEAR_TARGET,
		0x00000000,0.0f,0);

	handle=effect->GetTechniqueByName("T0");
	effect->SetTechnique(handle);

	UINT npass=0;
	effect->Begin(&npass, 0);
	int ipass[3]={0,0,0};
	char* pname[3]={"P0","P1","P2"};
	for(int j=0;j<(int)npass;j++)
	{
		D3DXHANDLE phandle=effect->GetPass(handle, j);
		D3DXPASS_DESC desc;
		effect->GetPassDesc(phandle, &desc);
		for(int i=0;i<3;i++)
		{
			if(strcmp(desc.Name,pname[i])==0)
				ipass[i]=j;
		}
	}
	device->BeginScene();
	{
		effect->BeginPass(boutput?ipass[1]:ipass[0]);
		device->SetFVF(vertex::FVF);
		device->SetStreamSource(0,vb,0,sizeof(vertex));
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
		effect->EndPass();
	}
	device->EndScene();

	handle=effect->GetParameterByName(0,"Tex");
	effect->SetTexture(handle, *texbuf[1-tindex]);

	IDirect3DSurface9 *pBackBuffer;
	device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	device->SetRenderTarget(0,pBackBuffer);
	pBackBuffer->Release();
	device->Clear(0,0,D3DCLEAR_TARGET,
		0x00000000,0.0f,0);

	device->BeginScene();
	{
		effect->BeginPass(ipass[2]);
		device->SetFVF(vertex::FVF);
		device->SetStreamSource(0,vb,0,sizeof(vertex));
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
		effect->EndPass();
	}
	device->EndScene();
	effect->End();

	device->Present(0,0,0,0);

	static float tcount=0,fcount=0;
	tcount+=tdelta;fcount++;
	if(tcount>=1.)
	{
		sprintf(txt,"Frame rate:%dfps",(int)fcount);
		tcount=0,fcount=0;
	}
	if(!boutput)
		tindex=1-tindex;

	return true;
}

void SaveOutput()
{
	IDirect3DTexture9* save=NULL;
	IDirect3DSurface9* surf=NULL;
	do
	{
		HRESULT hr=D3DXCreateTexture(
			device,
			W,
			H,
			1,
			0,
			D3DFMT_A32B32G32R32F,
			D3DPOOL_SYSTEMMEM,
			&save);
		if(FAILED(hr))
			break;
		save->GetSurfaceLevel(0,&surf);
		hr=device->GetRenderTargetData(*surfbuf[1-tindex],surf);
		if(FAILED(hr))
			break;
		D3DXSaveTextureToFileA("script1.dds", D3DXIFF_DDS, save, NULL);
	}while(false);
	SAFE_RELEASE(surf);
	SAFE_RELEASE(save);
}

void onmdown(HWND hwnd)
{
}
void onmove(HWND hwnd)
{
}

void onmup(HWND hwnd)
{
}

void onkey(int key,int keycode,bool down,int type)
{
	if(down)
	{
		keystate|=(1<<key);
		switch(keycode)
		{
		case 'M':
			boutput=true;
			break;
		case 'O':
			SaveOutput();
			break;
		}
	}
	else
	{
		keystate&=(~(1<<key));
	}
}

void onrdown(HWND hwnd)
{
}

void onrup(HWND hwnd)
{
}

void OnSize(int width,int height)
{
	screensize.z=width;
	screensize.w=height;
}
