#include "stdio.h"
#include "sata.h"

#pragma DATA_ALIGN(CmdLists, 1024);
CmdListHeader CmdLists[32]={0};

#pragma DATA_ALIGN(CmdTable, 128);
CommandTable CmdTable[LISTLENGTH];

#pragma DATA_ALIGN(RcvFis, 256);
ReceiveFis RcvFis;

#define MPY                         9 // bits3:0     4 // Both EVM and VDB Clock Sources are 100MHz
#define LB                          0 // bits5:4     2 // Depends on the REFCLK Jitter but start with 00b
#define LOS                         1 // bit6        1 // Should always be '1', I think
#define RXINVPAIR                   0 // bit7        1 // Unless swapped by the H/W Designer for routing purposes it should be 0 (not inverted)
#define RXTERM                      1 // bits9:8     2 // ??? Common Mode Voltage is speced to be 50 mVp-p PHY reg display time parameter not voltage.
#define RXCDR                       6 // bits12:10   3 // Going by the max jitter spec. ??? Need to look a bit more on this.
#define RXEQ                        1 // bits16:13   4 // Use Adaptive
#define TXINVPAIR                   0 // bit17       1 // Unless swapped by the H/W Designer for routing purposes it should be 0 (not inverted)
#define TXCM                        0 // bit18       1
#define TXSWING                     3 // bits21:19   3 // Tx Swing for Gen2m 400mvpp <---> 700mvpp =>TXSWING=011b - Good enough for both Short & Backplane architecture
#define TXDE                        0 // bits25:22   4
// RESERVED                              bits29:26   4
#define OVERRIDE                    0 // bit30       1
#define ENPLL                       1 // bit31       1

#define SPINUP_TEST_RETRY_COUNT		20
#define DETECT_TEST_RETRY_COUNT		20

/////////////////////////////////////////////////
typedef struct  {
    volatile UINT32 CAP;
    volatile UINT32 GHC;
    volatile UINT32 IS;
    volatile UINT32 PI;
    volatile UINT32 VR;
    volatile UINT32 CCCCTL;
    volatile UINT32 CCCPORTS;
    volatile UINT8 RSVD0[132];
    volatile UINT32 BISTAFR;
    volatile UINT32 BISTCR;
    volatile UINT32 BISTFCTR;
    volatile UINT32 BISTSR;
    volatile UINT32 BISTDECR;
    volatile UINT8 RSVD1[44];
    volatile UINT32 TIMER1MS;
    volatile UINT8 RSVD2[4];
    volatile UINT32 GPARAM1R;
    volatile UINT32 GPARAM2R;
    volatile UINT32 PPARAMR;
    volatile UINT32 TESTR;
    volatile UINT32 VERSIONR;
    volatile UINT32 IDR;
    volatile UINT32 P0CLB;
    volatile UINT8 RSVD3[4];
    volatile UINT32 P0FB;
    volatile UINT8 RSVD4[4];
    volatile UINT32 P0IS;
    volatile UINT32 P0IE;
    volatile UINT32 P0CMD;
    volatile UINT8 RSVD5[4];
    volatile UINT32 P0TFD;
    volatile UINT32 P0SIG;
    volatile UINT32 P0SSTS;
    volatile UINT32 P0SCTL;
    volatile UINT32 P0SERR;
    volatile UINT32 P0SACT;
    volatile UINT32 P0CI;
    volatile UINT32 P0SNTF;
    volatile UINT8 RSVD6[48];
    volatile UINT32 P0DMACR;
    volatile UINT8 RSVD7[4];
    volatile UINT32 P0PHYCR;
    volatile UINT32 P0PHYSR;
} CSL_SataRegs;

#define CSL_SATA_0_REGS ((CSL_SataRegs *)0x01E18000u)

CSL_SataRegs* sataRegs = CSL_SATA_0_REGS;
FirmwareCtrlFeatures swCtrlFeatures;
cmdFis myCmdFis;

UINT32 temp=0xABCDEF98;

static void delay(unsigned int n)
{
	unsigned int i;

	for(i = n; i > 0; i--);
}

