#include "stdafx.h"
#include "ring.h"
#include "mesh.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "dxutil.h"
#include "config.h"
#include "config_val.h"
#include "config_val_extern.h"
#define MOUSE_DRAG 0.025

extern int globe;
int W=0,H=0;
char txt[50]="";char txt1[20]="Scale:10";char txt2[50]="";
char txt3[]="Press F2 for help, F1 to hide this information";
char txt4[500]=
"Move:Arrow Keys\n"
"Rotate model:Left Mouse Key\n"
"Rotate light:Right Mouse Key\n"
"Change items:A,D\n"
"Set value:W,S\n"
"Hide Info:F2";
int help=0;
int showmsg=0;
float wscale=-3.0;
int col=0;
POINT pt={0,0};
int item=0;
int nvert,faces;
IDirect3DVertexDeclaration9* Decl=0;
IDirect3DDevice9* device=0;
DWORD BehaviorFlags = 0;
ID3DXEffect* effect=0;
IDirect3DVertexBuffer9* vb=0,*sqr=0;
IDirect3DIndexBuffer9* ib=0;
float scale=0,swell=0,red=0,glow=0;
float dscale=0,dswell=0,dred=0,dglow=0;
int autodemo=1,automode=1;
char availabletechnique[10]="";
int techlevel=0;
D3DXVECTOR4 lp(0,0,-10,0),lpt;
D3DXVECTOR4 ep(0,0,-10,1);
D3DXVECTOR4 screensize(0,0,0,0);
D3DXVECTOR2 anglea(0,0);
D3DXMATRIX view,proj,Rx,Ry,trans,world,step,worldview,lr,inv,spin,Rs;
D3DXVECTOR3 xt(1,0,0),yt(0,1,0),zt(0,0,1);
CD3DFont* font;
int keystate=0,hitstate=(1<<20);
const DWORD vertex::FVF=0;
bool mode=false,moder=false;
//device objects//
IDirect3DTexture9* accbuf[2];
IDirect3DSurface9* accsurf[2];
IDirect3DTexture9* mainbuf;
IDirect3DSurface9* mainsurf;
IDirect3DTexture9* texture_bk;
IDirect3DTexture9* texture_env;
IDirect3DTexture9* texture_script;
//////////////////
#define TYPE_INT   1
#define TYPE_FLOAT 2
#define TYPE_MAT   3
#define TYPE_VEC   4

bool PreInit(char* cfgfile)
{
	ConfigProfile profile;
	if(profile.LoadConfigFile(cfgfile==NULL?CFG_FILE_PATH:cfgfile)!=0)
		return false;
	config_val_container::get_val_container()->config_value(&profile);
	return true;
}

void SetUserValue(char *name,void* val,int type)
{
	D3DXHANDLE handle=effect->GetParameterByName(0,name);
	switch(type)
	{
	case TYPE_INT:
		effect->SetInt(handle, *(int*)val);
		break;
	case TYPE_FLOAT:
		effect->SetFloat(handle, *(float*)val);
		break;
	case TYPE_MAT:
		effect->SetMatrix(handle, (D3DXMATRIX*)val);
		break;
	case TYPE_VEC:
		effect->SetVector(handle, (D3DXVECTOR4*)val);
		break;
	}
}

bool Setup()
{
	W=HRes;H=VRes;
	screensize.x=screensize.z=W;
	screensize.y=screensize.w=H;
	HRESULT hr;
	int i;
	for(i=0;i<3;i++)
	{
		IDirect3DTexture9** tmp;
		IDirect3DSurface9** tmpsurf;
		if(i==2)
		{
			tmp=&mainbuf;
			tmpsurf=&mainsurf;
		}
		else
		{
			tmp=accbuf+i;
			tmpsurf=accsurf+i;
		}
		hr=D3DXCreateTexture(
			device,
			i==2?W:W/2,
			i==2?H:H/2,
			1,
			D3DUSAGE_RENDERTARGET,
			i==2?D3DFMT_A8R8G8B8:D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			tmp);
		if(FAILED(hr))
			return false;
		(*tmp)->GetSurfaceLevel(0,tmpsurf);
	}
	ID3DXBuffer* error=0;
#ifdef RESOURCE
	LPVOID pData;
	int nData;
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_FX1), _T("FX")))
		return false;
	hr=D3DXCreateEffect(device, pData, nData
		,NULL,NULL,0,NULL,&effect,&error);
