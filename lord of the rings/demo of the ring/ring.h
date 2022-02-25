#ifndef _ring_h
#define _ring_h
#include "d3dx9.h"
#include "d3dfont.h"

extern IDirect3DDevice9* device;
extern DWORD BehaviorFlags;
extern D3DXVECTOR3 Pos;
extern D3DXVECTOR3 Engine;
extern D3DXVECTOR3 Velocity;
extern float *h_map;

struct vertex
{
	float _x,_y,_z;
	float _nx,_ny,_nz;
	float lx,ly,lz;
	float _u,_v;
	static const DWORD FVF;
	vertex(){}
	vertex(float x,float y,float z,float nx,float ny,float nz,float dx,float dy,float dz,/**/float u,float v)
	{_x=x;_y=y;_z=z;_nx=nx;_ny=ny;_nz=nz;lx=dx;ly=dy;lz=dz;/**/_u=u;_v=v;}
	vertex(D3DXVECTOR3 pos,D3DXVECTOR3 norm,D3DXVECTOR3 norm1,D3DXVECTOR2 tex)
	{_x=pos.x;_y=pos.y;_z=pos.z;_nx=norm.x;_ny=norm.y;_nz=norm.z;lx=norm1.x;ly=norm1.y;lz=norm1.z;/**/_u=tex.x;_v=tex.y;}
};

bool InitD3D(int w,int h,HWND hwnd);
bool Setup();
void Cleanup();
bool Display(float tdelta);
void onmdown(HWND hwnd);
void onmup(HWND hwnd);
void onrdown(HWND hwnd);
void onrup(HWND hwnd);
void onmove(HWND hwnd);
void onkey(int key,bool down,int type);
void Differ(float tdelta);
void OnSize(int width,int height);

#endif