char placeHbaInIdle(void) {
	// To Place HBA In IDLE, need to make sure both DMAs (Cmd List and Rcv FIS) are not running.
	//  Order of Disabling the DMA is important.

	// Ensure that the Cmd List DMA is not running
	//  If is running, clear ST and wait for 500ms. Then Check CR.
	if (sataRegs->P0CMD & AHCI_PxCMD_ST) {
		sataRegs->P0CMD &= ~(1 << AHCI_PxCMD_ST_SHIFT);
		waitForXms(WAIT_500_MILLISECONDS);
	}// Wait another 500 Milliseconds for CR to clear. This is twice more than required.
	if (sataRegs->P0CMD & AHCI_PxCMD_CR)
		waitForXms(WAIT_500_MILLISECONDS);

	// If P0CMD.CR is still set, HBA probably has hunged. No need to continue.
	//  Need to perform HBA Reset.
	if (sataRegs->P0CMD & AHCI_PxCMD_CR)
		return(1);

	// Ensure that the Receive FIS DMA is running.
	//  If is running, clear FRE and wait for 500ms. Then Check FR.
	if (sataRegs->P0CMD & AHCI_PxCMD_FRE) {
			sataRegs->P0CMD &= ~(1 << AHCI_PxCMD_FRE_SHIFT);
			waitForXms(WAIT_500_MILLISECONDS);
		}// Wait unti FR is Cleared.
	while (sataRegs->P0CMD & AHCI_PxCMD_FR)
		waitForXms(WAIT_500_MILLISECONDS);

	// If P0CMD.FRE is still set, HBA probably has hunged. No need to continue.
	//  Need to perform HBA Reset.
	if (sataRegs->P0CMD & AHCI_PxCMD_FRE)
		return(1);

	return(0);
}

void waitForXms(UINT32 waitValue) {
	UINT32 i=0, j=0;;
	for (i=0; i<waitValue; i++)
		for(j=0; j<ONE_MS_VALUE; j++);
}

void enableRcvFis(void) {
	// Enable Receive FIS
	sataRegs->P0CMD |= AHCI_PxCMD_FRE;
	// Wait for the FIS Rcv DMA Engine to start
	while(temp=(sataRegs->P0CMD & AHCI_PxCMD_FR) != AHCI_PxCMD_FR);

}

char spinUpDeviceAndWaitForInitToComplete(void) {

	UINT32 rtn = ERR_TIMEOUT;
	UINT32 retryCount;

	// Make sure that the HBA is in a Listen Mode prior to Spinning Up Device
	//  Following Configuration is not allowed.
	//      [P0SCTL.DET, P0CMD.SUD] = [1,1] NOT Allowed.
	if((sataRegs->P0SCTL & AHCI_PxSCTL_PxSSTS_DET) != 0)
		sataRegs->P0SCTL &= ~(0xf << AHCI_PxSCTL_PxSSTS_DET_SHIFT);

	// Configure Line Speed.
	setSataSpeed(DESIRED_SPEED);  //GOASFASTASDEVICE=0,GEN1=1,GEN2=2
								  //Preprocessor Directive within Project file.
								  //Other related files @ speed_select.h & sata_utilities.h

	// Moved this prior to SUD in order to make sure that HBA updates P0TFD of the
	//  status of the Device as it is received.

	// Clear P0SERR.DIAG.X (RWC bit field) so that the P0TFD is updated by HBA.
	sataRegs->P0SERR |= 0x04000000;

	// Spin Up Device.
	sataRegs->P0CMD |= (1 << AHCI_PxCMD_SUD_SHIFT);

	// Wait for Device Detection or/and Speed Negotiation to take place and finish.
	//	while (!(((sataRegs->P0SSTS & AHCI_PxSCTL_PxSSTS_DET) == 0x1) ||
	//	       ((sataRegs->P0SSTS & AHCI_PxSCTL_PxSSTS_DET) ==0x3))
	//	      );

	retryCount = 0;
	while (((sataRegs->P0SSTS & AHCI_PxSCTL_PxSSTS_DET) !=0x3) && (retryCount < DETECT_TEST_RETRY_COUNT))
	{
		delay(0x00FFFFFF);
		retryCount++;
	}

	// Check if the command was successfull or we timed out.
	if ((sataRegs->P0SSTS & AHCI_PxSCTL_PxSSTS_DET) !=0x3)
	{
		rtn = ERR_FAIL;
		return(rtn);
	}	

	// Clear P0SERR.DIAG.X (RWC bit field) so that the P0TFD is updated by HBA.
	//	sataRegs->P0SERR |= 0x04000000;

	// Device would send its status and finished power up will send its default
	// Task file regs and look for Device ready status.
	// while ((sataRegs->P0TFD & AHCI_PxTFD_STS_BSY_DRQ_ERR) != 0);

	// wait for D2H FIS to propagate to Device Memory and avoid early comparison
	// Make sure that the expected Device signature is received.
	retryCount = 0;
	while ((sataRegs->P0SIG != AHCI_P0SIG_SIG_ATA_DEV_GOOD_STAT) && (retryCount < SPINUP_TEST_RETRY_COUNT)) // LBAhigh:LBAmid:LBAlow:SECcnt
	{
		delay(0x00FFFFFF);
		retryCount++;
	}

	// Check if the command was successfull or we timed out.
	if (sataRegs->P0SIG == AHCI_P0SIG_SIG_ATA_DEV_GOOD_STAT)
	{
		rtn = ERR_NO_ERROR;
	}

	// This also means that Device Signature has been received from Device (D2H FIS)
	// You can check RcvFIS[n]+0x40.

	return(rtn);
}

