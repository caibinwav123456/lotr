#include "stdafx.h"
#include<math.h>
#include "app.h"
#include <mmsystem.h>
#pragma warning(disable:4065)
int globe=0;
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE PreInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	char lpszClassName[]="LotrWin";//
	WNDCLASS wc;
	wc.style=0;//CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=lpszClassName;

	RegisterClass(&wc);

	HDC dc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	Width=GetDeviceCaps(dc, HORZRES);
	Height=GetDeviceCaps(dc, VERTRES);
	DeleteDC(dc);
	
	int w=Width,h=Height;

	hwnd=CreateWindow(lpszClassName,
		              "Lord of the rings",
					  WS_OVERLAPPEDWINDOW,
					  (Width-w)/2,(Height-h)/2,w,h,//200,100,640,480,
					  NULL,
					  NULL,
					  hInstance,
					  NULL);

	ShowWindow(hwnd,SW_MAXIMIZE);
	UpdateWindow(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	Width=rect.right-rect.left;
	Height=rect.bottom-rect.top;
	if(!InitD3D(Width,Height,hwnd))
	{
		MessageBox(0,"init failed",0,0);
		return 0;
	}
	if(!Setup())
	{
		MessageBox(0,"setup failed",0,0);
		return 0;
	}
	float lTime=(float)timeGetTime();
	ZeroMemory(&msg,sizeof(MSG));
	while(msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
		else 
		{
			float cTime=(float)timeGetTime();
			float tdelta=(cTime-lTime)*0.001f;
			if(globe==1){lTime=cTime;continue;}
			Display(tdelta);
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
	case WM_KEYDOWN: //if(wParam==VK_ESCAPE)::DestroyWindow(hwnd);
		switch(wParam)
		{
		default      :k=0;break;  
		}
		onkey(k,wParam,true,0);
		break;
	case WM_KEYUP:		
		switch(wParam)
		{
		default      :k=0;break;  
		}
		onkey(k,wParam,false,0);
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



