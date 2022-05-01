/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    NDK 网络协议栈                                                        */
/*                                                                          */
/*    2014年07月03日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include "App.h"

// NDK
#include <netmain.h>
#include <servers.h>
#include <console.h>

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// 句柄
static HANDLE hData = 0;
static HANDLE hNull = 0;

static HANDLE hEchoTCP = 0;
static HANDLE hEchoUDP = 0;
static HANDLE hOob = 0;

// NDK 运行标志
char NDKFlag;

/* NDK 协议栈运行时配置 */
// 字符串
char *VerStr = "\nCoreKernel DSP Evaluation Application...\r\n";

// 静态 IP 配置
char *HostName        = "CoreKernel";
char *LocalIPAddr     = "0.0.0.0";          // DHCP 模式下设置为 "0.0.0.0"
char StaticIPAddr[16] = "10.0.0.2";         // 开发板默认静态 IP
char LocalIPMask[16]  = "255.255.255.0";    // DHCP 模式下无效
char GatewayIP[16]    = "10.0.0.1";         // DHCP 模式下无效
char *DomainName      = "x.corekernel.org"; // DHCP 模式下无效
char *DNSServer       = "10.0.0.1";         // DNS 服务器

// IP 地址
char IPAddress[16];

// DHCP 选项
unsigned char DHCP_OPTIONS[] = {DHCPOPT_SERVER_IDENTIFIER, DHCPOPT_ROUTER};

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
static void ServiceReport(unsigned int Item, unsigned int Status, unsigned int Report, HANDLE h);

extern void AddWebFiles();
extern void RemoveWebFiles();

extern void SNTPInit();