void setSataSpeed(unsigned char iSpeed) {
	sataRegs->P0SCTL &= ~(AHCI_PxSCTL_PxSSTS_SPD);
	sataRegs->P0SCTL |= (iSpeed << AHCI_PxSCTL_PxSSTS_SPD_SHIFT);
	waitForXms(5); // This might not be necessary: wait a bit
}

void invokeHBAReset() {
    // HBA Reset will not affect the following Registers settings of PxFB and PxCLB
    //  regs and HwInit fields of Port Registers are not affected.
    // To Do: Check if the Global Registers are affected. Spec mentions not affected.

	// Note: COMRESET OOB will not be sent to attached Device because Freon supports
	//  Staggered Spinup capability and P0CMD.SUD is cleared to Zero when HBA Reset
	//  takes place. Software needs to invoke this if needed.

	// Most likely user want to ensure HBA comes up in its default operation state
	//  or has hung and are unable to idle the port when needing to perform an HBA
	//  reset. Regardless, there is no need to attempt to idle the HBA from
	//  running
	sataRegs->GHC |= (1 << AHCI_GHC_HR_SHIFT);

	// Max Spec time is 1 Second for Reset to complete.
	while((sataRegs->GHC & AHCI_GHC_HR) != 0) {
		waitForXms(WAIT_500_MILLISECONDS);
		waitForXms(WAIT_500_MILLISECONDS);
	}
}

void cfgDmaSetting(void) {
	sataRegs->P0DMACR   |= ((DMA_BURST_LENGTH << AHCI_P0DMACR_TXTS_SHIFT)       |
	                        (DMA_BURST_LENGTH << AHCI_P0DMACR_RXTS_SHIFT)       |
	                        (DMA_TRANACTION_LENGTH << AHCI_P0DMACR_TXABL_SHIFT) |
	                        (DMA_TRANACTION_LENGTH << AHCI_P0DMACR_RXABL_SHIFT)
	                       );
}

void initBaseAddresses(void) {
	sataRegs->P0CLB=(unsigned int)CmdLists;
	sataRegs->P0FB=(unsigned int)&RcvFis;
}

void performFirmwareInit(void) {
	sataRegs->CAP |= ((swCtrlFeatures.capSMPS << 28) |
	                  (swCtrlFeatures.capSSS  << 27)
	                 );

	sataRegs->PI  |=  (swCtrlFeatures.piPi  << 0);

	sataRegs->P0CMD |= ((swCtrlFeatures.p0cmdEsp << 21) |
                        (swCtrlFeatures.p0cmdCpd << 20) |
                        (swCtrlFeatures.p0cmdMpsp << 19)|
                        (swCtrlFeatures.p0cmdHpcp << 18)
                       );

	initBaseAddresses(); // Initialize Command List (P0CLB) and Receive FIS (P0FS)

	initPhyAndDmaPars(); // Initialize PHY and DMA Parameters

	enableRcvFis();	     // Enable Receive DMA
}

void initPhyAndDmaPars(void)
{
	sataRegs->P0PHYCR =  MPY		<< 0	|
						 LB			<< 4	|
						 LOS		<< 6	|
						 RXINVPAIR 	<< 7	|
						 RXTERM		<< 8	|
						 RXCDR		<< 10	|
						 RXEQ		<< 13	|
						 TXINVPAIR 	<< 17	|
						 TXCM		<< 18	|
						 TXSWING 	<< 19	|
						 TXDE		<< 22	|
						 OVERRIDE 	<< 30	|
						 ENPLL		<< 31;

	cfgDmaSetting();
}
