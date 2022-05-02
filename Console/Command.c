/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
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
#include <App.h>

#include "cmdline.h"

/****************************************************************************/
/*                                                                          */
/*              命令                                                        */
/*                                                                          */
/****************************************************************************/
int Cmd_Help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    UARTprintf("\r\nAvailable commands\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");

    pEntry = &g_sCmdTable[0];

     while(pEntry->pcCmd)
    {
        // 输出命令字及说明
        UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        pEntry++;
    }

    return 0;
}

int Cmd_LED(int argc, char *argv[])
{
	char LED_Status = 0;

    if(!strcmp(argv[2], "on"))
    {
    	LED_Status = 1;
    }
    else if(!strcmp(argv[2], "off"))
    {
    	LED_Status = 0;
    }

    int led, LEGGPIO;
    sscanf(argv[1], "%d", &led);

    switch(led)
    {
        case 0: LEGGPIO = SOM_LED2; break;
        case 1: LEGGPIO = SOM_LED3; break;
        case 2: LEGGPIO = EVM_LED3; break;
        case 3: LEGGPIO = EVM_LED4; break;

        default:
            LEGGPIO = SOM_LED2;
    }

    LEDControl(LEGGPIO, LED_Status);

    return 0;
}

int Cmd_SPIFlash(int argc, char *argv[])
{
    UARTprintf("\r\nSPI Flash Test Application\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");
    SPIFlashTest();
    return 0;
}

int Cmd_EEPROM(int argc, char *argv[])
{
    UARTprintf("\r\nEEPROM Test Application\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");
    EEPROMTest();
    return 0;
}

int Cmd_OV2640(int argc, char *argv[])
{
	if(!strcmp(argv[1], "on"))
	{
		UARTprintf("\r\nVPIF OV2640 Test Application\r\n");
		UARTprintf("--------------------------------------------------------------\r\n");
//		MainTaskStatus = OV2640_ON;
//		Semaphore_post();
	}
	else if(!strcmp(argv[1], "off"))
	{
//		MainTaskStatus = OV2640_OFF;
//		Semaphore_post();
	}

    return 0;
}

int Cmd_PRULoad(int argc, char *argv[])
{
    UARTprintf("\r\nPRU LED Test Application\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");

    return 0;
}

char buff[64*1024];
int Cmd_DDR2(int argc, char *argv[])
{
    UARTprintf("\r\nDDR2 Test Application\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");

    DSP_core_MEM_Test((unsigned int) buff, (unsigned int) buff + 64*1024, 1);

    return 0;
}

int Cmd_EHRPWM(int argc, char *argv[])
{
	unsigned int pwm_clk, duty_ratio;

    if(!strcmp(argv[1], "on"))
    {
    	// 关闭触摸屏中断
    	EventCombiner_disableEvent(SYS_INT_GPIO_B2INT);

    	sscanf(argv[2], "%d", &pwm_clk);
    	sscanf(argv[3], "%d", &duty_ratio);
    	PWMInit(pwm_clk, duty_ratio);
    }
    else if(!strcmp(argv[1], "off"))
    {
        PWMInit(0, 0);
//
//    	// 打开触摸屏中断
//    	EventCombiner_enableEvent(SYS_INT_GPIO_B2INT);
//    	// 还原引脚为 spi片选
//		// SP1 片选管脚复用配置
//    	SPI1CSPinMuxSet(1);
    }

    return 0;
}

int Cmd_SATA(int argc, char *argv[])
{
    UARTprintf("\r\nSATA Test Application\r\n");
    UARTprintf("--------------------------------------------------------------\r\n");

    SATATest();

    return 0;
}

