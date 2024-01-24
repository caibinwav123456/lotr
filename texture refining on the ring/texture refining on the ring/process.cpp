#include "stdafx.h"
#include "vector.h"
#include <math.h>
#include <vector>
#include <assert.h>
#include "process.h"
#if _MSC_VER < 1400
#define UINT8 unsigned char
#endif
using namespace std;
float **X;
float **Y;
float **_X;
float **_Y;
int nCt;
int *nP;
int w,h;
char *img;
struct VERTEX_FLAG
{
	int flag;
	int backup;
};
VERTEX_FLAG **flag = NULL;
POINT po[2][4][3];
int   di[2][4][3];
#define SAFE_RELEASE(p) {if(p){delete[]p;p=NULL;}}
#define GC_BASE   40
#define GC_LENGTH (GC_BASE*2+1)
#define GC_S      10
#define GATE      0.002
#define REFINE_FACTOR3_N    0.1//normal
#define REFINE_FACTOR3_P      1//point node
#define GC_BASE_NODE         5
#define GC_S_NODE            3
double  RF1 =      1;       //Curvation
double  RF2 =     10;       //Curvation difference
double  RF3 =    0.1;       //Deviation
int   MAX_INTERVAL=5;
int   MIN_SEG     =20;
float* GaussCore=NULL;
int GBase=0;
float GSD=0;
void InitGaussCore();
void FindInflect();

vector<float>xl,yl;
vector<int>nl;
vector<float*>xlist,ylist;
void GenContour();

void Release()
{
	for(int i=0;i<nCt;i++)
	{
		SAFE_RELEASE(X[i]);
		SAFE_RELEASE(Y[i]);
		SAFE_RELEASE(_X[i]);
		SAFE_RELEASE(_Y[i]);
		SAFE_RELEASE(flag[i]);
	}
	SAFE_RELEASE(X);
	SAFE_RELEASE(Y);
	SAFE_RELEASE(_X);
	SAFE_RELEASE(_Y);
	SAFE_RELEASE(flag);
	SAFE_RELEASE(nP);
	SAFE_RELEASE(GaussCore);
}

unsigned int& GP(int x,int y)
{
	x<0?x=0:(x>=w?x=w-1:x=x);
	y<0?y=0:(y>=h?y=h-1:y=y);
	return ((unsigned int*)img)[y*w+x];
}

void LoadData(int x,int y,char* data)
{
	w=x,h=y;
	img=data;
	int i;
	for(i=0;i<h;i++)
		for(int j=0;j<w;j++)
			GP(j,i)&=0x00ffffff;

	for(i=0;i<h;i++)
		for(int j=0;j<w;j++)
		{
			unsigned int& p=GP(j,i);
			unsigned int& pdx=GP(j+1,i);
			unsigned int& pdy=GP(j,i+1);
			if( ((float)_R((int)p)-128.5)*((float)_R((int)pdx)-128.5)<0 )
			{
				if((float)_R((int)p)-128.5<0)
					p|=0x01000000;
				else
					pdx|=0x04000000;
			}
			if( ((float)_R((int)p)-128.5)*((float)_R((int)pdy)-128.5)<0 )
			{
				if((float)_R((int)p)-128.5<0)
					p|=0x02000000;
				else
					pdy|=0x08000000;
			}
		}

	int xx[4]={1, 0,-1, 0};
	int xy[4]={0,-1, 0, 1};
	int yx[4]={0, 1, 0,-1};
	int yy[4]={1, 0,-1, 0};

	POINT p[2][3]={ {{0,0},{0,-1},{1,-1}} , {{0,0},{0,1},{1,1}} };
	int d[2][3]={{3,0,1},{1,0,3}};

	for(i=0;i<2;i++)
		for(int j=0;j<4;j++)
			for(int k=0;k<3;k++)
			{
				po[i][j][k].x=p[i][k].x*xx[j]+p[i][k].y*xy[j];
				po[i][j][k].y=p[i][k].x*yx[j]+p[i][k].y*yy[j];
				di[i][j][k]=(d[i][k]+j)%4;
			}
	GBase = GC_BASE;
	GSD = GC_S;
	InitGaussCore();
	GenContour();
	FindInflect();
}

