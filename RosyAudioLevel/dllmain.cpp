// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "RosyAudioLevelInterface.h"

bool Initialized = false;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!Initialized) {
            if (!InitializeAudioInterface()) {
                MessageBoxW(NULL, L"Rosy Unable to Initialize Audio Interface!", L"Rosy Error", MB_OK | MB_ICONERROR);
            }
            else {
                //MessageBoxW(NULL, L"Rosy Already Initialized Audio Interface!", L"Rosy Alert", MB_OK | MB_ICONINFORMATION);
            }
            Initialized = true;
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