int Cmd_Buzzer(int argc, char *argv[])
{
    ConsoleWrite("Beep...\n");

    if((!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {

    }
    else
    {
        int t;
        sscanf(argv[1], "%d", &t);
        BUZZERBeep(t);
    }

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              时间服务                                                    */
/*                                                                          */
/****************************************************************************/
int Cmd_Date(int argc, char *argv[])
{
    if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {

    }
    else if(!strcmp(argv[1], "-s") || !strcmp(argv[1], "--set"))
    {
//        extern struct tm RTCTime;
//        RTCTime.tm_isdst = -1;
//
//        sscanf(argv[2], "%4d%2d%2d", &time.tm_year, &time.tm_mon, &time.tm_mday);
//        sscanf(argv[3], "%2d%2d%2d", &time.tm_hour, &time.tm_min, &time.tm_sec);
//
//        time.tm_year -= 1900;
//        time.tm_mon -= 1;
//
//        unsigned int t;
//        t = mktime(&time);
//      Seconds_set(t);
    }
    else
    {
//      unsigned int t;
//      t = Seconds_get();

//      struct tm *time = localtime(&t);

//        ConsoleWrite("%s\n", asctime(RTCTime));
    }

    return 0;
}

int Cmd_RTC(int argc, char *argv[])
{
    extern struct tm RTCTime;

    if((!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {

    }
    else if(!strcmp(argv[1], "-s") || !strcmp(argv[1], "--set") && argc == 4)
    {
        RTCTime.tm_isdst = -1;

        sscanf(argv[2], "%4d%2d%2d", &RTCTime.tm_year, &RTCTime.tm_mon, &RTCTime.tm_mday);
        sscanf(argv[3], "%2d%2d%2d", &RTCTime.tm_hour, &RTCTime.tm_min, &RTCTime.tm_sec);

        // 计算星期
        // 使用基姆拉尔森计算公式
        int y = RTCTime.tm_year, m = RTCTime.tm_mon, d = RTCTime.tm_mday;
        if(RTCTime.tm_mon == 1 || RTCTime.tm_mon==2)
        {
            m = (m == 1 ? 13 : 14);
            y = y - 1;
        }

        RTCTime.tm_wday = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;

        RTCSet();

        RTCTime.tm_year -= 1900;
        RTCTime.tm_mon -= 1;
    }

    ConsoleWrite("%s\n", asctime(&RTCTime));

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              WIFI                                                        */
/*                                                                          */
/****************************************************************************/
int Cmd_WIFI(int argc, char *argv[])
{
    if((!strcmp(argv[1], "-e") || !strcmp(argv[1], "--exit")))
    {
        UART1Puts("+++", -1);
    }
    else
    {
        UART1Puts(argv[1], -1);
        UART1Puts("\r\n", -1);
    }

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              网络                                                        */
/*                                                                          */
/****************************************************************************/
// 静态网络参数配置
extern char StaticIPAddr[16];
extern char LocalIPMask[16];
extern char GatewayIP[16];
extern char NDKFlag;
extern int NDKCfg;

extern Void NetworkTask(UArg a0, UArg a1);

int Cmd_NET(int argc, char *argv[])
{
    // EMAC 初始化
//    EMACInit();

    // 输出参数
    char i;
    for(i = 1; i < argc; i++)
    {
        ConsoleWrite("parameter: %s\n", argv[i]);
    }

    // DHCP 或静态配置
    /* NDK 任务 */
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.priority = 5;
    TaskParams.instance->name = "Network";
    TaskParams.stackSize = 1024 * 1024;


    if(!strcmp(argv[1], "dhcp"))
    {
        if(NDKFlag)
        {
            ConsoleWrite("NDK Stack is already running!\n");
        }
        else
        {
            TaskParams.arg0 = 0;
            Task_create(NetworkTask, &TaskParams, NULL);
        }
    }
    else if(!strcmp(argv[1], "static") && argc == 5)
    {
        if(NDKFlag)
        {
            ConsoleWrite("NDK Stack is already running!\n");
        }
        else
        {
            TaskParams.arg0 = 1;

            strcpy(StaticIPAddr, argv[2]);
            strcpy(LocalIPMask, argv[3]);
            strcpy(GatewayIP, argv[4]);

            Task_create(NetworkTask, &TaskParams, NULL);
        }
    }
    else if(!strcmp(argv[1], "exit"))
    {
//        NC_NetStop(0);
        Task_sleep(1000);

//      NC_SystemClose();

        NDKFlag = 0;
    }
    else
    {
        ConsoleWrite("Unknown parameter\n");
    }

    Task_sleep(100);

    return 0;
}

// 命令字表
tCmdLineEntry g_sCmdTable[] =
{
    {"help",         Cmd_Help,       "\t\t - Display list of commands [help]"},
    {"h",            Cmd_Help,       "\t\t - Display list of commands [help]"},

    {"led",          Cmd_LED,        "\t\t - LED control  [led+2/3/4/5+on/off]"},
    {"spiflash",     Cmd_SPIFlash,   "\t\t - SPI Flash Test  [spiflash]"},
    {"eeprom",       Cmd_EEPROM,     "\t\t - EEPROM Test  [eeprom]"},

    {"ddr2",         Cmd_DDR2,       "\t\t - DDR2 Test  [ddr2]"},
    {"ov2640",       Cmd_OV2640,     "\t\t - VPIF OV2640 Test  [tl2640+on/off]"},
    {"ehrpwm",       Cmd_EHRPWM,     "\t\t - EHRPWM Test  [ehrpwm+on+'freq(Hz)'+'duty ratio(%)'] [ehrpwm+off]"},
    {"sata",         Cmd_SATA,       "\t\t - SATA Test  [sata]"},

    {"pru",          Cmd_PRULoad,    "\t\t - Load PRU program.[pruled]"},

    {"buzzer",       Cmd_Buzzer,     "\t\t - Beep a while."},

    {"date",         Cmd_Date,       "\t\t - Get/Set System Date or Time."},
    {"hwclock",      Cmd_RTC,        "\t\t - Get/Set RTC Date or Time."},
    {"clock",        Cmd_RTC,        "\t\t - Get/Set RTC Date or Time."},

    {"wifi",         Cmd_WIFI,       "\t\t - WIFI."},

    { "net",         Cmd_NET,        "\t\t Config the NDK stack"},

    { 0, 0, 0 }
};