void FindNextNode(POINT *node,int *dir,int wdo)
{
	for(int i=0;i<3;i++)
	{
		POINT tmp;
		tmp.x=po[wdo][*dir][i].x+node->x;
		tmp.y=po[wdo][*dir][i].y+node->y;
		int d=di[wdo][*dir][i];
		DWORD p=GP(tmp.x,tmp.y);
		if(_A(p)&(1<<d))
		{
			node->x=tmp.x,node->y=tmp.y;
			*dir=d;
			break;
		}
	}
}

void GenContour()
{
	int i;
	for(i=0;i<h;i++)
		for(int j=0;j<w;j++)
		{
			for(int k=0;k<4;k++)
			{
				DWORD p=GP(j,i);
				DWORD a=_A(p);
				if(((a&(1<<k))!=0)&&((a&(1<<(4+k)))==0))
				{
					xl.clear();
					yl.clear();
					int dir=k;
					POINT pt={j,i};
					do
					{
						float fx,fy;
						switch(dir)
						{
						case 0:fx=pt.x+0.5f,fy=pt.y;break;
						case 1:fx=pt.x,fy=pt.y+0.5f;break;
						case 2:fx=pt.x-0.5f,fy=pt.y;break;
						case 3:fx=pt.x,fy=pt.y-0.5f;break;
						}
						fx+=1.;
						fy+=1.;
						xl.push_back(fx);
						yl.push_back(fy);
						GP(pt.x,pt.y)|=(1<<(28+dir));
						FindNextNode(&pt,&dir,0);
					}while(pt.x!=j||pt.y!=i||dir!=k);
					int n=xl.size();
					if(n<=4)continue;
					float* conx=new float[n];
					float* cony=new float[n];
					for(int _i=0;_i<n;_i++)
					{
						conx[_i]=xl[_i];
						cony[_i]=yl[_i];
					}
					xlist.push_back(conx);
					ylist.push_back(cony);
					nl.push_back(n);
				}
			}
		}
	nCt=xlist.size();
	X=new float*[nCt];
	Y=new float*[nCt];
	_X=new float*[nCt];
	_Y=new float*[nCt];
	nP=new int[nCt];
	for(i=0;i<nCt;i++)
	{
		X[i]=xlist[i];
		Y[i]=ylist[i];
		nP[i]=nl[i];
		_X[i]=new float[nP[i]];
		_Y[i]=new float[nP[i]];
		for(int j=0;j<nP[i];j++)
		{
			_X[i][j]=X[i][j];
			_Y[i][j]=Y[i][j];
		}
	}
}

int GetNContour()
{
	return nCt;
}

void GetContour(int* n,float **x,float **y,int cnt)
{
	if(cnt>=0&&cnt<nCt)
	{
		*n=nP[cnt];
		*x=X[cnt];
		*y=Y[cnt];
	}
}

void InitGaussCore()
{
	if(GaussCore!=NULL)
	{
		delete[] GaussCore;
		GaussCore=NULL;
	}
	int GLength=GBase*2+1;
	int i;
	GaussCore=new float[GLength];
	for(i=0;i<GLength;i++)
	{
		GaussCore[i] = exp(-(float)(i-GBase)*(i-GBase)/GSD/GSD);
	}
	float total = 0;
	for(i=0;i<GLength;i++)
	{
		total += GaussCore[i];
	}
	for(i=0;i<GLength;i++)
	{
		GaussCore[i] /= total;
	}
}

