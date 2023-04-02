#pragma once
#include "MosyThreadManager.h"
#include "MosyMessageQueue.h"
#include "MosyModuleManager.h"
#include "MosyWebSocketManager.h"

struct CoreStruct {
	MosyThreadManager* ThreadManager;
	MosyMessageQueue* MessageQueue;
	MosyModuleManager* ModuleManager;
	MosyWebSocketManager* WebSocketManager;
};

struct CoreSubSenderStruct {
	MosyThreadManager* ThreadManager;
	MosyMessageQueue* MessageQueue;
	int Index;
};