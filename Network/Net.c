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
#include "App.h"

// NDK
#include <netmain.h>
#include <servers.h>
#include <console.h>

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
// ���
static HANDLE hData = 0;
static HANDLE hNull = 0;

static HANDLE hEchoTCP = 0;
static HANDLE hEchoUDP = 0;
static HANDLE hOob = 0;

// NDK ���б�־
char NDKFlag;

/* NDK Э��ջ����ʱ���� */
// �ַ���
char *VerStr = "\nCoreKernel DSP Evaluation Application...\r\n";

// ��̬ IP ����
char *HostName        = "CoreKernel";
char *LocalIPAddr     = "0.0.0.0";          // DHCP ģʽ������Ϊ "0.0.0.0"
char StaticIPAddr[16] = "10.0.0.2";         // ������Ĭ�Ͼ�̬ IP
char LocalIPMask[16]  = "255.255.255.0";    // DHCP ģʽ����Ч
char GatewayIP[16]    = "10.0.0.1";         // DHCP ģʽ����Ч
char *DomainName      = "x.corekernel.org"; // DHCP ģʽ����Ч
char *DNSServer       = "10.0.0.1";         // DNS ������

// IP ��ַ
char IPAddress[16];

// DHCP ѡ��
unsigned char DHCP_OPTIONS[] = {DHCPOPT_SERVER_IDENTIFIER, DHCPOPT_ROUTER};

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
static void ServiceReport(unsigned int Item, unsigned int Status, unsigned int Report, HANDLE h);

extern void AddWebFiles();
extern void RemoveWebFiles();

