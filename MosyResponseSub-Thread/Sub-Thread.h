#pragma once
#include "pch.h"
#include "MosyMessageQueue.h"
#include "MosyRequestPackage.h"
#include "MosyRequestQueuePackage.h"
#include "MosyResponsePackage.h"
#include "MosyResponseQueuePackage.h"
#include "MosySocket.h"
#include "MosyRequestDeserializer.h"
#include "MosyResponseSerializer.h"
#include "MosyDataPackage.h"
#include "MosyHtmlPackage.h"
#include "MosyDataSerializer.h"
#include "MosyHtmlSerializer.h"
#include "ThreadStruct.h"
#include "MosyLogger.h"
#include "CoreStruct.h"
#include "MosyRequestWrapper.h"
#include "MosyWebSocketManager.h"
#include "sha1.h"
#include "base64.h"
#include <string.h>
#include <sstream>
using namespace std;

DWORD _stdcall InitThread(LPVOID Param)
{
	MosySocket Socket;
	try
	{
		Socket = (((CoreStruct*)Param)->MessageQueue->GetRequestSocket());
		wchar_t str[7];
		wsprintfW(str, L"%d", Socket.GetConnectPort());
		wstring ReceiveData = Socket.Receive();
		MosyRequestPackage RequestPackage = MosyRequestDeserializer::Parse(ReceiveData);
		//MosyLogger::Log(MosyValue(L"Accept Connect from " + Socket.GetConnectAddr() + L" on Port " + str + L" to " + RequestPackage.TargetInterface + L" with Method " + RequestPackage.Method.GetString()));
		MosyRequestQueuePackage qp;
		qp.Package = RequestPackage;
		qp.ClientSocket = Socket.GetHandler();
		MosyRequestQueuePackage RequestQueuePackage = qp;
		MosyRequestPackage RequestPack = RequestQueuePackage.Package;
		MosyModuleManager* ModuleManager = ((CoreStruct*)Param)->ModuleManager;
		MosyWebSocketManager* WebSocketManager = ((CoreStruct*)Param)->WebSocketManager;
		BOOL Upgrade = false;
		try
		{
			if (RequestPackage.RequestParams[L"Connection"].GetString() == L"Upgrade") {
				Upgrade = true;
				if (RequestPackage.Method.GetString() == L"GET" && 
					RequestPackage.RequestParams[L"Upgrade"].GetString() == L"websocket" && 
					RequestPackage.RequestParams[L"Host"].GetString().size() != 0 && 
					RequestPackage.RequestParams[L"Sec-WebSocket-Version"].GetString() == L"13" &&
					RequestPackage.RequestParams[L"Sec-WebSocket-Key"].GetString().size() != 0) {
					wstring AcpCode = RequestPackage.RequestParams[L"Sec-WebSocket-Key"].GetString();
					AcpCode += L"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
					unsigned H[5];
					SHA1 sha1;
					sha1.Input((MosyString::WString2String(AcpCode).c_str()),MosyString::WString2String(AcpCode).size());
					sha1.Result(H); 
					for (int i = 0; i < 5; i++) {
						H[i] = htonl(H[i]);
					}
					try
					{
						wostringstream oss;
						oss << H[0] << H[1] << H[2] << H[3] << H[4];
						string st = base64_encode(reinterpret_cast<const unsigned char*>(H), 20);
						Socket.Send(L"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ");
						Socket.Send(st);
						Socket.Send(L"\r\n\r\n");
						WebSocketManager->pushSocket(Socket);
					}
					catch (MosySocket::MosySocketException e)
					{
						MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
						Socket.Close();
					}
					catch (exception e)
					{
						MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
					}
				}
			}
		}
		catch (exception e)
		{

		}
		if (!Upgrade) {
			try
			{
				MosyResponseQueuePackage qp;
				MosyResponsePackage Response;
				MosyRequestWrapperResult Result = MosyRequestWrapper::GetRequestTarget(RequestPack.TargetInterface, RequestPack.Method);
				if (Result[L"Type"].GetString() == L"RestController")
				{
					MosyEnvironment Environment;
					Environment.insert_or_assign(L"ModuleManager", ModuleManager);
					Environment.insert_or_assign(L"WebSocketManager", WebSocketManager);
					MosyValue v = ModuleManager->ExecuteRestfulController(Result[L"Path"], &Environment, RequestPack.Params);
					MosyDataPackage DataPack;
					DataPack.Data = v.GetAString();
					Response = MosyResponseSerializer::Serialize(MOSY_200, DataPack);
				}
				else if (Result[L"Type"].GetString() == L"ViewController")
				{
					MosyEnvironment Environment;
					Environment.insert_or_assign(L"ModuleManager", ModuleManager);
					Environment.insert_or_assign(L"WebSocketManager", WebSocketManager);
					MosyViewModule v = ModuleManager->ExecuteViewController(Result[L"Path"], &Environment, RequestPack.Params);
					wstring pth = v[L"View"].GetString();
					while (pth.find(L"/") != pth.npos)
					{
						pth = pth.replace(pth.find(L"/"), 1, L"\\");
					}
					MosyHtmlSerializer HtmlSerializer;
					MosyHtmlPackage HtmlPackage = HtmlSerializer.AddFile(pth);
					if (HtmlPackage.Exit)
					{
						Response = MosyResponseSerializer::Serialize(MOSY_200, HtmlPackage);
					}
					else
					{
						Response = MosyResponseSerializer::Serialize(MOSY_404, HtmlPackage);
					}
				}
				else if (Result[L"Type"].GetString() == L"Resource")
				{
					MosyHtmlSerializer HtmlSerializer;
					string Path = MosyString::WString2String(RequestPackage.TargetInterface);
					MosyHtmlPackage HtmlPackage = HtmlSerializer.AddFile(Result[L"Path"].GetString());
					if (HtmlPackage.Exit)
					{
						Response = MosyResponseSerializer::Serialize(MOSY_200, HtmlPackage);
					}
					else
					{
						Response = MosyResponseSerializer::Serialize(MOSY_404, HtmlPackage);
					}
				}
				else if (Result[L"Type"].GetString() == L"ERROR")
				{
					MosyHtmlPackage HtmlPackage;
					HtmlPackage.Exit = false;
					const char* strCharA_ = "<HTML><TITLE>Not Found</TITLE>\r\n<BODY><h1 align='center'>404</h1><br/><h1 align='center'>NOT FOUND.</h1><br/><h1 align='center'>Mosy Server Framework</h1>\r\n</BODY></HTML>\r\n";
					HtmlPackage.Html.insert(HtmlPackage.Html.end(), strCharA_, strCharA_ + strlen(strCharA_));
					HtmlPackage.Length = HtmlPackage.Html.size();
					HtmlPackage.type = L"text/html";
					Response = MosyResponseSerializer::Serialize(MOSY_404, HtmlPackage);
					MosyLogger::Log(Result[L"Msg"]);
				}
				qp.ClientSocket = RequestQueuePackage.ClientSocket;
				qp.Package = Response;
				MosyResponseQueuePackage ResponseQueuePackage = qp;
				MosySocket Socket;
				try
				{
					Socket = ResponseQueuePackage.ClientSocket;
					if (ResponseQueuePackage.Package.Size != 0)
					{
						Socket.Send(ResponseQueuePackage.Package.Head);
						Socket.Send(ResponseQueuePackage.Package.Body, ResponseQueuePackage.Package.Size);
					}
					Socket.Close();
				}
				catch (MosySocket::MosySocketException e)
				{
					MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
					Socket.Close();
				}
				catch (exception e)
				{
					MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
				}
			}
			catch (exception e)
			{
				MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
				MosySocket(RequestQueuePackage.ClientSocket).Close();
			}
		}
	}
	catch (MosySocket::MosySocketException e)
	{
		MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
		Socket.Close();
	}
	catch (exception e)
	{
		MosyLogger::Log(MosyValue(MosyString::String2WString(e.what())));
	}
	return 0;
}