#else
	hr=D3DXCreateEffectFromFile(device, TEXT("effect_dotr.txt"),
		NULL,NULL,0,NULL,&effect,&error);
#endif
	if(FAILED(hr)&&error!=NULL)
	{
		char* e=(char*)error->GetBufferPointer();
		MessageBoxA(0,e,"error",MB_OK);
		error->Release();
		return false;
	}
	LPCSTR texfile_bk,texfile_env,texfile_script;
	D3DXHANDLE handle = effect->GetParameterByName(0, "Bk");
	handle = effect->GetAnnotationByName(handle, "name");
	effect->GetString(handle, &texfile_bk);
	handle = effect->GetParameterByName(0, "Env");
	handle = effect->GetAnnotationByName(handle, "name");
	effect->GetString(handle, &texfile_env);
	handle = effect->GetParameterByName(0, "Script");
	handle = effect->GetAnnotationByName(handle, "name");
	effect->GetString(handle, &texfile_script);
#ifdef RESOURCE
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_JPEG1), _T("JPEG")))
		return false;	
	hr=D3DXCreateTextureFromFileInMemory(device,pData,nData,&texture_bk);
	if(FAILED(hr))
		return false;
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_JPEG1), _T("JPEG")))
		return false;	
	hr=D3DXCreateTextureFromFileInMemory(device,pData,nData,&texture_env);
	if(FAILED(hr))
		return false;
	if(!LoadResourceData(&pData, &nData, MAKEINTRESOURCE(IDR_JPEG1), _T("JPEG")))
		return false;	
	hr=D3DXCreateTextureFromFileInMemory(device,pData,nData,&texture_script);
	if(FAILED(hr))
		return false;
#else	
	hr=D3DXCreateTextureFromFile(device,texfile_bk,&texture_bk);
	if(FAILED(hr))
		return false;
	hr=D3DXCreateTextureFromFile(device,texfile_env,&texture_env);
	if(FAILED(hr))
		return false;
	hr=D3DXCreateTextureFromFile(device,texfile_script,&texture_script);
	if(FAILED(hr))
		return false;
