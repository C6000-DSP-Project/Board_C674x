/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    RTC ʵʱʱ��                                                          */
/*                                                                          */
/*    2014��06��05��                                                        */
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
#include <App.h>

#include "rtc.h"

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
struct tm RTCTime;

extern unsigned char LEDNixieVal[4];
extern unsigned char SEGVal[16];

/****************************************************************************/
/*                                                                          */
/*              BCD/ʮ������ת��                                            */
/*                                                                          */
/****************************************************************************/
int bcd2dec(int bcd)
{
    return (bcd - (bcd >> 4) * 6);
}

int dec2bcd(int dec)
{
    return (dec + (dec / 10) * 6);
}

/****************************************************************************/
/*                                                                          */
/*              ��������/ʱ��                                               */
/*                                                                          */
/****************************************************************************/
void RTCSet()
{
    // ��������
    RTCYearSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_year));
    RTCMonthSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_mon));
    RTCDayOfMonthSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_mday));
    RTCDayOfTheWeekSet(SOC_RTC_0_REGS, RTCTime.tm_wday);   // ����

    // ����ʱ��
    RTCHourSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_hour));
    RTCMinuteSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_min));
    RTCSecondSet(SOC_RTC_0_REGS, dec2bcd(RTCTime.tm_sec));
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void RTCHwi(UArg arg)
{
    // ��ȡ����
    RTCTime.tm_year = bcd2dec(RTCYearGet(SOC_RTC_0_REGS));
    RTCTime.tm_mon = bcd2dec(RTCMonthGet(SOC_RTC_0_REGS));
    RTCTime.tm_mday = bcd2dec(RTCDayOfMonthGet(SOC_RTC_0_REGS));
    RTCTime.tm_wday = RTCDayOfTheWeekGet(SOC_RTC_0_REGS);  // ����

    // ��ȡʱ��
    unsigned short hour, min, sec;
    hour = RTCHourGet(SOC_RTC_0_REGS);
    min = RTCMinuteGet(SOC_RTC_0_REGS);
    sec = RTCSecondGet(SOC_RTC_0_REGS);

    RTCTime.tm_hour = bcd2dec(hour);
    RTCTime.tm_min = bcd2dec(min);
    RTCTime.tm_sec = bcd2dec(sec);

    // �������ʾʱ��
    LEDNixieVal[0] = SEGVal[(hour >> 4) & 0x0F];
    LEDNixieVal[1] = SEGVal[hour & 0x0F];

    LEDNixieVal[2] = SEGVal[(min >> 4) & 0x0F];
    LEDNixieVal[3] = SEGVal[min & 0x0F];

    if((LEDNixieVal[0] == 5) && (LEDNixieVal[1] == 9) && (LEDNixieVal[2] == 5) && (LEDNixieVal[3] == 9))
    {
        BUZZERBeep(0xFFFFFF);
    }
}

static Void HwiInit()
{
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    // �ж��¼�
    // ����ֱ��ʹ���ж��¼����Ҳ����ʹ�� StarterWare "interrupt.h" �еĺ궨��
    // ������� TMS320C6748 DSP Technical Reference Manual 83-86ҳ DSP Interrupt Map
    hwiParams.eventId = SYS_INT_RTC_IRQS;
    Hwi_create(C674X_MASK_INT13, RTCHwi, &hwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              ��ʱ���Ǿ�ȷ��                                              */
/*                                                                          */
/****************************************************************************/
void Delay(volatile unsigned int count)
{
    while(count--);
}

/****************************************************************************/
/*                                                                          */
/*              ����ʶ����                                                  */
/*                                                                          */
/****************************************************************************/
unsigned int RtcVersionGet()
{
    // ���� 1 ��ʾ AM1808
    // AM1808 ʹ�� RTC ִ��һ�θ�λ

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void RTCInit()
{
    // ���� RTC �Ĵ���д����
    RTCWriteProtectDisable(SOC_RTC_0_REGS);

    // �����λ��ʹ�� RTC
    RTCEnable(SOC_RTC_0_REGS);

    // ��ʱ ��С3�� 32KH ʱ������
    Delay(0xFFFF);

    // ʹ�� 32KHz ������
    RTCRun(SOC_RTC_0_REGS);

    // ʹ��ʵʱʱ���ж� ÿ�����һ���ж�
    RTCIntTimerEnable(SOC_RTC_0_REGS, RTC_INT_EVERY_SECOND);

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
