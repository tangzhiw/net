
#define PORT 65500                     // 端口号
#define msgSize 3096                   // 缓冲区大小
#define cMax 3                    // 最大连接数
#define _CRT_SECURE_NO_WARNINGS         
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
int counterr = 1;
#include <iostream>
#include <cstring>
#include <thread>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // socket库

using namespace std;




SOCKET Clients[cMax];    // 客户端
SOCKET Server;                // 服务器端
SOCKADDR_IN clientAddrs[cMax]; // 客户端地址数组
SOCKADDR_IN serverAddr;             // 定义服务器地址

int currentConnect = 0; // 当前连接的客户数
int connectCondition[cMax] = {0}; // 每一个连接的情况

int isEmpty() // 查询空闲的连接口的索引
{
    for (int i = 0; i < cMax; i++)
    {
        if (connectCondition[i] == 0) // 连接空闲
        {
            return i;
        }
    }
    return -1;
}

DWORD WINAPI ThreadFunction(LPVOID lpParameter) // 线程
{
    int receByt = 0;
    char recvMsg[msgSize]; // 接收缓冲区
    char sendMsg[msgSize]; // 发送缓冲区
    int num = (int)lpParameter; // 当前连接的索引
    // 发送连接成功的提示消息
    snprintf(sendMsg, sizeof(sendMsg), "你的id是:%d", Clients[num]); 
    send(Clients[num], sendMsg, strlen(sendMsg), 0);
    // 循环接收信息
    while (true)
    {
        Sleep(10); // 延时10ms
        receByt = recv(Clients[num], recvMsg, sizeof(recvMsg), 0); // 接收信息

        // 获取客户端ip地址
        char clientIp[INET_ADDRSTRLEN] = "";
        inet_ntop(AF_INET, &(clientAddrs[num].sin_addr), clientIp, INET_ADDRSTRLEN);
        if (receByt > 0) // 接收成功
        {
            cout << "Client [" << Clients[num]  << " "  << clientIp <<  "] " <<  " 发送:"  << endl << recvMsg  << endl << endl;
            sprintf_s(sendMsg, sizeof(sendMsg), "Id%d发送：%s ", Clients[num], recvMsg); // 格式化发送信息
            
            for (int i = 0; i < cMax; i++) // 将消息同步到所有聊天窗口
            {
                if (connectCondition[i] == 1 && i != num)
                {
                    send(Clients[i], sendMsg, strlen(sendMsg), 0); // 发送信息
                }
            }
        }
        else // 接收失败
        {
                cout << "Client ["  << Clients[num] <<  " "  << clientIp <<  "] 退出 "    << endl << endl;

                closesocket(Clients[num]);
                currentConnect--;
                connectCondition[num] = 0;
                cout << "当前客户端连接数量为: "   << currentConnect << endl << endl;
                return 0;
        }
    }
}

int main()
{
    // 初始化WinSock库
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) == -1) {
        cout << "初始化出错" ;
    }
    else {
        cout << " WinSock已经初始化" << endl;
    }

    Server = socket(AF_INET, SOCK_STREAM, 0);
    /*
    AF_INET：使用ipv4
    SOCK_STREAM：套接字类型，确保了数据的完整性和可靠性，流式传输
    参数0自动选择协议，这里使用TCP
    */

    if (Server == INVALID_SOCKET) // 错误处理
    {
        cout<<"socket创建失败";
        return -1;
    }
    cout << "创建 Socket 成功" << endl;
    
    

    // 绑定服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;   // 地址类型
    serverAddr.sin_port = htons(PORT); // 端口号
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(Server, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) // 将服务器套接字与服务器地址和端口绑定
    {
        perror("绑定失败");
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "端口 "  << PORT <<  " 绑定成功" << endl;
    }

    // 设置监听/等待队列
    if (listen(Server, cMax) != 0)
    {
        perror("监听失败");
        return -1;
    }
    else
    {
        cout << "监听成功" << endl;
    }

    cout << "服务端成功启动" << endl;

    // 循环接收客户端请求
    
    while (true)
    {
        if (currentConnect < cMax)
        {
            if (isEmpty() == -1) {
                exit(1);
            }
            int num = isEmpty();
            int addrlen = sizeof(SOCKADDR);
            Clients[num] = accept(Server, (sockaddr*)&clientAddrs[num], &addrlen); // 等待客户端请求

            // 获取客户端ip地址
            char clientIp[INET_ADDRSTRLEN] = "";
            inet_ntop(AF_INET, &(clientAddrs[num].sin_addr), clientIp, INET_ADDRSTRLEN);

            if (Clients[num] == SOCKET_ERROR)
            {
                cout<<"客户端出错";
                closesocket(Server);
                WSACleanup();
                return -1;
            }
            connectCondition[num] = 1;// 连接位置1表示占用
            currentConnect++; // 当前连接数加1
            
            cout <<  Clients[num] <<  " "  << clientIp <<  " 连接 "    << endl << endl;
            cout << "当前客户端连接数量为: "  << currentConnect <<   endl << endl;

            HANDLE Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, (LPVOID)num, 0, NULL); // 创建线程
            if (Thread == NULL) // 线程创建失败
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
                cout << "客户端数量已满" << endl << endl;
                counterr--;
            }
            
        }
    }

    closesocket(Server);
    WSACleanup();

    return 0;
}