#endif

	////////////////////////////////////
	hr=device->CreateVertexBuffer(
		6*sizeof(vertex),
		D3DUSAGE_WRITEONLY,
		vertex::FVF,
		D3DPOOL_MANAGED,
		&sqr,
		0);
	if(FAILED(hr))
	{
		MessageBox(0,"sqr failed",0,0);
		return false;
	}

	GetNumVert(&nvert, &faces);
	hr=device->CreateVertexBuffer(
		nvert*sizeof(vertex),
		D3DUSAGE_WRITEONLY,
		vertex::FVF,
		D3DPOOL_MANAGED,
		&vb,
		0);
	if(FAILED(hr))
	{
		MessageBox(0,"vb failed",0,0);
		return false;
	}
	hr=device->CreateIndexBuffer(
		faces*3*sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&ib,
		0);
	if(FAILED(hr))
	{
		MessageBox(0,"ib failed",0,0);
		return false;
	}
	vertex* vertices;
	DWORD* indices;
	vb->Lock(0,0,(void**)&vertices,0);
	ib->Lock(0,0,(void**)&indices,0);

	BuildMesh(vertices,indices);

	vb->Unlock();
	ib->Unlock();

	sqr->Lock(0,0,(void**)&vertices,0);
	vertices[0]=vertex(-1,-1,0,0,0,-1,0,-1,0,0,1);
	vertices[1]=vertex(-1, 1,0,0,0,-1,0,-1,0,0,0);
	vertices[2]=vertex( 1, 1,0,0,0,-1,0,-1,0,1,0);
	vertices[3]=vertex( 1, 1,0,0,0,-1,0,-1,0,1,0);
	vertices[4]=vertex( 1,-1,0,0,0,-1,0,-1,0,1,1);
	vertices[5]=vertex(-1,-1,0,0,0,-1,0,-1,0,0,1);
	sqr->Unlock();

	device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);//NONE
	device->SetRenderState(D3DRS_LIGHTING,false);
	device->SetRenderState(D3DRS_ZENABLE,true);

	D3DXMatrixIdentity(&spin);

	font=new CD3DFont("Times New Roman",16,0);
	font->InitDeviceObjects(device);
	font->RestoreDeviceObjects();
	D3DXMatrixRotationX(&world,-0.2);
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI/3,
		8./6,
		0.01f,
		1000.0f);
	D3DXMatrixIdentity(&lr);
	D3DXMatrixTranslation(&trans,0,0,10);
	view=trans;
	worldview=spin*world*view;
	D3DVERTEXELEMENT9 decl[]={
							 {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,
							 D3DDECLUSAGE_POSITION,0},
							 {0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,
							 D3DDECLUSAGE_NORMAL,0},
							 {0,24,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,
							 D3DDECLUSAGE_NORMAL,1},
							 {0,36,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,
							 D3DDECLUSAGE_TEXCOORD,0},
	                         D3DDECL_END()
							 };
	hr=device->CreateVertexDeclaration(decl,&Decl);
	if(FAILED(hr))
	{
		::MessageBox(0,"d3dx create VD failed",0,0);
		return false;
	}
	device->SetVertexDeclaration(Decl);
	for(i=0;;i++)
	{
		char n[10];
		sprintf(n,"T%d",i);
		D3DXHANDLE hTech=effect->GetTechniqueByName(n);
		if(hTech==0)break;
		if(FAILED(effect->ValidateTechnique(hTech)))break;
		techlevel=i;
		strcpy(availabletechnique,n);
	}
	if(availabletechnique[0]==0)
		return false;
	srand(time(NULL));
	return true;
}

void OnSize(int width,int height)
{
	screensize.x=width;
	screensize.y=height;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI/3,
		(float)width/height,
		0.01f,
		1000.0f);
}

