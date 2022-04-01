C6748 DSP 综合演示程序  

CCSv7 IDE  
- 7.4  

编译器  
- TI C6000 CGT 8.3.12  

依赖组件  
- XDCTools 3.25.6.96  
- NDK 2.24.3.35  
- SYS/BIOS 6.37.5.35  
- UIA 1.4.0.06  

测试方法  

核心板  
1. LED2             呼吸灯  
2. LED3             闪烁  
3. SPI NOR FLASH    在 USB UART 串口终端运行 「spiflash」 命令  
4. SD NAND          暂不支持  

EVM-CKL138PKT 底板  
 1. LED3             按下 SW4 会闪烁  
 2. LED4             按下 SW6 会闪烁  
 3. 复位按键         DSP 复位  
 4. NMI 按键         串口终端输出异常信息  
 5. USB UART         串口终端  
 6. LCD 连接         LCD 屏幕 屏幕会显示图片、MAC/IP、温湿度以及时间等信息  
 7. 蜂鸣器           串口终端 运行「buzzer 500000」数字为持续时间  
 8. 数码管           显示时间 时和分  
 9. USB OTG 2.0      暂不支持  
10. SD 卡            暂不支持  
11. 网络             连接网线后串口终端输出 DHCP 获取的 IP 地址  
12. RTC              串口终端设置 RTC 时间 运行「clock 或 hwclock -s 20220222 222222」年月日 时分秒  
13. WIFI/BT 模块     使用 AT 指令建立连接 PC 需要将网络程序设置为防火墙例外或者关闭指定网络防火墙  
                     下述命令需要在串口终端输入  
                     [ 1] wifi AT+GMR                                       # [可选] 获取模块版本信息  
                     [ 2] wifi AT                                           # [可选] 测试 AT 功能  
                     [ 3] wifi AT+CWMODE=1                                  # 设置模组进入 STA 模式  
                     [ 4] wifi AT+CWJAP="GS-FHQ","gsdz2019"                 # 连接 WIFI SSID/密码  
                     [ 5] wifi AT+CIPSTART="TCP","172.26.0.160",8080        # 连接 TCP 服务器 IP/端口  
                     [ 6] wifi AT+SAVETRANSLINK=1,"172.26.0.160",8080,"TCP" # [可选] 设置开机进入TCP透传模式，将配置参数写入flash中  
                     [ 7] wifi AT+CIPMODE=1                                 # 设置设备进入透传模式  
                     [ 8] wifi AT+CIPSEND                                   # 在透传模式下传输数据，串口输入的数据均会发送到服务器中  
                     [ 9] wifi ABCD                                         # 发送任意数据到服务器  
                     [10] wifi -e                                           # 退出透传模式（+++）  
                     [11] wifi AT+CIPCLOSE                                  # 断开TCP连接  
14. VPIF             暂不支持  
15. EMIFA            暂不支持  
16. M.2 USB          不支持  
17. M.2 SATA         不支持  

模块  

- 点阵模块         使用 PRU 控制 DSP 加载 PRU 程序运行即可  

程序固化  
- 程序镜像生成     使用 AISgen_d800k008.exe 上位机程序生成 ais 文件  
                   使用 HexAIS_OMAP-L138.exe 生成 ais 文件  

- 使用串口         [1] 调整拨码开关启动模式为 UART2  
                   [2] 运行程序 sfh_OMAP-L138 -flash_noubl -targetType C6748 -flashType SPI_MEM -p COM14 Board_C674x.ais  
			       [3] 等待串口加载和烧写完成  
			       [4] 调整拨码开关启动模式为 SPI NOR FLASH  