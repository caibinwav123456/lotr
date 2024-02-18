#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "UnZip.h"
#include "XUnzip.h"

BOOL UnZipToDir(CString strZipFile, CString strDir, CallBackOnUnzipItem cbfOnUnzipItem)
{
	BOOL ret = PathFileExists(strDir);
	if (! ret){
		return FALSE;
	}

	TCHAR Buffer[MAX_PATH];
	DWORD dwRet;

	dwRet = GetCurrentDirectory(MAX_PATH, Buffer);

	SetCurrentDirectory(strDir);

	HZIP hz = OpenZip((void*)(LPCTSTR)strZipFile, 0, ZIP_FILENAME);
	ret = TRUE;
	if (hz)
	{
		ZRESULT zr = 0;
		int index = 0;
		ZIPENTRYW ze;

		memset(&ze, 0, sizeof(ze));
		zr = GetZipItem(hz, -1, &ze);

		assert(ZR_OK == zr);

		int num = ze.index;
		int i;

		for (i = 0; i < num; i++)
		{
			zr = GetZipItem(hz, i, &ze);
			if (ZR_OK != zr)
			{
				ret = FALSE;
				continue;
			}

			CString strFn(ze.name);
			if (cbfOnUnzipItem)
			{
				(*cbfOnUnzipItem)(strFn);
			}
			CString strFullPathName = strFn;
			strFullPathName.Replace(_T('/'), _T('\\'));
			CString strDir2 = strDir;
			if(strDir2.Right(1)!=_T("\\"))
				strDir2+=_T("\\");
			strFullPathName = strDir2+strFullPathName;
			SetFileAttributes(strFullPathName, FILE_ATTRIBUTE_NORMAL);
			zr = UnzipItem(hz, i, (void*)(LPCTSTR)strFn, 0, ZIP_FILENAME);
			if (ZR_OK != zr){
				ret = FALSE;
			}
		}

		CloseZip(hz);
	}

	SetCurrentDirectory(Buffer);

	return ret;
}
BOOL UnZipInMemory(void* buf,int size,void*& dstbuf,int& dstsize)
{
	if(size<4)
		return FALSE;
	BOOL ret=TRUE;
	char *srcbuf=NULL;
	dstsize=*(DWORD*)buf;
	dstbuf=new char[dstsize];
	size-=sizeof(DWORD);
	srcbuf=(char*)buf+sizeof(DWORD);
	HZIP hz=OpenZip(srcbuf,size,ZIP_MEMORY);
	if(!hz)
		return FALSE;
	ZIPENTRYW ze;

	memset(&ze,0,sizeof(ze));
	if(GetZipItem(hz,-1,&ze)!=ZR_OK)
	{
		ret=FALSE;
		goto end;
	}
	if(ze.index<=0)
	{
		ret=FALSE;
		goto end;
	}
	if(GetZipItem(hz,0,&ze)!=ZR_OK)
	{
		ret=FALSE;
		goto end;
	}
	if(UnzipItem(hz,0,dstbuf,dstsize,ZIP_MEMORY)!=ZR_OK)
		ret=FALSE;
end:
	CloseZip(hz);
	return ret;
}