void Cleanup()
{
	SAFE_RELEASE(Decl);
	SAFE_RELEASE(effect);
	SAFE_RELEASE(vb);
	SAFE_RELEASE(sqr);
	SAFE_RELEASE(ib);
	if(font)
		font->DeleteDeviceObjects();
	for(int i=0;i<2;i++)
	{
		SAFE_RELEASE(accbuf[i]);
		SAFE_RELEASE(accsurf[i]);
	}
	SAFE_RELEASE(mainbuf);
	SAFE_RELEASE(mainsurf);
	SAFE_RELEASE(texture_bk);
	SAFE_RELEASE(texture_env);
	SAFE_RELEASE(texture_script);
	SAFE_RELEASE(device);
}
void ProcessMouseMove(float tdelta);
bool FrameMove(float tdelta,float rtc)
{
	static float tcount=0,fcount=0;
	tcount+=rtc;fcount++;
	if(tcount>=1.)
	{
		char txtwnd[30]="",*txtvp;
		switch(BehaviorFlags)
		{
		case D3DCREATE_HARDWARE_VERTEXPROCESSING:
			txtvp="(hw vp)";
			break;
		case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
			txtvp="(sw vp)";
			break;
		default:
			txtvp="";
			break;
		}
		if(Windowed)
			sprintf(txtwnd,"(Wnd:%d*%d)",HResF,VResF);
		sprintf(txt,"%d*%d%s,%dfps%s",HRes,VRes,txtwnd,(int)fcount,txtvp);
		tcount=0,fcount=0;
	}
	switch(item)
	{
	case 0:sprintf(txt1,"AutoMode");sprintf(txt2,autodemo!=0?"ON":"OFF");break;
	case 1:sprintf(txt1,"Glow");sprintf(txt2,"%d",(int)(100*glow));break;
	case 2:sprintf(txt1,"Red");sprintf(txt2,"%d",(int)(100*red));break;
	case 3:sprintf(txt1,"Scale");sprintf(txt2,"%d",(int)(100*scale));break;
	case 4:sprintf(txt1,"Swell");sprintf(txt2,"%d",(int)(1000*swell));break;
	}
	
	scale+=(dscale-scale)*tdelta;
	if(autodemo==0)swell+=(dswell-swell)*tdelta;
	red+=(dred-red)*tdelta;
	glow+=(dglow-glow)*tdelta;
	if(autodemo!=0)Differ(tdelta);

	ProcessMouseMove(tdelta);
	D3DXMatrixIdentity(&step);
	if(keystate&(1<<3))
	{
		D3DXMatrixTranslation(&step,0,0,-0.5*tdelta);
	}
	if(keystate&(1<<4))
	{
		D3DXMatrixTranslation(&step,0,0, 0.5*tdelta);
	}
	if(keystate&(1<<1))
	{
		D3DXMatrixTranslation(&step,0.5*tdelta,0,0 );
	}
	if(keystate&(1<<2))
	{
		D3DXMatrixTranslation(&step,-0.5*tdelta,0,0 );
	}
	if(keystate&(1<<7))
	{
		D3DXMatrixTranslation(&step,0,-0.5*tdelta,0);
	}
	if(keystate&(1<<8))
	{
		D3DXMatrixTranslation(&step,0,0.5*tdelta,0);
	}
	if(keystate&(1<<5))
	{
	}
	if(keystate&(1<<6))
	{
	}
	if(keystate&(1<<15))
	{
		switch(item)
		{
		case 0:if(autodemo==0)autodemo=1;break;
		case 1:if(autodemo==0){dglow+=0.2*tdelta;glow=dglow;}break;
		case 2:if(autodemo==0){dred+=0.2*tdelta;red=dred;}break;
		case 3:if(autodemo==0){dscale+=0.2*tdelta;scale=dscale;}break;
		case 4:if(autodemo==0){dswell+=0.2*tdelta;swell=dswell;}break;
		}
	}
	if(keystate&(1<<16))
	{
		switch(item)
		{
		case 0:if(autodemo!=0)autodemo=0;break;
		case 1:if(autodemo==0){dglow-=0.2*tdelta;glow=dglow;}break;
		case 2:if(autodemo==0){dred-=0.2*tdelta;red=dred;}break;
		case 3:if(autodemo==0){dscale-=0.2*tdelta;scale=dscale;}break;
		case 4:if(autodemo==0){dswell-=0.2*tdelta;swell=dswell;}break;
		}
	}

	if(keystate)//&((1<<3)|(1<<4))
	{
		trans=trans*step;
	}
	view=trans;
	D3DXVec4Transform(&lpt,&lp,&lr);
	D3DXMatrixRotationY(&Ry,-tdelta*0.5);
	if(!mode)spin*=Ry;
	worldview=spin*world*view;
	return true;
}

void InitPassNum(int npass,int *ipass, char** pname, int nname)
{
	D3DXHANDLE handle;
	handle=effect->GetTechniqueByName(availabletechnique);
	for(int j=0;j<(int)npass;j++)
	{
		D3DXHANDLE phandle=effect->GetPass(handle, j);
		D3DXPASS_DESC desc;
		effect->GetPassDesc(phandle, &desc);
		for(int i=0;i<nname;i++)
		{
			if(strcmp(desc.Name,pname[i])==0)
			{
				ipass[i]=j;
				break;
			}
		}
	}
}

