#include "stdafx.h"
#include<windows.h>
#include<math.h>
#include "ring.h"
#include <mmsystem.h>
#include "resource.h"
#include "config_val_extern.h"
#define MAKEDWORD(x,y) ((x)|((y)<<16))
int globe=0;
int HRes=0,VRes=0;
int HResF=0;
int VResF=0;
int RefreshRate=0;
DEFINE_BOOL_VAL(Windowed,false);
DEFINE_FLOAT_VAL(DTClamp_high,1.1f);
DEFINE_FLOAT_VAL(DTClamp_low,0.9f);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
static inline double get_performance_greq()
{
	LARGE_INTEGER bigint;
	QueryPerformanceFrequency(&bigint);
	return (double)bigint.QuadPart;
}
static inline double get_performance_cnt()
{
	LARGE_INTEGER bigint;
	QueryPerformanceCounter(&bigint);
	return (double)bigint.QuadPart;
}
static inline void clamp_tdelta(float* tdelta,float inv_rfrshrt)
{
	if(*tdelta>DTClamp_high*inv_rfrshrt)
		*tdelta=DTClamp_high*inv_rfrshrt;
	if(*tdelta<DTClamp_low*inv_rfrshrt)
		*tdelta=DTClamp_low*inv_rfrshrt;
}
static bool parse_cmdline(char** cfg,char* buf,uint buflen)
{
	char* default_cfg="config.cfg";
	LPSTR cmdline=GetCommandLine();
	char *op_tag="--cfgfile=";
	char* cfgstart=strstr(cmdline,op_tag);
	if(cfgstart!=NULL)
	{
		cfgstart+=strlen(op_tag);
		char* cfgend=strchr(cfgstart,' ');
		if(cfgend!=NULL)
		{
			uint len=cfgend-cfgstart;
			if(len>=buflen)
				return false;
			memcpy(buf,cfgstart,len);
			buf[len]=0;
			*cfg=buf;
		}
		else
			*cfg=cfgstart;
	}
	else
		*cfg=default_cfg;
	return true;
}
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE PreInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	const uint buflen=50;
	char buf[buflen],*cfg;
	if(!parse_cmdline(&cfg,buf,buflen))
	{
		MessageBox(0,"invalid command line","error",MB_OK);
		return -1;
	}

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	HWND hwnd;
	MSG msg;
	char lpszClassName[]="DotrWin";//
	WNDCLASS wc;
	wc.style=0;//CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON_256));
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=lpszClassName;

	RegisterClass(&wc);

	HResF=GetSystemMetrics(SM_CXFULLSCREEN);
	VResF=GetSystemMetrics(SM_CYFULLSCREEN);
	HRes=GetSystemMetrics(SM_CXSCREEN);
	VRes=GetSystemMetrics(SM_CYSCREEN);

	hwnd=CreateWindow(lpszClassName,
		              "Lord of the rings",
					  WS_OVERLAPPEDWINDOW,
					  0,0,HResF,VResF,
					  NULL,
					  NULL,
					  hInstance,
					  NULL);

	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);

	if(!PreInit(cfg))
	{
		MessageBox(0,"pre-init failed",0,0);
		return -1;
	}
	if(!InitD3D(HRes,VRes,RefreshRate,hwnd,Windowed))
	{
		MessageBox(0,"init failed",0,0);
		return 0;
	}
	if(!Setup())
	{
		MessageBox(0,"setup failed",0,0);
		return 0;
	}
	SendMessage(hwnd,WM_SIZE,0,MAKEDWORD(HRes,VRes));
	ZeroMemory(&msg,sizeof(MSG));
	double freq=get_performance_greq();
	double inv_freq=1.0f/freq;
	float inv_rfrshrate=1.0f/RefreshRate;
	while(msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static float atdelta=0;
			double cTime=get_performance_cnt();
			static double lTime=cTime;
			float tdelta=(cTime-lTime)*inv_freq;
			atdelta=tdelta*0.01f+atdelta*0.99f;
			clamp_tdelta(&atdelta,inv_rfrshrate);
			FrameMove(atdelta,tdelta);
			Display(atdelta);
			Present();
			lTime=cTime;
		}
	}
	Cleanup();
	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam)
{
	int k;
	switch(message)
	{
	case WM_LBUTTONDOWN: onmdown(hwnd);break;
	case WM_LBUTTONUP:   onmup(hwnd);break;
	case WM_RBUTTONDOWN: onrdown(hwnd);break;
	case WM_RBUTTONUP: onrup(hwnd);break;
	case WM_MOUSEMOVE: onmove(hwnd);break;
	case WM_KEYDOWN:
		if(wParam==VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			break;
		}
		switch(wParam)
		{
		case  VK_LEFT:k=1;break;
		case      'A':k=13;break;
		case VK_RIGHT:k=2;break;
		case      'D':k=14;break;
		case    VK_UP:k=3;break;
		case      'W':k=15;break;
		case  VK_DOWN:k=4;break;
		case      'S':k=16;break;
		case      'Q':k=5;break;
		case      'E':k=6;break;
		case      'Z':k=7;break;
		case      'X':k=8;break;
		case      'K':k=9;break;
		case      'L':k=10;break;
		case    VK_F2:k=17;break;
		case VK_SPACE:
		case VK_RCONTROL:k=18;break;
		case    VK_F1:k=19;break;
		default      :k=0;break;  
		}
		onkey(k,true,0);
		break;
	case WM_KEYUP:		
		if(wParam==VK_ESCAPE)
		{
			DestroyWindow(hwnd);
			break;
		}
		switch(wParam)
		{
		case VK_ESCAPE:DestroyWindow(hwnd);break;
		case  VK_LEFT:k=1;break;
		case      'A':k=13;break;
		case VK_RIGHT:k=2;break;
		case      'D':k=14;break;
		case    VK_UP:k=3;break;
		case      'W':k=15;break;
		case  VK_DOWN:k=4;break;
		case      'S':k=16;break;
		case      'Q':k=5;break;
		case      'E':k=6;break;
		case      'Z':k=7;break;
		case      'X':k=8;break;
		case      'K':k=9;break;
		case      'L':k=10;break;
		case    VK_F2:k=17;break;
		case VK_SPACE:
		case VK_RCONTROL:k=18;break;
		case    VK_F1:k=19;break;
		default      :k=0;break;  
		}
		onkey(k,false,0);
		break;
	case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			OnSize(rc.right-rc.left,rc.bottom-rc.top);
		}
		break;
	case WM_DESTROY: PostQuitMessage(0);break;
	default: return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}



