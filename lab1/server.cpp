
#define PORT 65500                     // �˿ں�
#define msgSize 3096                   // ��������С
#define cMax 3                    // ���������
#define _CRT_SECURE_NO_WARNINGS         
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
int counterr = 1;
#include <iostream>
#include <cstring>
#include <thread>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // socket��

using namespace std;




SOCKET Clients[cMax];    // �ͻ���
SOCKET Server;                // ��������
SOCKADDR_IN clientAddrs[cMax]; // �ͻ��˵�ַ����
SOCKADDR_IN serverAddr;             // �����������ַ

int currentConnect = 0; // ��ǰ���ӵĿͻ���
int connectCondition[cMax] = {0}; // ÿһ�����ӵ����

int isEmpty() // ��ѯ���е����ӿڵ�����
{
    for (int i = 0; i < cMax; i++)
    {
        if (connectCondition[i] == 0) // ���ӿ���
        {
            return i;
        }
    }
    return -1;
}

DWORD WINAPI ThreadFunction(LPVOID lpParameter) // �߳�
{
    int receByt = 0;
    char recvMsg[msgSize]; // ���ջ�����
    char sendMsg[msgSize]; // ���ͻ�����
    int num = (int)lpParameter; // ��ǰ���ӵ�����
    // �������ӳɹ�����ʾ��Ϣ
    snprintf(sendMsg, sizeof(sendMsg), "���id��:%d", Clients[num]); 
    send(Clients[num], sendMsg, strlen(sendMsg), 0);
    // ѭ��������Ϣ
    while (true)
    {
        Sleep(10); // ��ʱ10ms
        receByt = recv(Clients[num], recvMsg, sizeof(recvMsg), 0); // ������Ϣ

        // ��ȡ�ͻ���ip��ַ
        char clientIp[INET_ADDRSTRLEN] = "";
        inet_ntop(AF_INET, &(clientAddrs[num].sin_addr), clientIp, INET_ADDRSTRLEN);
        if (receByt > 0) // ���ճɹ�
        {
            cout << "Client [" << Clients[num]  << " "  << clientIp <<  "] " <<  " ����:"  << endl << recvMsg  << endl << endl;
            sprintf_s(sendMsg, sizeof(sendMsg), "Id%d���ͣ�%s ", Clients[num], recvMsg); // ��ʽ��������Ϣ
            
            for (int i = 0; i < cMax; i++) // ����Ϣͬ�����������촰��
            {
                if (connectCondition[i] == 1 && i != num)
                {
                    send(Clients[i], sendMsg, strlen(sendMsg), 0); // ������Ϣ
                }
            }
        }
        else // ����ʧ��
        {
                cout << "Client ["  << Clients[num] <<  " "  << clientIp <<  "] �˳� "    << endl << endl;

                closesocket(Clients[num]);
                currentConnect--;
                connectCondition[num] = 0;
                cout << "��ǰ�ͻ�����������Ϊ: "   << currentConnect << endl << endl;
                return 0;
        }
    }
}

int main()
{
    // ��ʼ��WinSock��
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) == -1) {
        cout << "��ʼ������" ;
    }
    else {
        cout << " WinSock�Ѿ���ʼ��" << endl;
    }

    Server = socket(AF_INET, SOCK_STREAM, 0);
    /*
    AF_INET��ʹ��ipv4
    SOCK_STREAM���׽������ͣ�ȷ�������ݵ������ԺͿɿ��ԣ���ʽ����
    ����0�Զ�ѡ��Э�飬����ʹ��TCP
    */

    if (Server == INVALID_SOCKET) // ������
    {
        cout<<"socket����ʧ��";
        return -1;
    }
    cout << "���� Socket �ɹ�" << endl;
    
    

    // �󶨷�������ַ
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;   // ��ַ����
    serverAddr.sin_port = htons(PORT); // �˿ں�
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(Server, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) // ���������׽������������ַ�Ͷ˿ڰ�
    {
        perror("��ʧ��");
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "�˿� "  << PORT <<  " �󶨳ɹ�" << endl;
    }

    // ���ü���/�ȴ�����
    if (listen(Server, cMax) != 0)
    {
        perror("����ʧ��");
        return -1;
    }
    else
    {
        cout << "�����ɹ�" << endl;
    }

    cout << "����˳ɹ�����" << endl;

    // ѭ�����տͻ�������
    
    while (true)
    {
        if (currentConnect < cMax)
        {
            if (isEmpty() == -1) {
                exit(1);
            }
            int num = isEmpty();
            int addrlen = sizeof(SOCKADDR);
            Clients[num] = accept(Server, (sockaddr*)&clientAddrs[num], &addrlen); // �ȴ��ͻ�������

            // ��ȡ�ͻ���ip��ַ
            char clientIp[INET_ADDRSTRLEN] = "";
            inet_ntop(AF_INET, &(clientAddrs[num].sin_addr), clientIp, INET_ADDRSTRLEN);

            if (Clients[num] == SOCKET_ERROR)
            {
                cout<<"�ͻ��˳���";
                closesocket(Server);
                WSACleanup();
                return -1;
            }
            connectCondition[num] = 1;// ����λ��1��ʾռ��
            currentConnect++; // ��ǰ��������1
            
            cout <<  Clients[num] <<  " "  << clientIp <<  " ���� "    << endl << endl;
            cout << "��ǰ�ͻ�����������Ϊ: "  << currentConnect <<   endl << endl;

            HANDLE Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, (LPVOID)num, 0, NULL); // �����߳�
            if (Thread == NULL) // �̴߳���ʧ��
            {
                cout<<"error";
                return -1;
            }
            else
            {
                CloseHandle(Thread);
                
            }
        }
        else
        {
            if (counterr == 1) {
                cout << "�ͻ�����������" << endl << endl;
                counterr--;
            }
            
        }
    }

    closesocket(Server);
    WSACleanup();

    return 0;
}