void FindInflect()
{
	int i,j,k;
	flag = new VERTEX_FLAG*[nCt];

	for(i=0; i<nCt; i++)
	{
		flag[i] = new VERTEX_FLAG[nP[i]];
		for(j=0; j<nP[i]; j++)
		{
			flag[i][j].flag = flag[i][j].backup = 0;
		}
	}
	
	for(i=0;i<nCt;i++)
	{
		int np = nP[i];
		float* tmpX = new float[np];
		float* tmpY = new float[np];
		for(j=0;j<np;j++)
		{
			float accX=0,accY=0;
			for(k=-GC_BASE;k<GC_BASE;k++)
			{
				int si = j+k;
				if(si<0)
					while(si<0)
						si+=np;
				else if(si>=np)
					while(si>=np)
						si-=np;
				int nsi=si+1;
				if(nsi==np)
					nsi=0;
				float dx = X[i][nsi]-X[i][si];
				float dy = Y[i][nsi]-Y[i][si];

				accX+=dx*GaussCore[k+GC_BASE];
				accY+=dy*GaussCore[k+GC_BASE];
			}
			tmpX[j] = accX;
			tmpY[j] = accY;
		}
		for(j=0;j<np;j++)
		{
			int pi = j-1;
			if(pi<0)
				pi = np-1;
			int ni = j+1;
			if(ni>=np)
				ni=0;
			float offpX = tmpX[j]-tmpX[pi];
			float offpY = tmpY[j]-tmpY[pi];
			float offnX = tmpX[ni]-tmpX[j];
			float offnY = tmpY[ni]-tmpY[j];
			if(offpX*offnX+offpY*offnY<0)
			{
				flag[i][j].flag = 1;
			}
			if(offnX*offnX+offnY*offnY>GATE)
			{
				flag[i][j].flag = 2;
			}
		}
		delete[] tmpX;
		delete[] tmpY;
	}
	for(i=0;i<nCt;i++)
	{
		int np=nP[i];
		int start=-1;
		int old=-1;
		int acc=0;
		bool have=false;
		int loop=-1;
		for(j=0;;j++)
		{
			if(flag[i][j%np].flag==2)
			{
				have=true;
				if(start!=-1)
				{
					if(j-old<=MAX_INTERVAL)
					{
						old=j; 
					}
					else
					{
						if(old-start>=MIN_SEG)
						{
							flag[i][start%np].flag|=4;
							flag[i][old%np].flag|=4;
						}
						else
							flag[i][((start+old)/2)%np].flag|=4;
						start=j;
						old=j;
						if(j>=loop+np)
							break;
					}
				}
				else
				{
					if(old==-1||j-old<=MAX_INTERVAL)
					{
						old=j;
					}
					else
					{
						start=old=loop=j;
					}
					if(j>=np)break;
				}
			}
			if(!have&&j>=np)
				break;
		}

		for(j=0;j<np;j++)
		{
			flag[i][j].backup=flag[i][j].flag;
		}
	}
}

int& GetFlag(int i,int j)
{
	return flag[i][j].flag;
}

void RestoreFlag()
{
	for(int i=0;i<nCt;i++)
	{
		for(int j=0;j<nP[i];j++)
		{
			flag[i][j].flag=flag[i][j].backup;
		}
	}
}

