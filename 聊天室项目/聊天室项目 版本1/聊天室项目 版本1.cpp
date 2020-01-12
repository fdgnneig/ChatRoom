#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

int main()
{
	// 1. 初始化套接字网络环境
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2),/*指定使用2.2的版本*/
		&wd);
///////////////////////////////////////////////////////////////////////////////////////
	// 2. 创建一个客户端的套接字
	SOCKET hClient = socket(
		AF_INET,/*协议族,AF_INET表示因特网的套接字*/
		SOCK_STREAM,/*套接字类型: SOCK_STREAM表示流式套接字*/
		IPPROTO_TCP/*协议的类型:IPPROTO_TCP表示TCP协议*/
	);
///////////////////////////////////////////////////////////////////////////////////////
	// 3. 连接到服务端
	// 3.1 配置服务端的IP地址和端口
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;/*协议族*/
	// htons
	// host to net short
	// 本机字节序转网络字节序, 按short类型来转
	//设置要连接的服务端的端口号
	addr.sin_port = htons(10086);/*必须用大端序来指定,端口号,端口不能被重复
						使用,如果其他进程已经使用
						了这个端口号,则另外的进程就不能再使用该端口号了*/
						// 服务端具体的IP地址
	addr.sin_addr.S_un.S_addr =
		inet_addr("127.0.0.1");
		//inet_addr("192.168.8.129");//这里因为是客户端，故需要填要连接的服务端的ip
	// 3.2 开始连接，该函数用于建立与服务端的链接，服务端的ip端口信息，保存在sockaddr_in结构体中
	//之后所有涉及数据的收发，全部使用客户端套接字
	connect(
		hClient,//客户端套接字句柄
		(sockaddr*)&addr,//客户端的sockaddr_in结构体，其中保存了需要服务端的ip和端口号
		sizeof(addr));//sockaddr_in结构体的大小
///////////////////////////////////////////////////////////////////////////////////////

	// 4. 收发数据客户端线发送数据后接收数据
	//可以考虑将发送数据和接收数据放在两个线程中
	while (1)
	{
		//这里因为数据的收发是放在同一个缓冲区存储的，所以如果
		//没有正确连接到服务器，输入的数据没有发送出去仍保留在缓冲区，
		//则在死循环中作为服务端发送的数据打印出来
		char buff[100];
		recv(hClient, buff, sizeof(buff), 0);
		printf("服务端说: %s\n", buff);		
		
		printf("> ");
		scanf_s("%s", buff, sizeof(buff));
		// 将控制台输入的内容发送给服务端
		send(hClient,
			buff,
			strlen(buff) + 1, 0);
	}
	// 5. 关闭套接字
	closesocket(hClient);
	// 6. 清空套接字环境.
	WSACleanup();
}