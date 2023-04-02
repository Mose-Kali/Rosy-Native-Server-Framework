#pragma once
#include "MosySocket.h"
#include <string>
#include <vector>
using namespace std;
class _declspec(dllexport) MosyWebSocketManager
{
	vector<MosySocket> SocketArray;
public:
	void pushSocket(MosySocket Socket);
	void broadcastSocket(wstring data);
	void releaseSocket(MosySocket Socket);
	void releaseAllSocket();
};

