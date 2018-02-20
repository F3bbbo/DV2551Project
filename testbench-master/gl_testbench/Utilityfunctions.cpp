#include <iostream>
#include <Windows.h>

wchar_t *LPCtransferfromstring(const char * title)
{
	size_t size = mbsrtowcs(NULL, &title, 0, NULL);
	wchar_t * buf = new wchar_t[size + 1]();
	size = mbsrtowcs(buf, &title, size + 1, NULL);
	return buf;
}

std::wstring convert(const std::string& as)
{
	// deal with trivial case of empty string
 	if (as.empty())    return std::wstring();

	// determine required length of new string
	size_t reqLength = ::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0);

	// construct new string of required length
	std::wstring ret(reqLength, L'\0');

	// convert old string to new string
	::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length());

	// return new string ( compiler should optimize this away )
	return ret;
}