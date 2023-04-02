#pragma once
#include "pch.h"
#include <Windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <io.h>
#include <iostream>
#include "MosyString.h"
#include "MosyWebSocketManager.h"
#include <shlobj_core.h>
#include <strsafe.h>
#include <shellapi.h>//SHGetFileInfo
#include <ShObjIdl.h>//IShellFolder::GetAttributesOf
#include <gdiplus.h>
#include <Commctrl.h>
#include "ModuleTemplates.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "comctl32.lib")
using namespace std;

MosyEnvironment Env;
int Initialize(MosyEnvironment* pEnv) {
	Env = (*pEnv);
	printf("initialize Module\n");
	return 0;
}


HICON GetFileIconHandle(string strFileName, BOOL bSmallIcon)
{

	SHFILEINFO    sfi;
	/*if (bSmallIcon)
	{
		SHGetFileInfo(
			(LPCTSTR)strFileName.c_str(),
			FILE_ATTRIBUTE_NORMAL,
			&sfi,
			sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	}
	else*/
	{
		SHGetFileInfo(
			(LPCTSTR)strFileName.c_str(),
			FILE_ATTRIBUTE_NORMAL,
			&sfi,
			sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
		BOOL ret;
		//ret = SHGetFileInfo((LPCTSTR)strFileName.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_ATTRIBUTES);
		static IImageList* imageList = NULL;
		static bool bInit = false;
		if (!bInit && imageList == NULL)
		{
			bInit = true;
			IID IID_IImageList = { 0 };
			HRESULT hr = IIDFromString(L"{46EB5926-582E-4017-9FDF-E8998DAA0950}", &IID_IImageList);
			//ASSERT(SUCCEEDED(hr));
			SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)(&imageList));
		}
		ret = SHGetFileInfo((LPCTSTR)strFileName.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SYSICONINDEX);
		if (ret) {
			HICON hIcon = sfi.hIcon;
			if (imageList)
			{
				hIcon = ImageList_GetIcon((HIMAGELIST)imageList, sfi.iIcon, 0);
				if (hIcon)
					DestroyIcon(sfi.hIcon);
				else
					hIcon = sfi.hIcon;
			}
			return hIcon;
		}
	}
	return sfi.hIcon;


}
HICON GetFolderIconHandle(BOOL bSmallIcon)
{
	SHFILEINFO    sfi;
	if (bSmallIcon)
	{
		SHGetFileInfo(
			(LPCTSTR)"Doesn't matter",
			FILE_ATTRIBUTE_DIRECTORY,
			&sfi,
			sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	}
	else
	{
		SHGetFileInfo(
			(LPCTSTR)"Does not matter",
			FILE_ATTRIBUTE_DIRECTORY,
			&sfi,
			sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
	}
	return sfi.hIcon;
}

string dEncode(const char* Data, int DataByte)
{
	//编码表  
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值  
	string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码  
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0, size = 0;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	bool found = false;
	for (UINT ix = 0; !found && ix < num; ++ix)
	{
		if (_wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[ix].Clsid;
			found = true;
			break;
		}
	}

	free(pImageCodecInfo);
	return found;
}

Gdiplus::Bitmap* ScaleBitmap(Gdiplus::Bitmap* pBitmap, UINT nWidth, UINT nHeight)
{
	Gdiplus::Bitmap* pTemp = new Gdiplus::Bitmap(nWidth, nHeight, pBitmap->GetPixelFormat());
	if (pTemp)
	{
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(pTemp);
		if (g)
		{
			// use the best interpolation mode
			g->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g->DrawImage(pBitmap, 0, 0, nWidth, nHeight);
			delete g;
		}
	}
	return pTemp;
}

bool SaveHIconToPngFile(HICON hIcon, LPCWSTR lpszPicFileName)
{
	if (hIcon == NULL)
	{
		return false;
	}

	ICONINFO icInfo = { 0 };
	if (!::GetIconInfo(hIcon, &icInfo))
	{
		return false;
	}

	BITMAP bitmap;
	GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

	Gdiplus::Bitmap* pBitmap = NULL;
	Gdiplus::Bitmap* pWrapBitmap = NULL;

	do
	{
		if (bitmap.bmBitsPixel != 32)
		{
			pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
		}
		else
		{
			pWrapBitmap = Gdiplus::Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
			if (!pWrapBitmap)
				break;

			Gdiplus::BitmapData bitmapData;
			Gdiplus::Rect rcImage((256 - pWrapBitmap->GetWidth()) / 2, (256 - pWrapBitmap->GetHeight()) / 2, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());

			pWrapBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData);
			pBitmap = new (Gdiplus::Bitmap)(bitmapData.Width, bitmapData.Height, bitmapData.Stride, PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
			pWrapBitmap->UnlockBits(&bitmapData);
		}

		CLSID encoderCLSID;
		GetEncoderClsid(L"image/png", &encoderCLSID);
		Gdiplus::Status st = pBitmap->Save(lpszPicFileName, &encoderCLSID, NULL);
		if (st != Gdiplus::Ok)
			break;

	} while (false);

	delete pBitmap;
	if (pWrapBitmap)
		delete pWrapBitmap;
	DeleteObject(icInfo.hbmColor);
	DeleteObject(icInfo.hbmMask);

	return true;
}

string getLnkFormPath(const wchar_t* lnkPath)
{
	// 初始化
	string sRet;
	char wRet[MAX_PATH];

	// 初始化 COM 库
	CoInitialize(NULL);
	IPersistFile* pPF = NULL;

	// 创建 COM 对象
	HRESULT hr = CoCreateInstance(
		CLSID_ShellLink,			// CLSID
		NULL,						// IUnknown 接口指针
		CLSCTX_INPROC_SERVER,		// CLSCTX_INPROC_SERVER：以 Dll 的方式操作类对象 
		IID_IPersistFile,			// COM 对象接口标识符
		(void**)(&pPF)				// 接收 COM 对象的指针
	); if (FAILED(hr)) { cout << "CoCreateInstance failed." << endl; }

	// 判断是否支持接口
	IShellLink* pSL = NULL;
	hr = pPF->QueryInterface(
		IID_IShellLink,				// 接口 IID
		(void**)(&pSL)				// 接收指向这个接口函数虚标的指针
	); if (FAILED(hr)) { cout << "QueryInterface failed." << endl; }

	// 打开文件
	hr = pPF->Load(
		lnkPath,					// 文件全路径
		STGM_READ					// 访问模式：只读
	); if (FAILED(hr)) { cout << "Load failed ：" << GetLastError() << endl; }

	// 获取 Shell 链接来源
	hr = pSL->GetPath(wRet, MAX_PATH, NULL, 0);
	sRet = wRet;

	// 关闭 COM 库
	pPF->Release();
	CoUninitialize();

	return sRet;
}

vector<string> MenuList;
vector<string> ExeList;
void TraverseFolder(LPCSTR lpPath, LPCTSTR lpAbs, vector<string>* pV)
{
	TCHAR szFind[MAX_PATH] = { _T("\0") };
	TCHAR szFind1[MAX_PATH] = { _T("\0") };
	WIN32_FIND_DATA findFileData;
	BOOL bRet;

	_tcscpy_s(szFind, MAX_PATH, lpPath);
	_tcscat_s(szFind, _T("\\*.*"));        //这里一定要指明通配符，不然不会读取所有文件和目录

	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	//遍历文件夹
	while (TRUE)
	{
		if (findFileData.cFileName[0] != _T('.'))
		{//不是当前路径或者父目录的快捷方式
			//_tprintf(_T("%s\\%s\n"), lpAbs, findFileData.cFileName);
			string cs = findFileData.cFileName;
			if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )&& cs.find("User Pinned")==string::npos)
			{//这是一个普通目录
				//设置下一个将要扫描的文件夹路径
				_tcscpy_s(szFind, MAX_PATH, lpPath);
				_tcscat_s(szFind, _T("\\"));
				_tcscat_s(szFind, findFileData.cFileName);

				_tcscpy_s(szFind1, MAX_PATH, lpAbs);
				_tcscat_s(szFind1, _T("\\"));
				_tcscat_s(szFind1, findFileData.cFileName);
				///_tcscat_s(szNextDir, _T("\\*"));
				//遍历该目录
				TraverseFolder(szFind, szFind1, pV);
			}
			else if (cs.find(".lnk") != string::npos && cs.find("uninstall") == string::npos && cs.find("Uninstall") == string::npos)
			{
				_tprintf(_T("%s\\%s\n"), lpAbs, findFileData.cFileName);
				_tcscpy_s(szFind, MAX_PATH, lpPath);
				_tcscat_s(szFind, _T("\\"));
				_tcscat_s(szFind, findFileData.cFileName);

				_tcscpy_s(szFind1, MAX_PATH, lpAbs);
				_tcscat_s(szFind1, strlen(lpAbs)==0?_T(""):_T("\\"));
				_tcscat_s(szFind1, findFileData.cFileName);
				ExeList.push_back(szFind);
				string stt = "{\"AbsPath\":\"";
				stt += szFind;
				stt += "\",\"Path\":\"";
				stt += szFind1;
				stt += "\",\"Img\":\"";
				_tcscpy_s(szFind1, MAX_PATH, _T("DesktopIcons"));
				//_tcscat_s(szFind1, lpAbs);
				_tcscat_s(szFind1, _T("/"));
				_tcscat_s(szFind1, to_string(pV->size()).c_str());
				_tcscat_s(szFind1, _T(".png"));
				stt += szFind1;
				stt += "\",\"Idx\":";
				stt += to_string(pV->size());
				stt += "}";
				CoInitialize(NULL);
				Gdiplus::GdiplusStartupInput gdiplusStartupInput;
				ULONG_PTR gdiplusToken;
				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
				_tcscpy_s(szFind1, MAX_PATH, _T(".\\RosyDesktop\\DesktopIcons"));
				//_tcscat_s(szFind1, lpAbs);
				_tcscat_s(szFind1, _T("\\"));
				_tcscat_s(szFind1, to_string(pV->size()).c_str());
				_tcscat_s(szFind1, _T(".png"));
				pV->push_back(stt);
				SaveHIconToPngFile(GetFileIconHandle(szFind, false), MosyString::String2WString(szFind1).c_str());
				Gdiplus::GdiplusShutdown(gdiplusToken);
				CoUninitialize();
				//HICON hICON = GetFileIconHandle(szFind, false);
				//HBITMAP hBITMAPcopy;
				//ICONINFOEX IconInfo;
				//BITMAP BM_32_bit_color;
				//BITMAP BM_1_bit_mask;

				//// 1. From HICON to HBITMAP for color and mask separately
				////.cbSize required
				////memset((void*)&IconInfo, 0, sizeof(ICONINFOEX));
				//IconInfo.cbSize = sizeof(ICONINFOEX);
				//GetIconInfoEx(hICON, &IconInfo);


				////HBITMAP IconInfo.hbmColor is 32bit per pxl, however alpha bytes can be zeroed or can be not.
				////HBITMAP IconInfo.hbmMask is 1bit per pxl

				//// 2. From HBITMAP to BITMAP for color
				////    (HBITMAP without raw data -> HBITMAP with raw data)
				////         LR_CREATEDIBSECTION - DIB section will be created,
				////         so .bmBits pointer will not be null
				//hBITMAPcopy = (HBITMAP)CopyImage(IconInfo.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
				////    (HBITMAP to BITMAP)
				//GetObject(hBITMAPcopy, sizeof(BITMAP), &BM_32_bit_color);
				////Now: BM_32_bit_color.bmBits pointing to BGRA data.(.bmWidth * .bmHeight * (.bmBitsPixel/8))

				//// 3. From HBITMAP to BITMAP for mask
				//hBITMAPcopy = (HBITMAP)CopyImage(IconInfo.hbmMask, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
				//GetObject(hBITMAPcopy, sizeof(BITMAP), &BM_1_bit_mask);
				////Now: BM_1_bit_mask.bmBits pointing to mask data (.bmWidth * .bmHeight Bits!)
				////BM_1_bit_mask.bmHeight* BM_1_bit_mask.bmWidthBytes;
				//

			}
		}
		//如果是当前路径或者父目录的快捷方式，或者是普通目录，则寻找下一个目录或者文件
		bRet = ::FindNextFile(hFind, &findFileData);
		if (!bRet)
		{//函数调用失败
			//cout << "FindNextFile failed, error code: " 
			//    << GetLastError() << endl;
			break;
		}
	}

	::FindClose(hFind);
}