/****************************************************************************/
/*                                                                          */
/*              看守线程                                                    */
/*                                                                          */
/****************************************************************************/
void NetworkOpen()
{
	// TCP 基准测试
    hData = DaemonNew(SOCK_STREAM, 0, 1000, dtask_tcp_datasrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);
    hNull = DaemonNew(SOCK_STREAMNC, 0, 1001, dtask_tcp_nullsrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

	// TCP 服务器
    hEchoTCP = DaemonNew(SOCK_STREAMNC, 0, 7, dtask_tcp_echo, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

    // TCP OOB 服务器
    hOob  = DaemonNew(SOCK_STREAMNC, 0, 999, dtask_tcp_oobsrv, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);

    // UDP 连接
    hEchoUDP = DaemonNew(SOCK_DGRAM, 0, 7, dtask_udp_echo, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1);

    // 添加静态网页文件到 RAM EFS 文件系统
    AddWebFiles();

    // 启动校时服务
//    SNTPInit();
}

void NetworkClose()
{
	// 关闭会话
	DaemonFree(hOob);
	DaemonFree(hNull);
	DaemonFree(hData);
	DaemonFree(hEchoTCP);
	DaemonFree(hEchoUDP);

	// 关闭控制台
    ConsoleClose();

    // 移除 WEB 文件
    RemoveWebFiles();
}

/****************************************************************************/
/*                                                                          */
/*              IP 地址变更                                                 */
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
/*              DHCP 重置                                                   */
/*                                                                          */
/****************************************************************************/
void DHCPReset(unsigned int IfIdx, unsigned int fOwnTask)
{
    CI_SERVICE_DHCPC dhcpc;
    HANDLE h;
    int rc, tmp;
    unsigned int idx;

    // 如果在新创建的任务线程中被调用
    // 等待实例创建完成
    if(fOwnTask)
    {
        TaskSleep(500);
    }

    // 在接口上检索 DHCP 服务
    for(idx = 1; ; idx++)
    {
        // 寻找 DHCP 实例
        rc = CfgGetEntry(0, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, idx, &h);
        if(rc != 1)
        {
            goto RESET_EXIT;
        }

        // 取得 DHCP 实例数据
        tmp = sizeof(dhcpc);
        rc = CfgEntryGetData(h, &tmp, (UINT8 *)&dhcpc);

        if((rc<=0) || dhcpc.cisargs.IfIdx != IfIdx)
        {
            CfgEntryDeRef(h);
            h = 0;

            continue;
        }

        // 移除当前 DHCP 服务
        CfgRemoveEntry(0, h);

        // 在当前接口配置 DHCP 服务
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
/*              DHCP 状态                                                   */
/*                                                                          */
/****************************************************************************/
void DHCPStatus()
{
    char IPString[16];
    IPN IPAddr;
    int i, rc;

    // 扫描 DHCPOPT_SERVER_IDENTIFIER 服务
    ConsoleWrite("DHCP Server ID:\n");
    for(i = 1; ; i++)
    {
        // 获取 DNS 服务
        rc = CfgGetImmediate(0, CFGTAG_SYSINFO, DHCPOPT_SERVER_IDENTIFIER, i, 4, (UINT8 *)&IPAddr);
        if(rc != 4)
        {
            break;
        }

        // IP 地址
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

    // 扫描 DHCPOPT_ROUTER 服务
    ConsoleWrite("Router Information:\n");
    for(i = 1; ; i++)
    {
        // 获取 DNS 服务
        rc = CfgGetImmediate(0, CFGTAG_SYSINFO, DHCPOPT_ROUTER, i, 4, (UINT8 *)&IPAddr);
        if(rc != 4)
        {
            break;
        }

        // IP 地址
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
/*              服务状态                                                    */
/*                                                                          */
/****************************************************************************/
char *TaskName[]  = {"Telnet", "HTTP", "NAT", "DHCPS", "DHCPC", "DNS"};
char *ReportStr[] = {" ", "Running", "Updated", "Complete", "Fault"};
char *StatusStr[] = {"Disabled", "Waiting", "IPTerm", "Failed", "Enabled"};

void ServiceReport(unsigned int Item, unsigned int Status, unsigned int Report, HANDLE h)
{
    ConsoleWrite("Service Status: %9s: %9s: %9s: %03d\n",
                 TaskName[Item - 1], StatusStr[Status], ReportStr[Report / 256], Report & 0xFF);

    // 配置 DHCP
    if(Item == CFGITEM_SERVICE_DHCPCLIENT &&
       Status == CIS_SRV_STATUS_ENABLED &&
       (Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPADD) ||
        Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPRENEW)))
    {
        IPN IPTmp;

        // 配置 DNS
        IPTmp = inet_addr(DNSServer);
        if(IPTmp)
        {
            CfgAddEntry(0, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0);
        }

        // DHCP 状态
        DHCPStatus();
    }

    // 重置 DHCP 客户端服务
    if(Item == CFGITEM_SERVICE_DHCPCLIENT && (Report & ~0xFF) == NETTOOLS_STAT_FAULT)
    {
        CI_SERVICE_DHCPC dhcpc;
        int tmp;

        // 取得 DHCP 入口数据(传递到 DHCP_reset 索引)
        tmp = sizeof(dhcpc);
        CfgEntryGetData(h, &tmp, (UINT8 *)&dhcpc);

        // 创建 DHCP 复位任务（当前函数是在回调函数中执行所以不能直接调用该函数）
        TaskCreate(DHCPReset, "DHCPReset", OS_TASKPRINORM, 0x1000, dhcpc.cisargs.IfIdx, 1, 0);
    }
}

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void NetworkTask(UArg a0, UArg a1)
{
	int rc;

    // 初始化操作系统环境
    // 必须在使用 NDK 之前最先调用
    rc = NC_SystemOpen(NC_PRIORITY_HIGH, NC_OPMODE_INTERRUPT);
    if(rc)
    {
    	ConsoleWrite("NC_SystemOpen Failed (%d)\n", rc);

        return;
    }

    // 创建新的配置
    HANDLE hCfg;
    hCfg = CfgNew();
    if(!hCfg)
    {
    	ConsoleWrite("Unable to create configuration\n");

        goto Exit;
    }

    // 配置主机名
    if(strlen( DomainName ) >= CFG_DOMAIN_MAX || strlen( HostName ) >= CFG_HOSTNAME_MAX)
    {
    	ConsoleWrite("Names too long\n");

        goto Exit;
    }

    // 添加全局主机名到 hCfg(对所有连接域有效)
    CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0, strlen(HostName), (UINT8 *)HostName, 0);

    // 静态或动态 IP 配置
    if(a0 == 1)
    {
		CI_IPNET NA;
		CI_ROUTE RT;
		IPN      IPTmp;

		// 配置 IP
		bzero(&NA, sizeof(NA));
		NA.IPAddr = inet_addr(StaticIPAddr);
		NA.IPMask = inet_addr(LocalIPMask);
		strcpy(NA.Domain, DomainName);
		NA.NetType = 0;

		// 添加地址到接口 1
		CfgAddEntry(hCfg, CFGTAG_IPNET, 1, 0, sizeof(CI_IPNET), (UINT8 *)&NA, 0);

		// 配置 默认网关
		bzero(&RT, sizeof(RT));
		RT.IPDestAddr = 0;
		RT.IPDestMask = 0;
		RT.IPGateAddr = inet_addr(GatewayIP);

		// 配置 路由
		CfgAddEntry(hCfg, CFGTAG_ROUTE, 0, 0, sizeof(CI_ROUTE), (UINT8 *)&RT, 0);

		// 配置 DNS 服务器
		IPTmp = inet_addr(DNSServer);
		if(IPTmp)
		CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0);

		// 输出配置信息
		ConsoleWrite("\r\n");
		ConsoleWrite("IP Address %s\n", StaticIPAddr);
		ConsoleWrite("IP Subnet Mask %s\n", LocalIPMask);
		ConsoleWrite("IP Default Gateway %s\n", GatewayIP);

    }
    else if(a0 == 0)
    {
        CI_SERVICE_DHCPC dhcpc;

        // 指定 DHCP 服务运行在接口 IF-1
        bzero(&dhcpc, sizeof(dhcpc));
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = 1;
        dhcpc.cisargs.pCbSrv = &ServiceReport;
        dhcpc.param.pOptions = DHCP_OPTIONS;
        dhcpc.param.len = 2;
        CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0, sizeof(dhcpc), (UINT8 *)&dhcpc, 0);
    }

    // 配置 TELNET 服务
    CI_SERVICE_TELNET telnet;

    bzero(&telnet, sizeof(telnet));
    telnet.cisargs.IPAddr = INADDR_ANY;
    telnet.cisargs.pCbSrv = &ServiceReport;
    telnet.param.MaxCon   = 2;
    telnet.param.Callback = &ConsoleOpen;
    CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_TELNET, 0, sizeof(telnet), (UINT8 *)&telnet, 0);

    /* 配置 HTTP 相关服务 */
    // HTTP 身份认证
	CI_ACCT CA;

	// 命名 HTTP 服务身份认证组(最大长度 31)
	CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_SYSINFO_REALM1, 0, 30, (UINT8 *)"DSP_CLIENT_AUTHENTICATE1", 0 );

	// 创建用户
	// 用户名 密码均为 admin
	strcpy(CA.Username, "admin");
	strcpy(CA.Password, "admin");
	CA.Flags = CFG_ACCTFLG_CH1;  // 成为 realm 1 成员
	rc = CfgAddEntry(hCfg, CFGTAG_ACCT, CFGITEM_ACCT_REALM, 0, sizeof(CI_ACCT), (UINT8 *)&CA, 0);

	// 配置 HTTP 服务
    CI_SERVICE_HTTP http;
    bzero(&http, sizeof(http));
    http.cisargs.IPAddr = INADDR_ANY;
    http.cisargs.pCbSrv = &ServiceReport;
    CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_HTTP, 0, sizeof(http), (UINT8 *)&http, 0);

    /* 配置协议栈选项 */
    // 显示警告消息
    rc = DBG_INFO;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // 设置 TCP 和 UDP buffer 大小（默认 8192）
    // TCP 发送 buffer 大小
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPTXBUF, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP 接收 buffer 大小(copy 模式)
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXBUF, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP 接收限制大小(non-copy 模式)
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXLIMIT, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // UDP 接收限制大小
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKUDPRXLIMIT, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

#if 0
    // TCP Keep Idle(10 秒)
    rc = 100;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPIDLE, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP Keep Interval(1 秒)
    rc = 10;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPINTVL, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);

    // TCP Max Keep Idle(5 秒)
    rc = 50;
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_TCPKEEPMAXIDLE, CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0);
#endif

    // 使用当前配置启动 NDK 网络
    NDKFlag = 1;

    do
    {
        rc = NC_NetStart(hCfg, NetworkOpen, NetworkClose, NetworkIPAddr);
    } while(rc);

    // 停止消息
    ConsoleWrite("Network Task has been stop(Return Code %d)!\r\n", rc);

    // 删除配置
    CfgFree(hCfg);

    // 退出
    goto Exit;

Exit:
    NC_SystemClose();

    TaskExit();
}
