#ifndef _app_h
#define _app_h
#include "d3d9.h"
#include "d3dx9.h"
#include "dxutil.h"
#include "d3dfont.h"

extern IDirect3DDevice9* device;
extern D3DXVECTOR3 Pos;
extern D3DXVECTOR3 Engine;
extern D3DXVECTOR3 Velocity;
extern float *h_map;

bool InitD3D(int w,int h,HWND hwnd);
bool Setup();
void Cleanup();
bool Display(float tdelta);
void onmdown(HWND hwnd);
void onmup(HWND hwnd);
void onrdown(HWND hwnd);
void onrup(HWND hwnd);
void onmove(HWND hwnd);
void onkey(int key,int keycode, bool down,int type);
void OnSize(int width,int height);

#endif
