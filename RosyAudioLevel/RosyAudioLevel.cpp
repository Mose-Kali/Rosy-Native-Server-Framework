#include "pch.h"
#include "ModuleTemplates.h"
#include "RosyAudioLevelInterface.h"
#include "MosyWebSocketManager.h"


DWORD _stdcall SenderThread(LPVOID param) {
	MosyWebSocketManager* smgr = (MosyWebSocketManager*)param;
	while (true) {
		smgr->broadcastSocket(L"{\"Status\":\"ready\",\"left\":" + CaptureAudioData(1) + L",\"right\":" + CaptureAudioData(2) + L",\"center\":" + CaptureAudioData(-1) + L"}");
		Sleep(100);
	}
}

MosyEnvironment Env;
int Initialize(MosyEnvironment* pEnv) {
	Env = (*pEnv);
	MosyWebSocketManager* smgr = ((MosyWebSocketManager*)Env[L"WebSocketManager"]);
	CreateThread(NULL, NULL, SenderThread, (LPVOID)smgr, NULL, NULL);
	return 0;
}


MosyRestfulResult RosyAudioLevel(MosyEnvironment Environment, MosyControllerParams Params)
{
	int nFlag = Params[L"Flag"].GetInteger();
	return MosyValue(L"{\"Status\":\"ready\"}");
}

MosyRestfulResult RosyAudioControlNext(MosyEnvironment Environment, MosyControllerParams Params)
{
	keybd_event(VK_MEDIA_NEXT_TRACK, 0, 0, 0);
	keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_KEYUP, 0);
	return MosyRestfulResult(L"{}");
}

MosyRestfulResult RosyAudioControlPause(MosyEnvironment Environment, MosyControllerParams Params)
{
	keybd_event(VK_MEDIA_PLAY_PAUSE, 0, 0, 0);
	keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_KEYUP, 0);
	return MosyRestfulResult(L"{}");
}

MosyRestfulResult RosyAudioControlPrevious(MosyEnvironment Environment, MosyControllerParams Params)
{
	keybd_event(VK_MEDIA_PREV_TRACK, 0, 0, 0);
	keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_KEYUP, 0);
	return MosyRestfulResult(L"{}");
}