bool Display(float tdelta)
{
	if(!device)
		return false;
	static int tindex=0;
	static bool binit=true;
	D3DXHANDLE handle;
	static int ipass[5]={-1,-1,-1,-1,-1};
	UINT npass=0;

	handle=effect->GetTechniqueByName(availabletechnique);
	effect->SetTechnique(handle);
	effect->Begin(&npass, 0);

	if(binit)
	{
		char* pname[5]={"P0","P1","P2","P1_0","P1_1"};
		device->SetRenderTarget(0,accsurf[0]);
		device->Clear(0,0,D3DCLEAR_TARGET,
			0x00000000,0.0f,0);
		InitPassNum(npass,ipass,pname,5);
		binit=false;
	}
	if(ipass[0]<0||ipass[2]<0||(ipass[1]<0&&(ipass[3]<0||ipass[4]<0)))
	{
		effect->End();
		return false;
	}

	SetUserValue("worldview",(void*)&worldview,TYPE_MAT);
	SetUserValue("proj",(void*)&proj,TYPE_MAT);
	SetUserValue("lp",(void*)&lpt,TYPE_VEC);
	SetUserValue("red",(void*)&red,TYPE_FLOAT);
	SetUserValue("swell",(void*)&swell,TYPE_FLOAT);
	SetUserValue("scale",(void*)&scale,TYPE_FLOAT);
	SetUserValue("glow",(void*)&glow,TYPE_FLOAT);
	SetUserValue("screensize",(void*)&screensize,TYPE_VEC);
	if(techlevel==1)
		SetUserValue("index",&tindex,TYPE_INT);

	//Phase 1
	{
		device->SetRenderTarget(0,mainsurf);

		handle=effect->GetParameterByName(0,"Tex");
		effect->SetTexture(handle, NULL);
		handle=effect->GetParameterByName(0,"Mask");
		effect->SetTexture(handle, NULL);
		handle=effect->GetParameterByName(0,"Bk");
		effect->SetTexture(handle, texture_bk);
		handle=effect->GetParameterByName(0,"Env");
		effect->SetTexture(handle, texture_env);
		handle=effect->GetParameterByName(0,"Script");
		effect->SetTexture(handle, texture_script);

		device->SetRenderState(D3DRS_ZENABLE,true);
		device->SetRenderState(D3DRS_ZWRITEENABLE,true);
		device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
			0x00000000,100.0f,0);
		device->BeginScene();
		effect->BeginPass(ipass[0]);
		device->SetStreamSource(0,vb,0,sizeof(vertex));
		device->SetIndices(ib);
		device->SetVertexDeclaration(Decl);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,nvert,0,faces);
		effect->EndPass();
		device->EndScene();
	}
	//Phase 2
	{
		device->SetRenderTarget(0,accsurf[1-tindex]);

		handle=effect->GetParameterByName(0,"Tex");
		effect->SetTexture(handle,accbuf[tindex]);
		handle=effect->GetParameterByName(0,"Mask");
		effect->SetTexture(handle,mainbuf);
		handle=effect->GetParameterByName(0,"Bk");
		effect->SetTexture(handle,texture_bk);
		handle=effect->GetParameterByName(0,"Env");
		effect->SetTexture(handle,texture_env);
		handle=effect->GetParameterByName(0,"Script");
		effect->SetTexture(handle,texture_script);

		device->SetRenderState(D3DRS_ZENABLE,false);
		device->SetRenderState(D3DRS_ZWRITEENABLE,false);
		device->Clear(0,0,D3DCLEAR_TARGET,
			0x00000000,100.0f,0);
		device->BeginScene();
		if(ipass[1]>=0)
			effect->BeginPass(ipass[1]);
		else if(ipass[3]>=0&&ipass[4]>=0)
			effect->BeginPass(tindex?ipass[4]:ipass[3]);
		device->SetStreamSource(0,sqr,0,sizeof(vertex));
		device->SetVertexDeclaration(Decl);
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
		effect->EndPass();
		device->EndScene();
	}
	//Phase 3
	{
		IDirect3DSurface9 *pBackBuffer;
		device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
		device->SetRenderTarget(0,pBackBuffer);
		pBackBuffer->Release();

		handle=effect->GetParameterByName(0,"Tex");
		if(techlevel==0)
			effect->SetTexture(handle,accbuf[1-tindex]);
		else if(techlevel==1)
			effect->SetTexture(handle,accbuf[0]);
		handle=effect->GetParameterByName(0,"Mask");
		effect->SetTexture(handle,mainbuf);
		handle=effect->GetParameterByName(0,"Bk");
		effect->SetTexture(handle,texture_bk);
		handle=effect->GetParameterByName(0,"Env");
		effect->SetTexture(handle,texture_env);
		handle=effect->GetParameterByName(0,"Script");
		effect->SetTexture(handle,texture_script);

		device->SetRenderState(D3DRS_ZENABLE,false);
		device->SetRenderState(D3DRS_ZWRITEENABLE,false);
		device->Clear(0,0,D3DCLEAR_TARGET,
			0x00000000,100.0f,0);
		device->BeginScene();
		effect->BeginPass(ipass[2]);
		device->SetStreamSource(0,sqr,0,sizeof(vertex));
		device->SetVertexDeclaration(Decl);
		device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
		effect->EndPass();
		if(showmsg)
		{
			font->DrawText(0,0,0xffff0000,txt);
			font->DrawText(0,30,0xffff0000,txt1);
			font->DrawText(0,60,0xffff0000,txt2);
			if(help==0)font->DrawText(0,90,0xffff0000,txt3);
			else font->DrawText(0,90,0xffff0000,txt4);
		}
		device->EndScene();
	}
	effect->End();

	tindex=1-tindex;

	return true;
}
bool Present()
{
	device->Present(0,0,0,0);//MessageBox(0,"vb failed",0,0);
	return true;
}

