#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
//创建子线程，专门用于处理客户端的消息收发 接收消息是一个阻断函数（recv函数）
//主线程主要用于接收客户发起的链接（accept函数） 
DWORD CALLBACK clientHandler(LPVOID pArg)
{
	SOCKET hClient = (SOCKET)pArg;//形参为客户端的套接字句柄
	while (1)
	{
		//创建缓冲区用于接收客户端消息
		char buff[100] = { 0 };
		//接收客户端消息，参数分别为客户端套接字句柄、缓冲区指针、最大接收数据的字节数、一般填0
		//是阻塞函数，在接收到客户端输入的数据之前不会返回
		recv(hClient, buff, 100, 0);
		printf("客户端>:%s\n", buff);

		printf("> ");
		// 接收控制台输入
		scanf_s("%s", buff, sizeof(buff));
		// 将控制台输入的内容发送给服务端
		//参数分别为，客户端套接字句柄、缓冲区指针、要发送的数据的字节数、一般填0
		send(hClient, buff, strlen(buff) + 1, 0);
	}
	return 0;
}

int main()
{
	// 1. 初始化套接字环境
	//该结构体需要作为参数向WSAStartup传递socket版本信息
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2),/*指定使用2.2的版本*/
		&wd);

	////////////////////////////////////////////////////////////////	
	// 2. 创建一个服务端的套接字
	SOCKET hServer = socket(
		AF_INET,/*协议族,AF_INET表示因特网的套接字*/
		SOCK_STREAM,/*套接字类型: SOCK_STREAM表示流式套接字*/
		IPPROTO_TCP/*协议的类型:IPPROTO_TCP表示TCP协议*/
	);

	////////////////////////////////////////////////////////////////
	// 3. 将服务端的套接字和本机
	//    IP及端口绑定
	//定义sockaddr_in类的结构体，用于向绑定函数bind()传递要绑定的IP地址的端口号
	sockaddr_in addr = { 0 };
	//设置该结构体的协议族字段
	addr.sin_family = AF_INET;/*协议族*/
	// htons
	// host to net short
	// 本机字节序转网络字节序, 按short类型来转
	//设置结构体端口号
	addr.sin_port = htons(10086);/*必须用大端序来指定,端口号,端口不能被重复
								 使用,如果其他进程已经使用
								 了这个端口号,则另外的进程就不能再使用该端口号了*/
	addr.sin_addr.S_un.S_addr =
		inet_addr("0.0.0.0"); /*0.0.0.0表示本机所有的网卡的ip地址，服务器的套接字绑定本机ip即可*/
	bind(hServer,//客户端套接字句柄
		(sockaddr*)&addr,/*要绑定的地址结构体*/
		sizeof(addr)/*地址结构体占用的字节数*/);

	////////////////////////////////////////////////////////////////
	// 4. 将服务端套接字进入监听状态
	listen(hServer, SOMAXCONN);//这里listen的第二个参数为该端口最多可以开放的线程

	////////////////////////////////////////////////////////////////
	// 5. 等待客户端的连接
	while (1)
	{
		//定义sockaddr_in结构体用于保存客户端的IP地址和端口号
		sockaddr_in clientAddr = { 0 };
		//socketaddr_in结构体的大小
		int nAddrSize = sizeof(clientAddr);
		//定义一个socket句柄，用于接收客户端的套接字句柄
		SOCKET hClient = INVALID_SOCKET;
		// accept是一个阻塞性函数, 
		// 它会一直等待客户端连接进来
		// 连接进来之后,函数会将客户端的信息
		// 获取出来:
		// 1. 客户端的套接字句柄
		// 1. 客户端的地址
		hClient = accept(
			hServer,/*通过服务端套接字来等待客户端的连接*/
			(sockaddr*)&clientAddr,/*[输出]连接进来的客户端的IP地址*/
			&nAddrSize/*[输入]客户端的地址的大小*/);

		printf("客户端上线: %d.%d.%d.%d\n",//输出客户端的IP地址
			clientAddr.sin_addr.S_un.S_un_b.s_b1,
			clientAddr.sin_addr.S_un.S_un_b.s_b2,
			clientAddr.sin_addr.S_un.S_un_b.s_b3,
			clientAddr.sin_addr.S_un.S_un_b.s_b4);

		////////////////////////////////////////////////////////////////////
		// 6. 和客户端收发数据
		//    一个服务端可能会有几个客户端连接
		//   要给哪个客户端发送数据,就使用哪个客户端
		//   端的套接字.
		send(hClient, "欢迎连接", 9, 0);

		//创建一个新进程，用于于客户端持续收发数据
		//这个函数既可以打开一个线程，也可以创建一个线程，如果相应线程之前没有被创建过，该函数会创建
		//如果相应线程之前被创建过，该函数会打开
		//所以这里服务端可以支持多个客户端的链接，并为每一个客户端创建收发消息的线程
		CreateThread(0, 0,
			clientHandler,//新线程的回调函数
			(LPVOID)hClient,//将客户端的套接字句柄作为附加参数传给新线程的回调函数
			0,
			0);
	}
	// 7. 关闭套接字
	closesocket(hServer);
	// 8. 清空套接字环境.
	WSACleanup();
}