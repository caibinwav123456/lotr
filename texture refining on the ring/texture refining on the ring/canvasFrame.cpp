// canvasFrame.cpp : implementation file
//

#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"
#include "process.h"
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "vector.h"
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
float X=0;
float Y=0;
int Width=0;
int Height=0;
/////////////////////////////////////////////////////////////////////////////
// canvasFrame
int nc=0;
int _select=-1;
int _ns=-1,_nsnode=-1;
int _nspre=-1,_nsnodepre=-1;
float sX=X,sY=Y;
float lX=0,lY=0;
float cX=0,cY=0;
float dscale=1;
float bscale=0.5;
float scale=0.5;
int wX=1000;
int wY=1000;
bool mdn=0;
POINT wp;
int showmode=0;
bool showpoint=false;
bool shownode=false;
bool showcontour=true;
int curflag=0;
char *px;
char num[50];
IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)
CPen pen,pst,ypen,gpen,bpen,rpen,ppen,rpenw;
void Select(CPoint point, int& n, int& np, int range=5);
void canvasFrame::LoadMap(char* name)
{
	Src=new CBitmap;
	Mix=new CBitmap;
	blank=new CBitmap;
	Src->m_hObject=LoadImage(0,name,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	BITMAP bm;
	Src->GetBitmap(&bm);
	sX=X=Width=bm.bmWidth;
	sY=Y=Height=bm.bmHeight;
	wY=wX*Height/Width;
	px=new char[Width*Height*4];
	Src->GetBitmapBits(Width*Height*4,px);
	LoadData(Width,Height,px);
	Src->SetBitmapBits(Width*Height*4,px);
	mdc=new CDC;
	mxdc=new CDC;
	CClientDC dc(this);
	mdc->CreateCompatibleDC(&dc);
	mxdc->CreateCompatibleDC(&dc);
	Mix->CreateCompatibleBitmap(&dc,wX,wY);
	blank->CreateCompatibleBitmap(&dc,wX,wY);
	mdc->SelectObject(blank);
	mxdc->SelectObject(Mix);
	for(int i=0;i<wY;i++)
		for(int j=0;j<wX;j++)
		{
			mdc->SetPixel(j,i,RGB(255,255,255));
		}
	nc=GetNContour();
	LoadConfig();
}

void canvasFrame::LoadConfig()
{
	CFile file;
	if(file.Open(_T("refine_save"),CFile::modeRead))
	{
		int n;
		file.Read(&n, sizeof(int));
		for(int i=0;i<nc&&i<n;i++)
		{
			int np,j,_n;
			file.Read(&np,sizeof(int));
			float*x,*y;
			GetContour(&_n,&x,&y,i);
			for(j=0;j<_n;j++)
			{
				GetFlag(i,j)&=~4;
			}
			for(j=0;j<np;j++)
			{
				int index;
				file.Read(&index,sizeof(int));
				if(index<_n)
					GetFlag(i,index)|=4;
			}
		}
		file.Close();
	}
}

canvasFrame::canvasFrame()
{
	RECT rect;
	Create(NULL,"texture refining");
	CClientDC dc(this);
	int width=dc.GetDeviceCaps(HORZRES);
	int height=dc.GetDeviceCaps(VERTRES);
	//GetWindowRect(&rect);
	int x=640,y=480;
	LoadMap("output.bmp");
	width=(width-(wX))/2;
	height=(height-(wY))/2;
	rect.left=width,rect.top=height,rect.right=width+wX,rect.bottom=height+wY;
	CalcWindowRect(&rect);
	MoveWindow(&rect,true);
	pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	pst.CreatePen(PS_SOLID,1,RGB(255,0,0));
	ypen.CreatePen(PS_SOLID,1,RGB(255,255,0));
	gpen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	bpen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	rpen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	ppen.CreatePen(PS_SOLID,1,RGB(255,0,255));
	rpenw.CreatePen(PS_SOLID,2,RGB(255,0,0));
	selectmode=0;
	timer_on=false;
}

canvasFrame::~canvasFrame()
{


}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

BOOL canvasFrame::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return 0;//CFrameWnd::OnEraseBkgnd(pDC);
}

void canvasFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Draw();
}
void canvasFrame::Draw()
{
	CClientDC dc(this);
	mdc->SelectObject(blank);
	mxdc->BitBlt(0,0,wX,wY,mdc,0,0,SRCCOPY);
	mdc->SelectObject(Src);
	if(showmode==0)
	{
		mxdc->SetStretchBltMode(COLORONCOLOR);
		mxdc->StretchBlt(0,0,wX,wY,mdc,lX,lY,sX,sY,SRCCOPY);
	}
	CPen* oldpen;
	oldpen=mxdc->SelectObject(&pen);

	if(showcontour)
	{
		for(int i=0;i<nc;i++)
		{
			if(i==_select)
				mxdc->SelectObject(&pst);
			else
				mxdc->SelectObject(&pen);
			int np;
			float *x,*y;
			GetContour(&np,&x,&y,i);
			int px=(x[np-1]-lX)/sX*wX;
			int py=(y[np-1]-lY)/sY*wY;
			mxdc->MoveTo(px,py);
			for(int j=0;j<np;j++)
			{
				px=(x[j]-lX)/sX*wX;
				py=(y[j]-lY)/sY*wY;
				mxdc->LineTo(px,py);
			}
		}
	}
	mxdc->SelectObject(&gpen);
	if(showpoint)
	{
		for(int i=0;i<nc;i++)
		{
			int np;
			float *x,*y;
			GetContour(&np,&x,&y,i);

			for(int j=0;j<np;j++)
			{
				if(GetFlag(i,j)==2)
				{
					int px=(x[j]-lX)/sX*wX;
					int py=(y[j]-lY)/sY*wY;
					mxdc->MoveTo(px-5,py-5);
					mxdc->LineTo(px+5,py+5);
					mxdc->MoveTo(px+5,py-5);
					mxdc->LineTo(px-5,py+5);
				}
			}
		}
	}
	mxdc->SelectObject(&rpen);
	if(shownode)
	{
		for(int i=0;i<nc;i++)
		{
			int np;
			float *x,*y;
			GetContour(&np,&x,&y,i);

			for(int j=0;j<np;j++)
			{
				if(GetFlag(i,j)&4)
				{
					int px=(x[j]-lX)/sX*wX;
					int py=(y[j]-lY)/sY*wY;
					mxdc->MoveTo(px-5,py-5);
					mxdc->LineTo(px+5,py+5);
					mxdc->MoveTo(px+5,py-5);
					mxdc->LineTo(px-5,py+5);
				}
			}
		}
	}
	if(_nspre>=0&&_nsnodepre>=0)
	{
		int np;
		float *x,*y;
		GetContour(&np,&x,&y,_nspre);
		if(GetFlag(_nspre, _nsnodepre)&4)
			mxdc->SelectObject(rpenw);
		else
			mxdc->SelectObject(ppen);
		int px=(x[_nsnodepre]-lX)/sX*wX;
		int py=(y[_nsnodepre]-lY)/sY*wY;
		mxdc->MoveTo(px-5,py-5);
		mxdc->LineTo(px+5,py+5);
		mxdc->MoveTo(px+5,py-5);
		mxdc->LineTo(px-5,py+5);
	}
	if(_ns>=0&&_nsnode>=0&&(_ns!=_nspre||_nsnode!=_nsnodepre))
	{
		int np;
		float *x,*y;
		GetContour(&np,&x,&y,_ns);
		if(GetFlag(_ns,_nsnode)&4)
			mxdc->SelectObject(rpenw);
		else
			mxdc->SelectObject(ppen);
		int px=(x[_nsnode]-lX)/sX*wX;
		int py=(y[_nsnode]-lY)/sY*wY;
		mxdc->MoveTo(px-10,py-10);
		mxdc->LineTo(px+10,py+10);
		mxdc->MoveTo(px+10,py-10);
		mxdc->LineTo(px-10,py+10);
	}
	mxdc->SelectObject(oldpen);
	dc.BitBlt(0,0,wX,wY,mxdc,0,0,SRCCOPY);
	if(_select>=0)
		sprintf(num,"curve id=%d,RF1=%.1f",_select,RF1);
	else
		sprintf(num,"RF1=%.1f",RF1);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,0));
	dc.TextOut(0,0,num);
	// TODO: Add your message handler code here
	
	// Do not call CFrameWnd::OnPaint() for painting messages
}

void canvasFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();

	// TODO: Add your message handler code here
	Src->DeleteObject();
	Mix->DeleteObject();
	blank->DeleteObject();
	mdc->DeleteDC();
	mxdc->DeleteDC();
	delete Src;
	delete Mix;
	delete blank;
	delete mdc;
	delete mxdc;
	delete[] px;
	Release();
}
void AdjustRect(POINT point)
{
	float tX=lX+(float)point.x/wX*sX;
	float tY=lY+(float)point.y/wY*sY;
	lX-=tX-cX;
	lY-=tY-cY;
	if(lX<0)
	{
		cX-=lX;//cX=-lX+tX
		lX=0;
	}
	else if(lX+sX>X)
	{
		cX-=lX+sX-X;
		lX=X-sX;
	}
	if(lY<0)
	{
		cY-=lY;
		lY=0;
	}
	else if(lY+sY>Y)
	{
		cY-=lY+sY-Y;
		lY=Y-sY;
	}
}

void canvasFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!mdn)
	{
		cX=lX+(float)point.x/wX*sX;
		cY=lY+(float)point.y/wY*sY;
		if(selectmode==0)
			_nspre=_nsnodepre=-1;
		else if(selectmode==1)
			Select(point, _nspre, _nsnodepre);
	}
	else if(_ns>=0&&_nsnode>=0)
	{
		_nspre=_nsnodepre=-1;
		int ns,nsnode;
		Select(point,ns,nsnode,0);
		if(ns==_ns&&nsnode>=0)
		{
			GetFlag(_ns,_nsnode)=curflag;
			_nsnode=nsnode;
			curflag=GetFlag(_ns,_nsnode);
			GetFlag(_ns,_nsnode)|=4;
		}
	}
	else
	{
		AdjustRect(point);
	}
	Draw();
	wp.x=point.x,wp.y=point.y;

	CFrameWnd::OnMouseMove(nFlags, point);
}

void canvasFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mdn=1;
	int np;
	Select(point, _select, np);
	if(_nspre!=-1&&_nsnodepre!=-1)
	{
		_ns=_nspre;
		_nsnode=_nsnodepre;
		curflag=(GetFlag(_ns,_nsnode)&~4);
		GetFlag(_ns,_nsnode)|=4;
	}
	else
	{
		_ns=_nsnode=-1;
		curflag=0;
	}
	Draw();
	CFrameWnd::OnLButtonDown(nFlags, point);
}

void canvasFrame::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mdn=0;
	CFrameWnd::OnLButtonUp(nFlags, point);
}