extern void SNTPInit();

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
void NetworkOpen()
{
	// TCP ��׼����
    hData = DaemonNew(SOCK_STREAM, 0, 1000, dtask_tcp_datasrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
    hNull = DaemonNew(SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

	// TCP ������
    hEchoTCP = DaemonNew(SOCK_STREAMNC, 0, 7, dtask_tcp_echo, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

    // TCP OOB ������
    hOob  = DaemonNew(SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

    // UDP ����
    hEchoUDP = DaemonNew(SOCK_DGRAM, 0, 7, dtask_udp_echo, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1);

    // ��Ӿ�̬��ҳ�ļ��� RAM EFS �ļ�ϵͳ
    AddWebFiles();

    // ����Уʱ����
//    SNTPInit();
}

void NetworkClose()
{
	// �رջỰ
	DaemonFree(hOob);
	DaemonFree(hNull);
	DaemonFree(hData);
	DaemonFree(hEchoTCP);
	DaemonFree(hEchoUDP);

	// �رտ���̨
    ConsoleClose();

    // �Ƴ� WEB �ļ�
    RemoveWebFiles();
}

/****************************************************************************/
/*                                                                          */
/*              IP ��ַ���                                                 */
/*                                                                          */
/****************************************************************************/
void NetworkIPAddr(IPN IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
	ConsoleWrite("\r\n");

	if(fAdd)
	{
		ConsoleWrite("Network Added: ");
	}
	else
	{
		ConsoleWrite("Network Removed: ");
	}

	NtIPN2Str(IPAddr, IPAddress);
	ConsoleWrite("%s\r\n", IPAddress);
}

/****************************************************************************/
/*                                                                          */
/*              DHCP ����                                                   */
/*                                                                          */
/****************************************************************************/
void DHCPReset(unsigned int IfIdx, unsigned int fOwnTask)
{
    CI_SERVICE_DHCPC dhcpc;
    HANDLE h;
    int rc, tmp;
    unsigned int idx;

    // ������´����������߳��б�����
    // �ȴ�ʵ���������
    if(fOwnTask)
    {
        TaskSleep(500);
    }

    // �ڽӿ��ϼ��� DHCP ����
    for(idx = 1; ; idx++)
    {
        // Ѱ�� DHCP ʵ��
        rc = CfgGetEntry(0, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, idx, &h);
        if(rc != 1)
        {
            goto RESET_EXIT;
        }

        // ȡ�� DHCP ʵ������
        tmp = sizeof(dhcpc);
        rc = CfgEntryGetData(h, &tmp, (UINT8 *)&dhcpc);

        if((rc<=0) || dhcpc.cisargs.IfIdx != IfIdx)
        {
            CfgEntryDeRef(h);
            h = 0;

            continue;
        }

        // �Ƴ���ǰ DHCP ����
        CfgRemoveEntry(0, h);

        // �ڵ�ǰ�ӿ����� DHCP ����
        bzero(&dhcpc, sizeof(dhcpc));
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = IfIdx;
        dhcpc.cisargs.pCbSrv = &ServiceReport;
        CfgAddEntry(0, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0, sizeof(dhcpc), (UINT8 *)&dhcpc, 0);

        break;
    }

RESET_EXIT:
    if(fOwnTask)
    {
        TaskExit();
    }
}

/****************************************************************************/
/*                                                                          */
/*              DHCP ״̬                                                   */
/*                                                                          */
/****************************************************************************/
void DHCPStatus()
{
    char IPString[16];
    IPN IPAddr;
    int i, rc;

    // ɨ�� DHCPOPT_SERVER_IDENTIFIER ����
    ConsoleWrite("DHCP Server ID:\n");
    for(i = 1; ; i++)
    {
        // ��ȡ DNS ����
        rc = CfgGetImmediate(0, CFGTAG_SYSINFO, DHCPOPT_SERVER_IDENTIFIER, i, 4, (UINT8 *)&IPAddr);
        if(rc != 4)
        {
            break;
        }

        // IP ��ַ
        NtIPN2Str(IPAddr, IPString);
        ConsoleWrite("DHCP Server %d = '%s'\n", i, IPString);
    }

    if(i == 1)
    {
    	ConsoleWrite("None\n\n");
    }
    else
    {
    	ConsoleWrite("\n");
    }

    // ɨ�� DHCPOPT_ROUTER ����
    ConsoleWrite("Router Information:\n");
    for(i = 1; ; i++)
    {
        // ��ȡ DNS ����
        rc = CfgGetImmediate(0, CFGTAG_SYSINFO, DHCPOPT_ROUTER, i, 4, (UINT8 *)&IPAddr);
        if(rc != 4)
        {
            break;
        }

        // IP ��ַ
        NtIPN2Str(IPAddr, IPString);
        ConsoleWrite("Router %d = '%s'\n", i, IPString);
    }

    if(i == 1)
    {
    	ConsoleWrite("None\n\n");
    }
    else
    {
    	ConsoleWrite("\n");
    }
}

/****************************************************************************/
/*                                                                          */
/*              ����״̬                                                    */
/*                                                                          */
/****************************************************************************/
char *TaskName[]  = {"Telnet", "HTTP", "NAT", "DHCPS", "DHCPC", "DNS"};
char *ReportStr[] = {" ", "Running", "Updated", "Complete", "Fault"};
char *StatusStr[] = {"Disabled", "Waiting", "IPTerm", "Failed", "Enabled"};

void ServiceReport(unsigned int Item, unsigned int Status, unsigned int Report, HANDLE h)
{
    ConsoleWrite("Service Status: %9s: %9s: %9s: %03d\n",
                 TaskName[Item - 1], StatusStr[Status], ReportStr[Report / 256], Report & 0xFF);

    // ���� DHCP
    if(Item == CFGITEM_SERVICE_DHCPCLIENT &&
       Status == CIS_SRV_STATUS_ENABLED &&
       (Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPADD) ||
        Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPRENEW)))
    {
        IPN IPTmp;

        // ���� DNS
        IPTmp = inet_addr(DNSServer);
        if(IPTmp)
        {
            CfgAddEntry(0, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0);
        }

        // DHCP ״̬
        DHCPStatus();
    }

    // ���� DHCP �ͻ��˷���
    if(Item == CFGITEM_SERVICE_DHCPCLIENT && (Report & ~0xFF) == NETTOOLS_STAT_FAULT)
    {
        CI_SERVICE_DHCPC dhcpc;
        int tmp;

        // ȡ�� DHCP �������(���ݵ� DHCP_reset ����)
        tmp = sizeof(dhcpc);
        CfgEntryGetData(h, &tmp, (UINT8 *)&dhcpc);

        // ���� DHCP ��λ���񣨵�ǰ�������ڻص�������ִ�����Բ���ֱ�ӵ��øú�����
        TaskCreate(DHCPReset, "DHCPReset", OS_TASKPRINORM, 0x1000, dhcpc.cisargs.IfIdx, 1, 0);
    }
}

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
Void NetworkTask(UArg a0, UArg a1)
{
	int rc;

    // ��ʼ������ϵͳ����
    // ������ʹ�� NDK ֮ǰ���ȵ���
    rc = NC_SystemOpen(NC_PRIORITY_HIGH, NC_OPMODE_INTERRUPT);
    if(rc)
    {
    	ConsoleWrite("NC_SystemOpen Failed (%d)\n", rc);

        return;
    }

    // �����µ�����
    HANDLE hCfg;
    hCfg = CfgNew();
    if(!hCfg)
    {
    	ConsoleWrite("Unable to create configuration\n");

        goto Exit;
    }

    // ����������
    if(strlen( DomainName ) >= CFG_DOMAIN_MAX || strlen( HostName ) >= CFG_HOSTNAME_MAX)
    {
    	ConsoleWrite("Names too long\n");

        goto Exit;
    }

    // ���ȫ���������� hCfg(��������������Ч)
    CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0, strlen(HostName), (UINT8 *)HostName, 0);

    // ��̬��̬ IP ����
    if(a0 == 1)
    {
		CI_IPNET NA;
		CI_ROUTE RT;
		IPN      IPTmp;

		// ���� IP
		bzero(&NA, sizeof(NA));
		NA.IPAddr = inet_addr(StaticIPAddr);
		NA.IPMask = inet_addr(LocalIPMask);
		strcpy(NA.Domain, DomainName);
		NA.NetType = 0;

		// ��ӵ�ַ���ӿ� 1
		CfgAddEntry(hCfg, CFGTAG_IPNET, 1, 0, sizeof(CI_IPNET), (UINT8 *)&NA, 0);

		// ���� Ĭ������
		bzero(&RT, sizeof(RT));
		RT.IPDestAddr = 0;
		RT.IPDestMask = 0;
		RT.IPGateAddr = inet_addr(GatewayIP);

		// ���� ·��
		CfgAddEntry(hCfg, CFGTAG_ROUTE, 0, 0, sizeof(CI_ROUTE), (UINT8 *)&RT, 0);

		// ���� DNS ������
		IPTmp = inet_addr(DNSServer);
		if(IPTmp)
		CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0);

		// ���������Ϣ
		ConsoleWrite("\r\n");
		ConsoleWrite("IP Address %s\n", StaticIPAddr);
		ConsoleWrite("IP Subnet Mask %s\n", LocalIPMask);
		ConsoleWrite("IP Default Gateway %s\n", GatewayIP);

    }
    else if(a0 == 0)
    {
        CI_SERVICE_DHCPC dhcpc;

        // ָ�� DHCP ���������ڽӿ� IF-1
        bzero(&dhcpc, sizeof(dhcpc));
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = 1;
        dhcpc.cisargs.pCbSrv = &ServiceReport;
        dhcpc.param.pOptions = DHCP_OPTIONS;
        dhcpc.param.len = 2;
        CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0, sizeof(dhcpc), (UINT8 *)&dhcpc, 0);
    }

    // ���� TELNET ����
    CI_SERVICE_TELNET telnet;

    bzero(&telnet, sizeof(telnet));
    telnet.cisargs.IPAddr = INADDR_ANY;
    telnet.cisargs.pCbSrv = &ServiceReport;
    telnet.param.MaxCon   = 2;
    telnet.param.Callback = &ConsoleOpen;
    CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_TELNET, 0, sizeof(telnet), (UINT8 *)&telnet, 0);

    /* ���� HTTP ��ط��� */
    // HTTP �����֤
	CI_ACCT CA;

	// ���� HTTP ���������֤��(��󳤶� 31)
	CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_SYSINFO_REALM1, 0, 30, (UINT8 *)"DSP_CLIENT_AUTHENTICATE1", 0 );

	// �����û�
	// �û��� �����Ϊ admin
	strcpy(CA.Username, "admin");
	strcpy(CA.Password, "admin");
	CA.Flags = CFG_ACCTFLG_CH1;  // ��Ϊ realm 1 ��Ա
	rc = CfgAddEntry(hCfg, CFGTAG_ACCT, CFGITEM_ACCT_REALM, 0, sizeof(CI_ACCT), (UINT8 *)&CA, 0);

	// ���� HTTP ����
    CI_SERVICE_HTTP http;
    bzero(&http, sizeof(http));
    http.cisargs.IPAddr = INADDR_ANY;
    http.cisargs.pCbSrv = &ServiceReport;
    CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_HTTP, 0, sizeof(http), (UINT8 *)&http, 0);

    /* ����Э��ջѡ�� */
    // ��ʾ������Ϣ
    rc = DBG_INFO;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // ���� TCP �� UDP buffer ��С��Ĭ�� 8192��
    // TCP ���� buffer ��С
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPTXBUF, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP ���� buffer ��С(copy ģʽ)
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXBUF, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP �������ƴ�С(non-copy ģʽ)
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXLIMIT, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // UDP �������ƴ�С
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKUDPRXLIMIT, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

#if 0
    // TCP Keep Idle(10 ��)
    rc = 100;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPIDLE, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP Keep Interval(1 ��)
    rc = 10;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPINTVL, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP Max Keep Idle(5 ��)
    rc = 50;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPMAXIDLE, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);
#endif

    // ʹ�õ�ǰ�������� NDK ����
    NDKFlag = 1;

    do
    {
        rc = NC_NetStart(hCfg, NetworkOpen, NetworkClose, NetworkIPAddr);
    } while(rc);

    // ֹͣ��Ϣ
    ConsoleWrite("Network Task has been stop(Return Code %d)!\r\n", rc);

    // ɾ������
    CfgFree(hCfg);

    // �˳�
    goto Exit;

Exit:
    NC_SystemClose();

    TaskExit();
}
