#include "stdafx.h"
#include "mesh.h"

//Constant Definition//
#define RINGSEG 100//
#define RINGRAD 4
#define RINGARC (100./360*D3DX_PI)
#define BIGRAD 3
#define SMALLRAD 0.5
#define BIGSEG 15//
#define SMALLSEG 11//
#define BIGARC (2./180*D3DX_PI)
#define SMALLARC (15./180*D3DX_PI)
#define LINESEG 10//
///////////////////////

float txtclp(float x);

void GetNumVert(int* nvert, int* nface)
{
	*nvert=(RINGSEG+2)*(BIGSEG+2*SMALLSEG+LINESEG+2);
	*nface=(RINGSEG)*(BIGSEG+2*SMALLSEG+LINESEG)*2;
}

void BuildMesh(vertex* vertices,DWORD* indices)
{
	vertex big[16],usmall[12],dsmall[12],line[11];
	float phi;
	int i;
	for(i=0;i<16;i++)
	{	phi=(-15.+2*i)/180*D3DX_PI;
	big[i]=vertex(1+3*cos(phi),3*sin(phi),0,cos(phi),sin(phi),0,0,0,0,0,0);}

	float x=1+2.75*cos(phi),y=2.75*sin(phi);

	for(i=0;i<12;i++)
	{	phi=(15.+15*i)/180*D3DX_PI;
	usmall[i]=vertex(0.25*cos(phi)+x,0.25*sin(phi)+y,0,cos(phi),sin(phi),0,0,0,0,0,0);}

	for(i=0;i<11;i++)
	{	line[i]=vertex(usmall[11]._x,usmall[11]._y/5*(5-i),0,-1,0,0,0,0,0,0,0);}
//	line[i]=vertex(0,0,0,0,0,0,0,0,0,0,0);}
	for(i=0;i<12;i++)
	{	dsmall[i]=usmall[11-i];dsmall[i]._ny*=-1;dsmall[i]._y*=-1;/**/}

	vertex loop[49];
	for(i=0;i<7;i++)
	{	loop[i]=usmall[i+5];}
	for(i=0;i<10;i++)
	{	loop[i+7]=line[i+1];}
	for(i=0;i<6;i++)
	{	loop[i+17]=dsmall[i+1];}
	for(i=0;i<6;i++)
	{	loop[i+23]=dsmall[i+6];}
	for(i=0;i<15;i++)
	{	loop[i+29]=big[i+1];}
	for(i=0;i<5;i++)
	{	loop[i+44]=usmall[i+1];}

	for(i=0;i<23;i++)
	{	loop[i].ly=loop[i]._nx;loop[i].lx=-loop[i]._ny;
		loop[i]._v=txtclp(1./22*i)*0.5;loop[i]._u=loop[i]._v+0.5;}
	for(i=0;i<26;i++)
	{	loop[i+23].ly=-loop[i+23]._nx;loop[i+23].lx=loop[i+23]._ny;
		loop[i+23]._v=txtclp(1-1./25*i)*0.5;loop[i+23]._u=0.5+loop[i+23]._v;}

	vertex ring[102*49];
	for(i=0;i<51;i++)
		for(int j=0;j<49;j++)
		{
			vertex &v=ring[i*49+j];
			vertex &l=loop[j];
			v._y=l._y;v._ny=l._ny;v.ly=l.ly;
			v._x=l._x*cos(D3DX_PI/50*i);//+l._y*sin(D3DX_PI/50*i)
			v._z=l._x*sin(D3DX_PI/50*i);
			v._nx=l._nx*cos(D3DX_PI/50*i);
			v._nz=l._nx*sin(D3DX_PI/50*i);
			v.lx=l.lx*cos(D3DX_PI/50*i);
			v.lz=l.lx*sin(D3DX_PI/50*i);
			v._u=1./50*(j>=23?i:50-i);v._v=l._v;
		}
	for(i=51;i<102;i++)
		for(int j=0;j<49;j++)
		{
			vertex &v=ring[i*49+j];
			vertex &l=loop[j];
			v._y=l._y;v._ny=l._ny;v.ly=l.ly;
			v._x=l._x*cos(D3DX_PI/50*(i-1));//+l._y*sin(D3DX_PI/50*i)
			v._z=l._x*sin(D3DX_PI/50*(i-1));
			v._nx=l._nx*cos(D3DX_PI/50*(i-1));
			v._nz=l._nx*sin(D3DX_PI/50*(i-1));
			v.lx=l.lx*cos(D3DX_PI/50*(i-1));
			v.lz=l.lx*sin(D3DX_PI/50*(i-1));
			v._u=1./50*(j>=23?i-51:101-i);v._v=l._u;
		}

	for(i=0;i<102*49;i++)
		vertices[i]=ring[i];

	for(i=0;i<50;i++)
	{
		int j;
		for(j=0;j<22;j++)
		{
			indices[(i*47+j)*6]=i*49+j;
			indices[(i*47+j)*6+1]=i*49+j+1;
			indices[(i*47+j)*6+2]=i*49+j+49+1;
			indices[(i*47+j)*6+3]=i*49+j+49+1;
			indices[(i*47+j)*6+4]=i*49+j+49;
			indices[(i*47+j)*6+5]=i*49+j;
		}
		for(j=23;j<48;j++)
		{
			indices[(i*47+j-1)*6]=i*49+j;
			indices[(i*47+j-1)*6+1]=i*49+j+1;
			indices[(i*47+j-1)*6+2]=i*49+j+49+1;
			indices[(i*47+j-1)*6+3]=i*49+j+49+1;
			indices[(i*47+j-1)*6+4]=i*49+j+49;
			indices[(i*47+j-1)*6+5]=i*49+j;
		}
	}
	for(i=51;i<101;i++)
	{
		int j;
		for(j=0;j<22;j++)
		{
			indices[((i-1)*47+j)*6]=i*49+j;
			indices[((i-1)*47+j)*6+1]=i*49+j+1;
			indices[((i-1)*47+j)*6+2]=i*49+j+49+1;
			indices[((i-1)*47+j)*6+3]=i*49+j+49+1;
			indices[((i-1)*47+j)*6+4]=i*49+j+49;
			indices[((i-1)*47+j)*6+5]=i*49+j;
		}
		for(j=23;j<48;j++)
		{
			indices[((i-1)*47+j-1)*6]=i*49+j;
			indices[((i-1)*47+j-1)*6+1]=i*49+j+1;
			indices[((i-1)*47+j-1)*6+2]=i*49+j+49+1;
			indices[((i-1)*47+j-1)*6+3]=i*49+j+49+1;
			indices[((i-1)*47+j-1)*6+4]=i*49+j+49;
			indices[((i-1)*47+j-1)*6+5]=i*49+j;
		}
	}
}
#define MIN 0.15
#define MAX 0.85
float txtclp(float x)
{
	float r;
	if(x<MIN)r=x/MIN*0.1;
	else if(x>=MIN&&x<MAX)r=0.1+(x-MIN)/(MAX-MIN)*(0.9-0.1);
	else r=0.9+(x-MAX)/(1-MAX)*(1-0.9);
	return r;
}