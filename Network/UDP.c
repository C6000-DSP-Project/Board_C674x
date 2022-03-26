/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    NDK ����Э��ջ                                                        */
/*                                                                          */
/*    2014��07��03��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include <netmain.h>

#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              �ص����� UDP Server Daemon                                  */
/*                                                                          */
/****************************************************************************/
int UDPServer(SOCKET s, UINT32 unused)
{
	struct sockaddr_in sinDst;
	struct timeval to;
	int i,tmp;
	char *pBuf;
	char Title[] = "UDP Message�� ";

	HANDLE hBuffer;

    // ��ȡ�ͻ�����Ϣ
    struct sockaddr_in addr;
    int len = sizeof(addr);
    getpeername(s, (PSA)&addr, &len);

    unsigned char *val = (unsigned char *)&addr.sin_addr;
    ConsoleWrite("Client Connected IP: %u.%u.%u.%u Port: %d\n", val[0], val[1], val[2], val[3], ntohs(addr.sin_port));

    // ���ó�ʱʱ�� 5s
    to.tv_sec  = 5;
    to.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    for(;;)
    {
        tmp = sizeof(sinDst);
        i = (int)recvncfrom(s, (void **)&pBuf, 0, (PSA)&sinDst, &tmp, &hBuffer);

        // ����һ���̶��ַ��� ���� 28
        sendto(s, &Title, 28, 0, (PSA)&sinDst, sizeof(sinDst));
        // �ش����յ�������
        if(i >= 0)
        {
            sendto(s, pBuf, i, 0, (PSA)&sinDst, sizeof(sinDst));
            recvncfree(hBuffer);
        }
        else
        {
            break;
        }
    }

    // ��������
    return(1);
}

/****************************************************************************/
/*                                                                          */
/*              NDK �ص�����                                                */
/*                                                                          */
/****************************************************************************/
static HANDLE hUdp = 0;

void NetOpenHook()
{
    // ����һ�������� �˿� 1025
    hUdp = DaemonNew(SOCK_DGRAM, 0, 1025, UDPServer, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1);
}

void NetCloseHook()
{
    DaemonFree(hUdp);
}

char StrIP[16];

void NetGetIPAddr(IPN IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    if(fAdd)
    {
        UARTprintf("Network Added: ", IfIdx);
    }
    else
    {
        UARTprintf("Network Removed: ", IfIdx);
    }

    NtIPN2Str(IPAddr, StrIP);
    UARTprintf("%s\r\n", StrIP);
}