BOOL canvasFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	float ps=dscale;
	if(zDelta>0)dscale*=0.9;
	else dscale/=0.9;
	if(dscale>1)dscale=1;
	scale=bscale*dscale;
	sX=X*dscale;sY=Y*dscale;

	AdjustRect(wp);
	Draw();
	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void canvasFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar=='S')
	{
		IplImage *pic=cvCreateImageHeader(cvSize(Width,Height),IPL_DEPTH_8U,4);
		cvSetData(pic,px,4*Width);
		cvSaveImage("jk.bmp",pic);
		cvReleaseImageHeader(&pic);
	}
	else if(nChar=='Z')
	{
		showmode=!showmode;
	}
	else if(nChar==VK_LEFT)
	{
		if(_select==-1)
			_select=0;
		else
			_select=(_select+nc-1)%nc;
	}
	else if(nChar==VK_RIGHT)
	{
		if(_select==-1)
			_select=0;
		else
			_select=(_select+1)%nc;
	}
	else if(nChar=='R')
	{
		if(_select!=-1)
		{
			Refine(_select);
		}
		else
		{
			for(int i=0;i<nc;i++)
			{
				Refine(i);
			}
		}
	}
	else if(nChar=='Q')
	{
		showpoint=!showpoint;
	}
	else if(nChar==VK_F1)
	{
		RF1=1;
	}
	else if(nChar==VK_F2)
	{
		RF1=10;
	}
	else if(nChar=='W')
	{
		shownode=!shownode;
	}
	else if(nChar=='N')
	{
		if(_ns>=0&&_nsnode>=0)
		{
			int np;
			float *x,*y;
			GetContour(&np,&x,&y,_ns);
			int pi=(_nsnode+np-1)%np;
			GetFlag(_ns,_nsnode)=curflag;
			_nsnode=pi;
			curflag=GetFlag(_ns,_nsnode);
			GetFlag(_ns,_nsnode)|=4;
		}
	}
	else if(nChar=='M')
	{
		if(_ns>=0&&_nsnode>=0)
		{
			int np;
			float *x,*y;
			GetContour(&np,&x,&y,_ns);
			int ni=(_nsnode+1)%np;
			GetFlag(_ns,_nsnode)=curflag;
			_nsnode=ni;
			curflag=GetFlag(_ns,_nsnode);
			GetFlag(_ns,_nsnode)|=4;
		}
	}
	else if(nChar=='F')
	{
		CFile file;
		if(file.Open(_T("refine_save"), CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(&nc, sizeof(int));
			for(int i=0;i<nc;i++)
			{
				int n,j;
				float* x,*y;
				GetContour(&n,&x,&y,i);
				int sum=0;
				for(j=0;j<n;j++)
				{
					if(GetFlag(i,j)&4)
						sum++;
				}
				file.Write(&sum,sizeof(int));
				for(j=0;j<n;j++)
				{
					if(GetFlag(i,j)&4)
						file.Write(&j,sizeof(int));
				}
			}
			file.Close();
		}
	}
	else if(nChar=='Y')
	{
		RestoreFlag();
	}
	else if(nChar=='T')
	{
		selectmode=(selectmode+1)%2;
		if(selectmode==0)
			_nspre=_nsnodepre=-1;
	}
	else if(nChar=='P')
	{
		timer_on=true;
		SetTimer(1, 1, NULL);
	}
	else if(nChar=='X')
	{
		showcontour=!showcontour;
	}
	else if(nChar=='O')
	{
		IplImage*image=cvCreateImage(cvSize(Width,Height),IPL_DEPTH_8U,3);
		for(int i=0;i<Height;i++)
		{
			for(int j=0;j<Width;j++)
			{
				memcpy((uchar*)image->imageData+i*image->widthStep+j*image->nChannels,
					px+i*Width*4+j*4, 3);
			}
		}
		cvSaveImage("refine_output.bmp", image);
		cvReleaseImage(&image);
	}
	else if(nChar=='I')
	{
		IplImage*image=cvCreateImage(cvSize(Width,Height),IPL_DEPTH_8U,3);
		for(int i=0;i<Height;i++)
		{
			for(int j=0;j<Width;j++)
			{
				memcpy((uchar*)image->imageData+i*image->widthStep+j*image->nChannels,
					px+i*Width*4+j*4, 3);
			}
		}
		CvSize sz=cvSize(720,1280);
		IplImage* scr=cvCreateImage(sz,IPL_DEPTH_8U,3);
		cvZero(scr);
		int h=image->height*sz.width/image->width;
		CvRect rc=cvRect(0,(sz.height-h)/2,sz.width,h);
		cvSetImageROI(scr,rc);
		cvResize(image,scr);
		cvResetImageROI(scr);
		cvReleaseImage(&image);
		cvSaveImage("screen.bmp",scr);
		cvReleaseImage(&scr);
	}
	else if(nChar=='V')
	{
		CFile file;
		if(file.Open(_T("vertex"),CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(&nc,sizeof(nc));
			for(int i=0;i<nc;i++)
			{
				float*x,*y;
				int np;
				GetContour(&np,&x,&y,i);
				file.Write(&np,sizeof(np));
				for(int j=0;j<np;j++)
				{
					file.Write(x+j,sizeof(float));
					file.Write(y+j,sizeof(float));
				}
			}
			file.Close();
		}
	}
	Draw();
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void canvasFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(_nspre>=0&&_nsnodepre>=0)
	{
		GetFlag(_nspre, _nsnodepre)&=~4;
		Draw();
	}
	CFrameWnd::OnRButtonDown(nFlags, point);
}
void Select(CPoint point, int& n, int& _np, int range)
{
	int i;
	for(i=0;i<nc;i++)
	{
		int np;
		float *x,*y;
		GetContour(&np,&x,&y,i);
		int j;
		for(j=0;j<np;j++)
		{
			int dj=(j+1)%np;
			float px1=(x[j]-lX)/sX*wX;
			float py1=(y[j]-lY)/sY*wY;
			float px2=(x[dj]-lX)/sX*wX;
			float py2=(y[dj]-lY)/sY*wY;
			Vector v1(px1,py1);
			Vector v2(px2,py2);
			Vector cur(point.x,point.y);
			Vector normal=norm(v2-v1);
			float l=length(v2-v1);
			Vector tn(normal.y,-normal.x);
			Vector c=cur-v1;
			float r1=c*normal,r2=c*tn;
			if(r1>-1&&r1<l+1&&fabs(r2)<2)
			{
				n=i;
				_np=j;
				for(int k=-range;k<=range;k++)
				{
					int sign=((k+range)%2)*2-1;
					int r=(k+range+1)/2;
					int _k=(j+r*sign+np)%np;
					if(GetFlag(i,_k)&4)
					{
						_np=_k;
						break;
					}
				}
				break;
			}
		}
		if(j!=np)break;
	}
	if(i==nc)n=-1;
}

void canvasFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==1&&timer_on)
	{
		static int n=0;
		Rasterize(n);
		Src->SetBitmapBits(Width*Height*4,px);
		Draw();
		n++;
		if(n>=Height)
		{
			timer_on=false;
			KillTimer(1);
			n=0;
			Rasterize(-1);
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}