void Refine(int index)
{
	int np = nP[index];
	GBase = GC_BASE_NODE;
	GSD = GC_S_NODE;
	InitGaussCore();
	for(int i=0;i<np;i++)
	{
		int p0 = i;
		int p1 = (i+1)%np;
		int p2 = (i+2)%np;
		int p3 = (i+3)%np;
		
		float x0=X[index][p0];
		float y0=Y[index][p0];
		float& x1=X[index][p1];
		float& y1=Y[index][p1];
		float& x2=X[index][p2];
		float& y2=Y[index][p2];
		float x3=X[index][p3];
		float y3=Y[index][p3];

		float _x1=_X[index][p1];
		float _y1=_Y[index][p1];
		float _x2=_X[index][p2];
		float _y2=_Y[index][p2];

		float acc=0;
		for(int j=-GC_BASE_NODE;j<GC_BASE_NODE;j++)
		{
			int smp=(i+j)%np;
			if(flag[index][smp].flag&4)
			{
				acc+=GaussCore[j+GC_BASE_NODE];
			}
		}
		RF3 = (REFINE_FACTOR3_P-REFINE_FACTOR3_N)*acc+REFINE_FACTOR3_N;

		double lFactor1 = 5*RF1+9*RF2+RF3;
		double lFactor2 = -4*RF1-9*RF2;
		double rFactor1 = 2*RF1+3*RF2;
		double rFactor2 = -RF1-3*RF2;

		double rItem1x = rFactor1*x0+rFactor2*x3;
		double rItem1y = rFactor1*y0+rFactor2*y3;
		double rItem2x = rFactor2*x0+rFactor1*x3;
		double rItem2y = rFactor2*y0+rFactor1*y3;

		double rSide1x = rItem1x+RF3*_x1;
		double rSide1y = rItem1y+RF3*_y1;
		double rSide2x = rItem2x+RF3*_x2;
		double rSide2y = rItem2y+RF3*_y2;

		double det = lFactor1*lFactor1-lFactor2*lFactor2;
		x1 = (float)((rSide1x*lFactor1-rSide2x*lFactor2)/det);
		y1 = (float)((rSide1y*lFactor1-rSide2y*lFactor2)/det);
		x2 = (float)((-lFactor2*rSide1x+lFactor1*rSide2x)/det);
		y2 = (float)((-lFactor2*rSide1y+lFactor1*rSide2y)/det);
		int a=1;
	}
}

struct Bucket
{
	int nc;
	int np;
	Bucket* next;
	Bucket* link;
	int nextnt; //next non-trivial
};

Bucket** BuildVertList()
{
	const int Pad=1;
	int i;
	Bucket** plist=new Bucket*[w*h];
	for(i=0;i<w*h;i++)
	{
		plist[i]=NULL;
	}
	for(i=0;i<nCt;i++)
	{
		float*x=X[i],*y=Y[i];
		int n=nP[i];
		for(int j=0;j<n;j++)
		{
			int nj=(j+1)%n;
			Vector start(x[j],y[j]);
			Vector v(x[nj]-x[j],y[nj]-y[j]);
			float len=length(v);
			if(len==0.0f)len=1.0f;
			int nstep=1;
			if(len>=4)
			{
				nstep=ceilf(len/4);
			}
			for(int k=0;k<nstep;k++)
			{
				Vector v1=start+v*((float)k/nstep);
				Vector v2=start+v*((float)(k+1)/nstep);
				int xstart=floorf(v1.x),xend=floorf(v2.x),
					ystart=floorf(v1.y),yend=floorf(v2.y);
				if(xstart>xend)swap(xstart,xend);
				if(ystart>yend)swap(ystart,yend);
				for(int l=xstart-Pad;l<=xend+Pad;l++)
				{
					for(int m=ystart-Pad;m<=yend+Pad;m++)
					{
						if(l<0||l>=w||m<0||m>=h)
							continue;
						Bucket* pb=plist[m*w+l];
						if(pb!=NULL&&pb->nc==i&&pb->np==j)
							continue;
						Bucket* it=new Bucket;
						it->nc=i,it->np=j,it->next=pb;
						plist[m*w+l]=it;
					}
				}
			}
		}
	}
	return plist;
}

void ReleaseList(Bucket** plist)
{
	for(int i=0;i<w*h;i++)
	{
		Bucket* pb=plist[i];
		while(pb!=NULL)
		{
			Bucket* p=pb;
			pb=pb->next;
			delete p;
		}
	}
	delete[] plist;
}

void QuickSort(float* val, int n)
{
	if(n<=1)return;
	float f=val[n/2];
	int low=0,high=n-1;
	int mid;
	while(low<high)
	{
		for(;val[low]<f&&low<n;low++);
		for(;val[high]>=f&&high>=0;high--);
		if(low<high)
			swap(val[low],val[high]);
	}
	if(high<0)
	{
		swap(val[0],val[n/2]);
		mid=1;
	}
	else
	{
		assert(low-high==1);
		mid=high+1;
	}
	QuickSort(val,mid);
	QuickSort(val+mid,n-mid);
}

