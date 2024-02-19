#pragma once
#include "cstring.h"
#include "../xzip/XUnZip.h"
typedef BOOL(*CallBackOnUnzipItem)(CString&);

// strDir is already exist.
BOOL UnZipToDir(CString strZipFile, CString strDir, CallBackOnUnzipItem cbfOnUnzipItem);
BOOL UnZipInMemory(void* buf,int size,void*& dstbuf,int& dstsize);
