#pragma once
#include <string>
#include <tchar.h>
class CString:private std::basic_string<TCHAR>
{
public:
	CString();
	CString(const TCHAR* str);
	CString(const CString& other);
	void operator=(const TCHAR* str);
	void operator=(const CString& str);
	operator const TCHAR*() const;

	CString operator+(const CString& other) const;
	CString operator+(const TCHAR* other) const;
	void operator+=(const CString& other);
	void operator+=(const TCHAR* other);

	bool operator==(const CString& other) const;
	bool operator==(const TCHAR* other) const;
	bool operator!=(const CString& other) const;
	bool operator!=(const TCHAR* other) const;
	bool operator<(const CString& other) const;
	bool operator<(const TCHAR* other) const;
	bool operator<=(const CString& other) const;
	bool operator<=(const TCHAR* other) const;
	bool operator>(const CString& other) const;
	bool operator>(const TCHAR* other) const;
	bool operator>=(const CString& other) const;
	bool operator>=(const TCHAR* other) const;

	int GetLength() const;
	CString Left(int num) const;
	CString Mid(int start,int num) const;
	CString Right(int num) const;
	void Replace(TCHAR chr,TCHAR subst);
};