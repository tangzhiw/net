
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")   //socket库
using namespace std;

//#define ServIp "10.130.101.102" //默认服务端IP
#define PORT 65500  //端口号
#define msgSize 1024  //缓冲区大小

SOCKET Client; //定义客户端socket

#define _WINSOCK_DEPRECATED_NO_WARNINGS

DWORD WINAPI recvThread() //接收消息线程
{
	while (true)
	{
		char msg[msgSize] = {};//接收消息缓冲区
		if (recv(Client, msg, sizeof(msg), 0) > 0)//参数：客户端套接字，要发送的缓冲区（信息），上一个参数的长度，标志
		{
			cout << endl << msg << endl;
		}
		else if (recv(Client, msg, sizeof(msg), 0) < 0)
		{
			cout << "连接状态异常" << endl;
			break;
		}
	}
	Sleep(10);
	return 0;
}

int main()
{
	//初始化 WinSock 库
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != -1) {
		cout << "成功初始化" << endl;
	}

	//创建一个client1套接字
	Client = socket(AF_INET, SOCK_STREAM, 0);
	/*
   AF_INET：使用ipv4
   SOCK_STREAM：套接字类型，确保了数据的完整性和可靠性，流式传输
   参数0自动选择协议，这里使用TCP
   */

	// 获取本机IP地址
	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
		cout << "获取地址失败" << endl;
		return -1;
	}
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	struct addrinfo* addrInfo = nullptr;
	if (getaddrinfo(hostName, nullptr, &hints, &addrInfo) != 0) {
		cout << "getaddrinfo 失败" << endl;
		return -1;
	}


	char localIp[INET_ADDRSTRLEN];


	for (struct addrinfo* p = addrInfo; p != nullptr; p = p->ai_next) {
		struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);

		if (inet_ntop(AF_INET, &(ipv4->sin_addr), localIp, INET_ADDRSTRLEN) == nullptr) {
			cout << "inet_ntop 失败" << endl;
			return -1;
		}

	}
	freeaddrinfo(addrInfo);
	SOCKADDR_IN servAddr; //定义服务器地址
	// 绑定服务器地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, localIp, &(servAddr.sin_addr)) != 1) {
		cout << "服务端绑定 failed" << endl;
		return -1;
	}
	else {
		cout << "服务端绑定 success";
		cout << " 端口为 " << PORT << endl;
	}

	cout << "连接服务端中..." << endl;

	//向服务器发起请求
	if (connect(Client, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "服务端连接 failed" << endl;
		return -1;
	}
	else
	{
		cout << "服务端连接 success" << endl;
	}


	//创建消息线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvThread, NULL, 0, 0);

	char msg[msgSize] = {};
	cout << "disconnect:" << "quit" << endl;



	while (true)
	{
		cin.getline(msg, sizeof(msg)); // 使用getline，可以识别到输入间的空格
		if (strcmp(msg, "quit") == 0) // 输入quit断开
		{
			cout << "按任意键继续...";
			cin.get(); // 等待用户按下任意键;
			break;

		}
		else if (msg[0] == ' ' || strlen(msg) == 0) // 当输入为空时不发送
		{
			continue;
		}
		send(Client, msg, sizeof(msg), 0); // 向服务端发送
	}

	//关闭客户端套接字以及WinSock库
	closesocket(Client);
	WSACleanup();

	return 0;
}