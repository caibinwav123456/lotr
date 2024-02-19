#include "cstring.h"

CString::CString()
{
}
CString::CString(const TCHAR* str):std::basic_string<TCHAR>(str)
{
}
CString::CString(const CString& other):std::basic_string<TCHAR>(other)
{
}
void CString::operator=(const TCHAR* str)
{
	((std::basic_string<TCHAR>*)this)->operator=(str);
}
void CString::operator=(const CString& str)
{
	((std::basic_string<TCHAR>*)this)->operator=(str);
}
CString::operator const TCHAR*() const
{
	return c_str();
}

CString CString::operator+(const CString& other) const
{
	return (std::operator+(*(std::basic_string<TCHAR>*)this,other)).c_str();
}
CString CString::operator+(const TCHAR* other) const
{
	return (std::operator+(*(std::basic_string<TCHAR>*)this,other)).c_str();
}
void CString::operator+=(const CString& other)
{
	((std::basic_string<TCHAR>*)this)->operator+=(other);
}
void CString::operator+=(const TCHAR* other)
{
	((std::basic_string<TCHAR>*)this)->operator+=(other);
}

bool CString::operator==(const CString& other) const
{
	return std::operator==(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator==(const TCHAR* other) const
{
	return std::operator==(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator!=(const CString& other) const
{
	return std::operator!=(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator!=(const TCHAR* other) const
{
	return std::operator!=(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator<(const CString& other) const
{
	return std::operator<(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator<(const TCHAR* other) const
{
	return std::operator<(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator<=(const CString& other) const
{
	return std::operator<=(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator<=(const TCHAR* other) const
{
	return std::operator<=(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator>(const CString& other) const
{
	return std::operator>(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator>(const TCHAR* other) const
{
	return std::operator>(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator>=(const CString& other) const
{
	return std::operator>=(*(std::basic_string<TCHAR>*)this,other);
}
bool CString::operator>=(const TCHAR* other) const
{
	return std::operator>=(*(std::basic_string<TCHAR>*)this,other);
}

int CString::GetLength() const
{
	return (int)size();
}
CString CString::Left(int num) const
{
	return substr(0,num).c_str();
}
CString CString::Mid(int start,int num) const
{
	return substr(start,num).c_str();
}
CString CString::Right(int num) const
{
	return substr(size()-num).c_str();
}
void CString::Replace(TCHAR chr, TCHAR subst)
{
	std::basic_string<TCHAR>::iterator first=begin(),last=end();
	for(;first<last;first++)
	{
		if(*first==chr)
			*first=subst;
	}
}
