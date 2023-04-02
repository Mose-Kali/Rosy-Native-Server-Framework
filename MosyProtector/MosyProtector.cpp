
// MosyProtector.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MosyProtector.h"
#include "MosyProtectorDlg.h"
#include <TlHelp32.h>
#include <string>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)

// CMosyProtectorApp

BEGIN_MESSAGE_MAP(CMosyProtectorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMosyProtectorApp 构造

CMosyProtectorApp::CMosyProtectorApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMosyProtectorApp 对象

CMosyProtectorApp theApp;

DWORD FindProcessIDByName(const std::wstring& processName)//0 not found ; other found; processName "processName.exe"
{
	PROCESSENTRY32 pe32;
	//在使用这个结构前，先设置它的大小
	pe32.dwSize = sizeof(pe32);
	//给系统内所有的进程拍个快照
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}
	//遍历进程快照，轮流显示每个进程的信息
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore) {
		if (pe32.szExeFile == processName)//进程名称
		{
			::CloseHandle(hProcessSnap);
			return pe32.th32ProcessID;//进程ID
		}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	//不要忘记清除掉snapshot对象
	::CloseHandle(hProcessSnap);
	return 0;
}

void CreateProcessor(std::wstring cmd)
{
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };//在产生子进程时，子进程的窗口相关信息
	PROCESS_INFORMATION pi;                  //子进程的ID/线程相关信息
	DWORD returnCode;//用于保存子程进的返回值;

	BOOL bRet = CreateProcessW(              //调用失败，返回0；调用成功返回非0；
		NULL,                               //一般都是空；（另一种批处理情况：此参数指定"cmd.exe",下一个命令行参数 "/c otherBatFile")
		&cmd[0],                       //命令行参数         
		NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
		FALSE,                              //_In_        BOOL                  bInheritHandles,
		CREATE_NEW_CONSOLE,                 //新的进程使用新的窗口。
		NULL,                               //_In_opt_    LPVOID                lpEnvironment,
		NULL,                               //_In_opt_    LPCTSTR               lpCurrentDirectory,
		&si,                                //_In_        LPSTARTUPINFO         lpStartupInfo,
		&pi);                               //_Out_       LPPROCESS_INFORMATION lpProcessInformation
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	//if (bRet) {
	//    std::cout << "process is running..." << std::endl;
	//    //等待子进程结束
	//    WaitForSingleObject(pi.hProcess, -1);
	//    std::cout << "process is finished" << std::endl;
	//    //获取子进程的返回值
	//    GetExitCodeProcess(pi.hProcess, &returnCode);
	//    std::cout << "process return code:" << returnCode << std::endl;
	//}
	//else {
	//    std::cout << "Create Process error!" << std::endl;
	//    return;
	//}
}

// CMosyProtectorApp 初始化

BOOL CMosyProtectorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("MosyProtector"));
	TCHAR TPath[MAX_PATH];
	DWORD dwRet;
	dwRet = GetModuleFileName(NULL, TPath, MAX_PATH);
	USES_CONVERSION;
	std::string StrExe = W2A(TPath);
	int index = StrExe.rfind('\\');
	std::string strPath = StrExe.substr(0, index);
	SetCurrentDirectory(A2W(strPath.c_str()));
	std::wstring TargetProcess;
	FILE* f = freopen(".\\TargetProcess.ini", "r", stdin);
	wchar_t str[1024];
	std::wcin.getline(str, 1024);
	TargetProcess = str;
	for (;;)
	{
		if (!FindProcessIDByName(TargetProcess))
		{
			CreateProcessor(TargetProcess);
		}
		Sleep(1000);
	}
	/*CMosyProtectorDlg dlg;
	m_pMainWnd = &dlg;*/
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “确定”来关闭对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “取消”来关闭对话框的代码
	//}
	//else if (nResponse == -1)
	//{
	//	TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
	//	TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	//}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

