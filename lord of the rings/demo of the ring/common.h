#ifndef _COMMON_H_
#define _COMMON_H_
#ifndef NULL
#define NULL 0
#endif
#define VALID(M) ((M)==0?false:((M)==(void*)-1?((M)=0,false):true))
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
#define hiword(x) ((x)>>16)
#define loword(x) ((x)&0xffff)
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
#define verify(m) assert(m)
#else
#define verify(m) (m)
#endif
#if defined(WIN32) || defined(WINDOWS) || defined(_WINDOWS)
#define BUILD_ON_WINDOWS
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#ifndef DLL_IMPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#define DLLAPI(T) DLL T __stdcall
#define main_entry WINAPI _tWinMain
#define main_args HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd
#else
#ifndef DLL_IMPORT
#define DLL __attribute__((visibility("default")))
#else
#define DLL
#endif
#define DLLAPI(T) DLL T
#define main_entry main
#define main_args int argc, char** argv
#endif
#include "ASTError.h"
#include "defines.h"
#include "sys.h"
#endif