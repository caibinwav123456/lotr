#include "stdafx.h"
#include "UnZip.h"
#include "xzip/XUnzip.h"

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

		_ASSERT(ZR_OK == zr);

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

			TRACE(_T("item name %s\n"), ze.name);

			CString strFn(ze.name);
			if (cbfOnUnzipItem)
			{
				(*cbfOnUnzipItem)(strFn);
			}
			CString strFullPathName = strFn;
			strFullPathName.Replace('/', '\\');
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