MosyRestfulResult LoadStartMenu(MosyEnvironment Environment, MosyControllerParams Params)
{
	//ViewControllerTemplate Controller = ((MosyModuleManager*)Environment[L"ModuleManager"])->LoadViewController(MosyValue(L"TestView"));
	//MosyViewModule ViewModule = (*Controller)(Environment, Params);
	//keybd_event(VK_RWIN, 0, 0, 0);
	//keybd_event('E', 0, 0, 0);
	//keybd_event('E', 0, KEYEVENTF_KEYUP, 0);
	//keybd_event(VK_RWIN, 0, KEYEVENTF_KEYUP, 0);
	wchar_t csStartMenu0[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, 0, 0, csStartMenu0);
	WIN32_FIND_DATAW ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFileW(csStartMenu0, &ffd);
	wstring ws = csStartMenu0;
	//ws += L"\\Programs";
	const wchar_t* csStartMenu = ws.c_str();
	TraverseFolder(MosyString::WString2String(ws).c_str(), "", &MenuList);
	//TraverseFolder("C:\\Users\\Moss\\AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch", "", &MenuList);
	//TraverseFolder("shell:::{AD1405D2-30CF-4877-8468-1EE1C52C759F}\Apps", "", &MenuList);
	string resData = "[";
	for (int i = 0; i < MenuList.size() - 1; i++) {
		resData += MenuList[i];
		resData += ",";
	}
	if (MenuList.size() > 0) {
		resData += MenuList[MenuList.size() - 1];
		resData += "]";
	}
	return MosyRestfulResult(MosyString::String2WString(resData));
}

MosyRestfulResult ExecuteStartMenu(MosyEnvironment Environment, MosyControllerParams Params)
{
	if (Params[L"Target"].GetInteger() < MenuList.size()) {
		string stt = ExeList[Params[L"Target"].GetInteger()];
		HINSTANCE hRslt = ShellExecute(NULL, _T("open"),
			stt.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	return MosyRestfulResult(MosyString::String2WString("{\"Success\":true}"));
}

MosyRestfulResult GetStartMenu(MosyEnvironment Environment, MosyControllerParams Params)
{
	string resData = "[";
	for (int i = 0; i < MenuList.size() - 1; i++) {
		resData += MenuList[i];
		resData += ",";
	}
	if (MenuList.size() > 0) {
		resData += MenuList[MenuList.size() - 1];
		resData += "]";
	}
	return MosyRestfulResult(MosyString::String2WString(resData));
}