void onmdown(HWND hwnd)
{
	mode=true;
	::GetCursorPos(&pt);
	::SetCapture(hwnd);
}

void onmove(HWND hwnd)
{
	POINT ptlast=pt;
	::GetCursorPos(&pt);
	float X=pt.x-ptlast.x;
	float Y=pt.y-ptlast.y;
	anglea.x+=(float)X/100;
	anglea.y+=(float)Y/100;
}
void ProcessMouseMove(float tdelta)
{
	const float inv_dim_time=1.0/MOUSE_DRAG;
	float acct=1-exp(-tdelta*inv_dim_time);
	if(acct>1)acct=1;
	D3DXVECTOR2 acc=anglea*acct;
	anglea-=acc;

	D3DXMatrixRotationX(&Rx,-acc.y);
	D3DXMatrixRotationY(&Ry,-acc.x);
	if(mode)
	{
		if(keystate&(1<<18))
		{
			D3DXVECTOR3 pos(0,0,0),tp;
			D3DXVec3TransformCoord(&tp,&pos,&trans);
			D3DXMatrixTranslation(&step,acc.x/10*tp.z,-acc.y/10*tp.z,0);
			trans*=step;
		}
		else
			world=world*Rx*Ry;
	}
	if(moder)
	{
		lr=lr*Rx*Ry;
	}
}

void onmup(HWND hwnd)
{
	mode=false;
	::ReleaseCapture();
}

void onkey(int key,bool down,int type)
{
	if(down)
	{
		keystate|=(1<<key);
	}
	else
	{
		keystate&=(~(1<<key));
	}
	if(keystate&(1<<13))item=(item+4)%5;
	if(keystate&(1<<14))item=(item+1)%5;
	if(keystate&(1<<17))help=!help;
	if(keystate&(1<<19))showmsg=!showmsg;
}

void onrdown(HWND hwnd)
{
	moder=true;
	::GetCursorPos(&pt);
	::SetCapture(hwnd);
}

void onrup(HWND hwnd)
{
	moder=false;
	::ReleaseCapture();
}

void Differ(float tdelta)
{
	static float t=0;
	static float start=0;
	if(t>=start)
	{
		if(autodemo==1)
		{
			autodemo=2;start=3;
			dswell=dglow=dred=dscale=0;
		}
		else if(autodemo==2)
		{
			autodemo=3;
			start=2;
			dscale=1;
		//	dglow=1;
		//	dred=1;
		}
		else
		{
			start=1;
			dglow=dred=dscale=0.2+0.8*(float)rand()/RAND_MAX;
		}
		t=0;
	}
	if(autodemo==2)
	{
		dswell=swell=0.1*t/3;
	}
	t+=tdelta;
}