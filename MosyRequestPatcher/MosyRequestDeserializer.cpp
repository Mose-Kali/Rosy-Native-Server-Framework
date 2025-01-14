#include "pch.h"
#include "MosyRequestDeserializer.h"
#ifndef DLL_FILE
#define DLL_FILE
#endif
#include <iostream>
#include <string>
#include <string_view>
using namespace std;

std::wstring UrlDecode(std::wstring& SRC)
{
	std::wstring ret;
	char ch;
	int ii;
	for (size_t i = 0; i < SRC.length(); i++) {
		if (int(SRC[i]) == 37) {
			swscanf_s(SRC.substr(i + 1, 2).c_str(), L"%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
		else {
			ret += SRC[i];
		}
	}
	return (ret);
}

static unsigned char dec_tab[256] = {
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
			0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

/**
 * URL 解码函数
 * @param str {const char*} 经URL编码后的字符串
 * @return {char*} 解码后的字符串，返回值不可能为空，需要用 free 释放
 */
char* acl_url_decode(const char* str) {
	int len = (int)strlen(str);
	char* tmp = (char*)malloc(len + 1);

	int i = 0, pos = 0;
	for (i = 0; i < len; i++) {
		if (str[i] != '%')
			tmp[pos] = str[i];
		else if (i + 2 >= len) {  /* check boundary */
			tmp[pos++] = '%';  /* keep it */
			if (++i >= len)
				break;
			tmp[pos] = str[i];
			break;
		}
		else if (isalnum(str[i + 1]) && isalnum(str[i + 2])) {
			tmp[pos] = (dec_tab[(unsigned char)str[i + 1]] << 4)
				+ dec_tab[(unsigned char)str[i + 2]];
			i += 2;
		}
		else
			tmp[pos] = str[i];

		pos++;
	}

	tmp[pos] = '\0';
	return tmp;
}

static const char safe[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
std::string decode(const std::string& uri)
{
	//Note from RFC1630:  "Sequences which start with a percent sign  
	//but are not followed by two hexadecimal characters (0-9,A-F) are reserved  
	//for future extension"  
	const unsigned char* ptr = (const unsigned char*)uri.c_str();
	string ret;
	ret.reserve(uri.length());
	for (; *ptr; ++ptr)
	{
		if (*ptr == '%')
		{
			if (*(ptr + 1))
			{
				char a = *(ptr + 1);
				char b = *(ptr + 2);
				if (!((a >= 0x30 && a < 0x40) || (a >= 0x41 && a < 0x47))) continue;
				if (!((b >= 0x30 && b < 0x40) || (b >= 0x41 && b < 0x47))) continue;
				char buf[3];
				buf[0] = a;
				buf[1] = b;
				buf[2] = 0;
				ret += (char)strtoul(buf, NULL, 16);
				ptr += 2;
				continue;
			}
		}
		if (*ptr == '+')
		{
			ret += ' ';
			continue;
		}
		ret += *ptr;
	}
	return ret;
}


MosyRequestPackage MosyRequestDeserializer::Parse(std::wstring str)
{
	if (str == L"")
	{
		return MosyRequestPackage();
	}
	int index = str.find(L" ");
	wstring method = str.substr(0, index);
	MosyRequestPackage Package;
	//Parse Method
	Package.Method = method;
	int index2 = str.find(L" ", index + 1);
	wstring url = str.substr(index + 1, index2 - index);
	index = url.find(L"?");
	//Get Params In URL
	if (index != url.npos)
	{
		wstring yrl_only = url.substr(0, index);
		Package.TargetInterface = UrlDecode(yrl_only);
		wstring param = url.substr(index + 1, url.length());
		int idx = 0;
		int pindex = param.find(L"&");
		while (pindex != param.npos)
		{
			int l = pindex - idx;
			wstring deng = param.substr(idx, l);
			int namel = deng.find(L"=");
			wstring name = deng.substr(0, namel);
			wstring value = deng.substr(namel + 1, deng.length());
			Package.Params.insert_or_assign(name, MosyValue(value));
			idx = pindex + 1;
			pindex = param.find(L"&", idx);
		}
		int l = pindex - idx;
		wstring d = param.substr(idx, param.length());
		int nam = d.find(L"=");
		wstring na = d.substr(0, nam);
		wstring val = d.substr(nam + 1, d.length());
		string sss = MosyString::WString2String(val);
		//sss = UTF8_To_string(sss);
		val = MosyString::String2WString(acl_url_decode(sss.c_str()));
		Package.Params.insert_or_assign(na, MosyValue(val));
	}
	else
	{
		Package.TargetInterface = MosyString::String2WString(acl_url_decode(MosyString::WString2String(url).c_str()));
	}
	//Parse Submit Mode
	int modeindex = str.find(L"Content-Type: ");
	if (modeindex != str.npos)
	{
		wstring body = str.substr(modeindex, str.length());
		modeindex = body.find(L"Content-Type: ");
		int fen = body.find(L";");
		if (fen == body.npos)
		{
			fen = body.find(L"\r\n");
		}
		modeindex += 14;
		wstring mode = body.substr(modeindex, fen - modeindex);
		if (mode == L"multipart/form-data")
		{
			int s1 = body.find(L"=");
			int s2 = body.find(L"\r\n", s1 + 1);
			wstring sp = body.substr(s1 + 1, s2 - s1 - 2);

			int b1 = body.find(L"Content-Disposition: form-data; name=\"");
			b1 += 38;
			int b2 = body.find(L"\"", b1 + 1);
			int b3 = body.find(sp, b2 + 1);
			while (b1 != body.npos&&b2 != body.npos&&b3 != body.npos)
			{
				wstring n = body.substr(b1, b2 - b1);
				b2 += 2;
				wstring value = body.substr(b2, b3 - b2);
				Package.Params.insert_or_assign(n, MosyValue(value));
				b1 = body.find(L"Content-Disposition: form-data; name=\"", b3 - 2);
				if (b1 == body.npos)
				{
					break;
				}
				b1 += 38;
				b2 = body.find(L"\"", b1 + 1);
				b3 = body.find(sp, b2 + 1);
			}
		}
		else if (mode == L"application/x-www-form-urlencoded")
		{
			int dstart = body.find_last_of(L"\r\n");
			dstart += 1;
			wstring bdy = body.substr(dstart, body.length());
			int idx = 0;
			int pindex = bdy.find(L"&");
			while (pindex != bdy.npos)
			{
				int l = pindex - idx;
				wstring deng = bdy.substr(idx, l);
				int namel = deng.find(L"=");
				wstring name = deng.substr(0, namel);
				wstring value = deng.substr(namel + 1, deng.length());
				Package.Params.insert_or_assign(name, MosyValue(value));
				idx = pindex + 1;
				pindex = bdy.find(L"&", idx);
			}
			int l = pindex - idx;
			wstring d = bdy.substr(idx, bdy.length());
			int nam = d.find(L"=");
			wstring na = d.substr(0, nam);
			wstring val = d.substr(nam + 1, d.length());
			Package.Params.insert_or_assign(na, MosyValue(val));
		}
		else if (mode == L"text/plain")
		{
			int ixd = body.find(L"Content-Length: ");
			if (ixd != body.npos)
			{
				ixd += 16;

				int start = body.find(L"\r\n", ixd);
				start += 4;
				wstring stt = body.substr(start, body.length());
				Package.Params.insert_or_assign(L"RawData", MosyValue(stt));
			}
		}
	}
	//Parse Connection Params
	int cpos = str.find(L"\r\n");
	while (str.find(L"\r\n", cpos + 1) != wstring::npos) {
		int ParamEndIndex = str.find(L"\r\n", cpos + 1);
		if (cpos >= str.find(L"\r\n\r\n")) {
			break;
		}
		wstring ParamString = str.substr(cpos + 2, ParamEndIndex - cpos - 2);
		int ParamSplitIndex = ParamString.find(L": ");
		wstring ParamName = ParamString.substr(0, ParamSplitIndex);
		wstring ParamValue = ParamString.substr(ParamSplitIndex + 2);
		Package.RequsetParams.insert(pair<wstring, wstring>(ParamName, ParamValue));
		cpos = ParamEndIndex;
	}
	return Package;
}

MosyRequestPackage MosyRequestDeserializer::Parse(std::string str)
{
	int index = str.find(" ");
	string method = str.substr(0, index);
	MosyRequestPackage Package;
	//Parse Method
	/*{
		if (method == "GET")
		{
			Package.Method = MOSY_GET;
		}
		else if (method == "POST")
		{
			Package.Method = MOSY_POST;
		}
		else if (method == "PUT")
		{
			Package.Method = MOSY_PUT;
		}
		else if (method == "PATCH")
		{
			Package.Method = MOSY_PATCH;
		}
		else if (method == "DELETE")
		{
			Package.Method = MOSY_DELETE;
		}
		else if (method == "COPY")
		{
			Package.Method = MOSY_COPY;
		}
		else if (method == "HEAD")
		{
			Package.Method = MOSY_HEAD;
		}
		else if (method == "OPTIONS")
		{
			Package.Method = MOSY_OPTIONS;
		}
		else if (method == "LINK")
		{
			Package.Method = MOSY_LINK;
		}
		else if (method == "UNLINK")
		{
			Package.Method = MOSY_UNLINK;
		}
		else if (method == "PURGE")
		{
			Package.Method = MOSY_PURGE;
		}
		else if (method == "LOCK")
		{
			Package.Method = MOSY_LOCK;
		}
		else if (method == "UNLOCK")
		{
			Package.Method = MOSY_UNLOCK;
		}
		else if (method == "PROPFIND")
		{
			Package.Method = MOSY_PROPFIND;
		}
		else if (method == "VIEW")
		{
			Package.Method = MOSY_VIEW;
		}
		else
		{
			return MosyRequestPackage();
		}
	}*/
	Package.Method = MosyString::String2WString(method);
	int index2 = str.find(" ", index + 1);
	string url = str.substr(index + 1, index2 - index);
	index = url.find("?");
	//Get Params In URL
	if (index != url.npos)
	{
		string yrl_only = url.substr(0, index - 1);
		Package.TargetInterface = MosyString::String2WString(yrl_only);
		string param = url.substr(index + 1, url.length());
		int idx = 0;
		int pindex = param.find("&");
		while (pindex != param.npos)
		{
			string deng = param.substr(idx, pindex - idx - 1);
			int namel = deng.find("=");
			string name = deng.substr(0, namel - 1);
			string value = deng.substr(namel, deng.length());
			Package.Params.insert_or_assign(MosyString::String2WString(name), MosyValue(MosyString::String2WString(value)));
			idx = pindex + 1;
			pindex = param.find("&", idx);
		}
		string deng = param.substr(idx, pindex - idx - 1);
		int namel = deng.find("=");
		string name = deng.substr(0, namel - 1);
		string value = deng.substr(namel, deng.length());
		Package.Params.insert_or_assign(MosyString::String2WString(name), MosyValue(MosyString::String2WString(value)));
	}
	//Parse Submit Mode
	int modeindex = str.find("Content-Type: ");
	string body = str.substr(modeindex, str.length());
	modeindex = body.find("Content-Type: ");
	int fen = body.find(";");
	modeindex += 15;
	string mode = body.substr(modeindex, fen - modeindex);
	if (mode == "multipart/form-data")
	{
		int s1 = body.find("=");
		int s2 = body.find("\n", s1 + 1);
		string sp = body.substr(s1 + 1, s2 - s1 - 1);

		int b1 = body.find("Content-Disposition: form-data; name=\"");
		int b2 = body.find("\"", b1 + 1);
		int b3 = body.find(sp, b2);
		while (b1 != body.npos)
		{
			string n = body.substr(b1, b2 - b1);
			b2 += 2;
			string value = body.substr(b2, b3 - b2 - 1);
			Package.Params.insert_or_assign(MosyString::String2WString(n), MosyValue(MosyString::String2WString(value)));
			b1 = body.find("Content-Disposition: form-data; name=\"", b3);
			b2 = body.find("\"", b1 + 1);
			b3 = body.find(sp, b2);
		}
	}
	return Package;
}