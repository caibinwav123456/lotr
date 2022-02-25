#ifndef _MESH_H
#define _MESH_H
#include "ring.h"
#include <math.h>
void BuildMesh(vertex* vertices,DWORD* indices);
void GetNumVert(int* nvert, int* nface);
#endif