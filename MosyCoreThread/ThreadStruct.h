#pragma once
#include "pch.h"
#include "MosySocket.h"
#include "MosyThreadManager.h"
#include "MosyMessageQueue.h"
#include "MosyWebSocketManager.h"

struct ThreadParamStruct
{
	SOCKET TargetSocket;
	MosyThreadManager* ThreadManager;
	MosyMessageQueue* Queue;
	MosyWebSocketManager* WebSocketManager;
};