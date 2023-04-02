#include "pch.h"
#include "MosyWebSocketManager.h"
#include "MosyString.h"
#include "MosyValue.h"
#include "MosyLogger.h"
#include "WebSocketPacket.h"

BOOL SocketLock = false;
void MosyWebSocketManager::pushSocket(MosySocket Socket)
{
	// TODO: 在此处添加实现代码.
	SocketArray.push_back(Socket);
}


void MosyWebSocketManager::broadcastSocket(wstring data)
{
	// TODO: 在此处添加实现代码.
	while(SocketLock){}
	SocketLock = true;
	string dataA = MosyString::WString2String(data);
	ByteBuffer buffer;
	buffer.resetoft();
	buffer.append(dataA.c_str(), dataA.size());
	WebSocketPacket* SocketPacket = new WebSocketPacket();
	SocketPacket->set_fin(1);
	SocketPacket->set_rsv1(0);
	SocketPacket->set_rsv2(0);
	SocketPacket->set_rsv3(0);
	SocketPacket->set_opcode(1);
	SocketPacket->set_mask(0);
	SocketPacket->set_payload_length(buffer.length());
	SocketPacket->set_payload(buffer.bytes(), buffer.length());
	ByteBuffer oBuffer;
	oBuffer.resetoft();
	SocketPacket->pack_dataframe(oBuffer);
	for (int i = 0; i < SocketArray.size(); i++) {
		MosySocket* pSocket = &SocketArray[i];
		try
		{
			if (pSocket->HasConnected()) {
				pSocket->Send(oBuffer.bytes(),oBuffer.length());
			}
			else {
				pSocket->Close();
				SocketArray.erase(SocketArray.begin() + i);
				i--;
			}
		}
		catch (MosySocket::MosySocketException e)
		{
			MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
			pSocket->Close();
			SocketArray.erase(SocketArray.begin() + i);
			i--;
		}
		catch (exception e)
		{
			MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
		}
	}
	delete SocketPacket;
	SocketLock = false;
}


void MosyWebSocketManager::releaseSocket(MosySocket Socket)
{
	// TODO: 在此处添加实现代码.
	for (int i = 0; i < SocketArray.size(); i++) {
		MosySocket* pSocket = &SocketArray[i];
		if (pSocket->GetHandler() == Socket.GetHandler()) {
			pSocket->Close();
			SocketArray.erase(SocketArray.begin() + i);
			break;
		}
	}
}


void MosyWebSocketManager::releaseAllSocket()
{
	// TODO: 在此处添加实现代码.
	for (int i = 0; i < SocketArray.size(); i++) {
		MosySocket* pSocket = &SocketArray[i];
		pSocket->Close();
	}
	SocketArray.clear();
}
