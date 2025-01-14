#include "pch.h"
#include "MosyServerApplication.h"
#include "CoreStruct.h"
#include "MosyMessageQueue.h"
#include "MosyWebSocketManager.h"

/*
Written By Moss
  __  __                      __ _
 |  \/  |                    / _| |
 | \  / | ___  ___ ___  ___ | |_| |_
 | |\/| |/ _ \/ __/ __|/ _ \|  _| __|
 | |  | | (_) \__ \__ \ (_) | | | |_
 |_|  |_|\___/|___/___/\___/|_|  \__|
 ====================================
*/

LONG WINAPI MosyUnhandledExceptionFilter(EXCEPTION_POINTERS* exp);

void MosyServerApplication::Boot()
{
	MosyLogger::BootLogo();
	CoreStruct Struct;
	MosyMessageQueue MessageQueue;
	MosyModuleManager ModuleManager;
	MosyThreadManager ThreadManager;
	MosyWebSocketManager WebSocketManager;
	Struct.MessageQueue = &MessageQueue;
	Struct.ThreadManager = &ThreadManager;
	Struct.ModuleManager = &ModuleManager;
	Struct.WebSocketManager = &WebSocketManager;
	MosyGobalManager::ThreadManager.Manager = Struct.ThreadManager;
	HANDLE Receiver = MosyGobalManager::ThreadManager.CreateCoreThread(ReceiverThread, &Struct);
	::SetUnhandledExceptionFilter(MosyUnhandledExceptionFilter);
	if (Receiver == NULL)
	{
		MosyLogger::Log(MosyValue(L"FATAL ERROR: Core Thread(Receiver) Cannot be Started!"));
	}
	else
	{
		WaitForSingleObject(Receiver, INFINITE);
	}
}

LONG WINAPI MosyUnhandledExceptionFilter(EXCEPTION_POINTERS* exp)
{
	MosyLogger::Log(MosyValue(L"Something Error"));
	return EXCEPTION_EXECUTE_HANDLER;
}