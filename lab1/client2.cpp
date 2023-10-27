
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")   //socket��
using namespace std;

//#define ServIp "10.130.101.102" //Ĭ�Ϸ����IP
#define PORT 65500  //�˿ں�
#define msgSize 1024  //��������С

SOCKET Client; //����ͻ���socket

#define _WINSOCK_DEPRECATED_NO_WARNINGS

DWORD WINAPI recvThread() //������Ϣ�߳�
{
	while (true)
	{
		char msg[msgSize] = {};//������Ϣ������
		if (recv(Client, msg, sizeof(msg), 0) > 0)//�������ͻ����׽��֣�Ҫ���͵Ļ���������Ϣ������һ�������ĳ��ȣ���־
		{
			cout << endl << msg << endl;
		}
		else if (recv(Client, msg, sizeof(msg), 0) < 0)
		{
			cout << "����״̬�쳣" << endl;
			break;
		}
	}
	Sleep(10);
	return 0;
}

int main()
{
	//��ʼ�� WinSock ��
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != -1) {
		cout << "�ɹ���ʼ��" << endl;
	}

	//����һ��client1�׽���
	Client = socket(AF_INET, SOCK_STREAM, 0);
	/*
   AF_INET��ʹ��ipv4
   SOCK_STREAM���׽������ͣ�ȷ�������ݵ������ԺͿɿ��ԣ���ʽ����
   ����0�Զ�ѡ��Э�飬����ʹ��TCP
   */

	// ��ȡ����IP��ַ
	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
		cout << "��ȡ��ַʧ��" << endl;
		return -1;
	}
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	struct addrinfo* addrInfo = nullptr;
	if (getaddrinfo(hostName, nullptr, &hints, &addrInfo) != 0) {
		cout << "getaddrinfo ʧ��" << endl;
		return -1;
	}


	char localIp[INET_ADDRSTRLEN];


	for (struct addrinfo* p = addrInfo; p != nullptr; p = p->ai_next) {
		struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);

		if (inet_ntop(AF_INET, &(ipv4->sin_addr), localIp, INET_ADDRSTRLEN) == nullptr) {
			cout << "inet_ntop ʧ��" << endl;
			return -1;
		}

	}
	freeaddrinfo(addrInfo);
	SOCKADDR_IN servAddr; //�����������ַ
	// �󶨷�������ַ
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, localIp, &(servAddr.sin_addr)) != 1) {
		cout << "����˰� failed" << endl;
		return -1;
	}
	else {
		cout << "����˰� success";
		cout << " �˿�Ϊ " << PORT << endl;
	}

	cout << "���ӷ������..." << endl;

	//���������������
	if (connect(Client, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "��������� failed" << endl;
		return -1;
	}
	else
	{
		cout << "��������� success" << endl;
	}


	//������Ϣ�߳�
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvThread, NULL, 0, 0);

	char msg[msgSize] = {};
	cout << "disconnect:" << "quit" << endl;



	while (true)
	{
		cin.getline(msg, sizeof(msg)); // ʹ��getline������ʶ�������Ŀո�
		if (strcmp(msg, "quit") == 0) // ����quit�Ͽ�
		{
			cout << "�����������...";
			cin.get(); // �ȴ��û����������;
			break;

		}
		else if (msg[0] == ' ' || strlen(msg) == 0) // ������Ϊ��ʱ������
		{
			continue;
		}
		send(Client, msg, sizeof(msg), 0); // �����˷���
	}

	//�رտͻ����׽����Լ�WinSock��
	closesocket(Client);
	WSACleanup();

	return 0;
}