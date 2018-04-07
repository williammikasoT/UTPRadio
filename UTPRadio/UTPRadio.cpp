// UTPRadio.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		cout << "sock error" << endl;
		return -1;
	}
	const int opt = -1;
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));//�����׽�������
	if (nb == -1)
	{
		cout << "set socket error...\n" << endl;
		return -1;
	}
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);//�׽��ֵ�ַΪ�㲥��ַ
	addrto.sin_port = htons(6000);//�׽��ֹ㲥�˿ں�Ϊ6000
	int nlen = sizeof(addrto);
	while (1)
	{
		sleep(1);
		char msg[] = { "the message broadcast" };
		int ret = sendto(sock, msg, strlen(msg), 0, (sockaddr*)&addrto, nlen);//��㲥��ַ������Ϣ
		if (ret<0)
		{
			cout << "send error...\n" << endl;
			return -1;
		}
		else
		{
			printf("ok\n");
		}
	}
	return 0;
}