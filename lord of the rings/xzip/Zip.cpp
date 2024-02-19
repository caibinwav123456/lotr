#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <assert.h>
#include "Zip.h"
#include "XZip.h"

BOOL AddDir(CString strDirSrc, CString strDir, HZIP hzip)
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA file;
	HANDLE handle=FindFirstFile(strDirSrc + _T("*.*"),&file);
	BOOL bFind = TRUE;
	while(handle!=INVALID_HANDLE_VALUE && bFind)
	{
		CString strFileNameSrc = strDirSrc +file.cFileName;
		CString strFileNameZip = strDir + file.cFileName;
		if(CString(file.cFileName) != _T(".") && CString(file.cFileName) != _T(".."))
		{
			if(PathIsDirectory(strFileNameSrc))
			{
				strFileNameSrc+=_T("\\");
				strFileNameZip+=_T("\\");
				if(ZipAdd(hzip, (LPCTSTR)strFileNameZip, 0,0,ZIP_FOLDER) != ZR_OK)
					bRet = FALSE;
				if(!AddDir(strFileNameSrc, strFileNameZip, hzip))
					bRet = FALSE;
			}
			else
			{
				if(ZipAdd(hzip, (LPCTSTR)strFileNameZip, (void*)(LPCTSTR)strFileNameSrc, 0, ZIP_FILENAME) != ZR_OK)
					bRet = FALSE;
			}
		}
		bFind = FindNextFile(handle,&file);
	}
	FindClose(handle);
	return bRet;
}

BOOL ZipToDir(CString strZipFile, CString strDir)
{
	BOOL ret = PathFileExists(strDir);
	if (! ret){
		return FALSE;
	}

	HZIP hz = CreateZip((void*)(LPCTSTR)strZipFile, 0, ZIP_FILENAME);
	ret = TRUE;
	if (hz)
	{
		ZRESULT zr = 0;
		int index = 0;

		if(strDir.Right(1) != _T("\\"))
			strDir+=_T("\\");

		BOOL bSuc = AddDir(strDir, _T(""), hz);


		assert(bSuc);

		CloseZip(hz);
	}

	return ret;
}