inline bool Cut(float x1,float y1,float x2,float y2,float& cut,float scan)
{
	if(y1==y2)
		return false;
	if(y1>y2)
	{
		swap(y1,y2);
		swap(x1,x2);
	}
	if(y1>scan||y2<=scan)
		return false;
	cut=x1+(x2-x1)*(scan-y1)/(y2-y1);
	return true;
}

void Rasterize(int scan)
{
	static Bucket** plist=NULL;
	if(plist==NULL)
		plist=BuildVertList();

	if(scan==-1)
	{
		ReleaseList(plist);
		plist=NULL;
		return;
	}

	Bucket* rlist=NULL;
	int nr=0,n,i;
	int tmpflag=(1<<15);
	for(n=0;n<w;n++)
	{
		Bucket* pb=plist[scan*w+n];
		while(pb!=NULL)
		{
			if(!(flag[pb->nc][pb->np].flag&tmpflag))
			{
				pb->link=rlist;
				rlist=pb;
				flag[pb->nc][pb->np].flag|=tmpflag;
				nr++;
			}
			pb=pb->next;
		}
	}
	for(n=0;n<w;n++)
	{
		Bucket* pb=plist[scan*w+n];
		while(pb!=NULL)
		{
			flag[pb->nc][pb->np].flag&=~tmpflag;
			pb=pb->next;
		}
	}
	float* val=new float[w];
	for(i=0;i<w;i++)
	{
		val[i]=0;
	}
	bool btrivial=true;
	int nontrivial=-1;
	int p=-1;
	for(n=0;n<w;n++)
	{
		if(plist[scan*w+n]!=NULL)
		{
			if(nontrivial==-1)
				nontrivial=n;
			else
				plist[scan*w+p]->nextnt=n;
			p=n;
		}
	}
	if(p!=-1)plist[scan*w+p]->nextnt=-1;
	for(int layer=0;layer<10;layer++)
	{
		float line=scan+(float)layer/10;
		float* cuts=new float[nr];
		int ncut=0;
		Bucket* pr=rlist;
		for(i=0;i<nr&&pr!=NULL;i++,pr=pr->link)
		{
			int nc=pr->nc;
			int np=pr->np;
			int n=nP[nc];
			int np2=(np+1)%n;
			float x=X[nc][np],
				  y=Y[nc][np],
				  x2=X[nc][np2],
				  y2=Y[nc][np2];
			float cut;
			if(Cut(x,y,x2,y2,cut,line))
			{
				cuts[ncut]=cut;
				ncut++;
			}
		}
		QuickSort(cuts,ncut);
		int icut=0;
		bool black=false;
		int pn=0;
		for(n=nontrivial;;n=plist[scan*w+n]->nextnt)
		{
			bool end=false;
			if(n==-1||n>=w)
			{
				n=w;
				end=true;
			}
			while(cuts[icut]<pn&&icut<ncut)
			{
				icut++;
				black=!black;
			}
			if(btrivial)
			{
				//trivial
				for(int i=pn;i<n;i++)
				{
					val[i]=(black?1:0);
				}
			}
			if(end)break;
			pn=n+1;
			//refine for anti aliasing
			for(int j=0;j<10;j++)
			{
				float xc=n+(float)j/10;
				while(icut<ncut&&cuts[icut]<xc)
				{
					icut++;
					black=!black;
				}
				val[n]+=(black?0.01f:0.0f);
			}
		}
		delete[] cuts;
		btrivial=false;
	}
	for(i=0;i<w;i++)
	{
		UINT32 c=(UINT32)(UINT8)(val[i]*255);
		GP(i,scan)=(c|(c<<8)|(c<<16));
	}
	delete[] val;
}