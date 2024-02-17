#pragma once

typedef BOOL(*CallBackOnUnzipItem)(CString&);

// strDir is already exist.
BOOL UnZipToDir(CString strZipFile, CString strDir, CallBackOnUnzipItem cbfOnUnzipItem);
