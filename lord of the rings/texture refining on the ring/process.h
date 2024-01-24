#ifndef _PROCESS_H
#define _PROCESS_H
#define _B(m) ((m)&0xff)
#define _G(m) (((m)>>8)&0xff)
#define _R(m) (((m)>>16)&0xff)
#define _A(m) (((m)>>24)&0xff)
extern double RF1,RF2,RF3;
void LoadData(int x,int y,char* data);
unsigned int& GP(int x,int y);
int GetNContour();
void GetContour(int* n,float **x,float **y,int cnt);
void FindNextNode(POINT *node,int *dir,int wdo);
int& GetFlag(int i,int j);
void Refine(int index);
void Release();
void RestoreFlag();
void Rasterize(int scan);
#endif