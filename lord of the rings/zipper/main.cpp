#include "Windows.h"
#include "shlwapi.h"
#include "Zip.h"
#include "UnZip.h"
#include "stdio.h"
#include "tchar.h"
#define safe_delete(ptr) if(ptr!=NULL)delete[] ptr
TCHAR *srcfile=NULL,*dstfile=NULL;
bool bdecompress=false;
bool binmem=false;
bool parse_args(int argc,TCHAR** argv)
{
	bdecompress=false;
	for(int i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			for(TCHAR* ptr=&argv[i][1];*ptr!=0&&*ptr!=' ';ptr++)
			{
				switch(*ptr)
				{
				case _T('x'):
					bdecompress=true;
					break;
				case _T('m'):
					binmem=true;
					break;
				default:
					_tprintf(_T("invalid option: \'%s\'\n"),argv[i]);
					return false;
				}
			}
			continue;
		}
		if(srcfile==NULL)
			srcfile=argv[i];
		else if(dstfile==NULL)
			dstfile=argv[i];
		else
		{
			printf("too many args\n");
			return false;
		}
	}
	if(srcfile==NULL||dstfile==NULL)
	{
		printf("too few args\n");
		return false;
	}
	return true;
}
int _tmain(int argc,TCHAR** argv)
{
	if(!parse_args(argc,argv))
		return -1;
	if ((!PathFileExists(srcfile))
		||PathIsDirectory(srcfile))
	{
		_tprintf(_T("invalid src file:\'%s\'"),srcfile);
		return -1;
	}
	int ret=0;
	HZIP hz;
	if(!bdecompress)
	{
		hz=CreateZip((void*)(LPCTSTR)dstfile,0,ZIP_FILENAME);
		if(!hz)
			return -1;
		int index=0;
		if(ZipAdd(hz,(LPCTSTR)srcfile,srcfile,0,ZIP_FILENAME)!=ZR_OK)
			ret=-1;
		CloseZip(hz);
		if(ret==0&&binmem)
		{
			HANDLE hSrc=CreateFile(srcfile,GENERIC_READ,0,NULL,OPEN_EXISTING,NULL,NULL);
			DWORD size=GetFileSize(hSrc,NULL);
			CloseHandle(hSrc);

			HANDLE hDst=CreateFile(dstfile,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,NULL,NULL);
			DWORD rdwrlen;
			DWORD dstsize=GetFileSize(hDst,NULL);
			char* buf=new char[dstsize];
			if(!ReadFile(hDst,buf,dstsize,&rdwrlen,NULL))
			{
				ret=-1;
				goto end;
			}
			SetFilePointer(hDst,0,0,FILE_BEGIN);
			if(!WriteFile(hDst,&size,sizeof(size),&rdwrlen,NULL))
			{
				ret=-1;
				goto end;
			}
			if(!WriteFile(hDst,buf,dstsize,&rdwrlen,NULL))
			{
				ret=-1;
				goto end;
			}
end:
			delete[] buf;
			CloseHandle(hDst);
		}
	}
	else
	{
		if(binmem)
		{
			char *buf=NULL,*dstbuf=NULL;
			int size=0,dstsize=0;
			HANDLE hSrc=CreateFile(srcfile,GENERIC_READ,0,NULL,OPEN_EXISTING,NULL,NULL),hDst;
			size=GetFileSize(hSrc,NULL);
			buf=new char[size];
			DWORD rdwrlen;
			if(!ReadFile(hSrc,buf,size,&rdwrlen,NULL))
			{
				CloseHandle(hSrc);
				ret=-1;
				goto end_1;
			}
			CloseHandle(hSrc);
			if(!UnZipInMemory(buf,size,*(void**)&dstbuf,dstsize))
			{
				ret=-1;
				goto end_1;
			}
			hDst=CreateFile(dstfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,NULL,NULL);
			if(!WriteFile(hDst,dstbuf,dstsize,&rdwrlen,NULL))
				ret=-1;
			CloseHandle(hDst);
end_1:
			safe_delete(buf);
			safe_delete(dstbuf);
			return ret;
		}
		hz=OpenZip(srcfile,0,ZIP_FILENAME);
		if(!hz)
			return -1;
		ZIPENTRYW ze;

		memset(&ze,0,sizeof(ze));
		if(GetZipItem(hz,-1,&ze)!=ZR_OK)
		{
			ret=-1;
			goto end2;
		}
		if(ze.index<=0)
		{
			ret=-1;
			goto end2;
		}
		if(GetZipItem(hz,0,&ze)!=ZR_OK)
		{
			ret=-1;
			goto end2;
		}
		if(UnzipItem(hz,0,dstfile,0,ZIP_FILENAME)!=ZR_OK)
		{
			ret=-1;
		}
end2:
		CloseZip(hz);
	}
	return ret;
}