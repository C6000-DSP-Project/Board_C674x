#ifndef _SATA_H
#define	 _SATA_H

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

#define ERR_NO_ERROR             (0)
#define ERR_FAIL                 (1)
#define ERR_TIMEOUT              (5)

#define NUMOFPORTS       (1)	// Freon Supports A Single HBA Port. However it can support up to
						    	//  16 additional Ports with the use of an external Port Multiplier.
						    	//  So keep this value to 1.
#define LISTLENGTH       (4)	// Max Command Header Per Port is 32

#define WRITE_CMD_SLOT   (0)	// Value used here should be <= LISTLENGTH-1
#define READ_CMD_SLOT    (1)	// Value used here should be <= LISTLENGTH-1
								// WARNING. PRDLENGTH can not be greater than 8 for this program.
								//  See Note captured by the area when memory has been reserved for
								//  within sata_utilities.c for Command Table "CmdTable" for
								//  more information.

//#define _MAX_DATA_TRANSFER_	// Define this in project file when needed.

#ifndef _MAX_DATA_TRANSFER_ 	// 512 Bytes Data Size within 2 PRD Descriptors.
	#define PRDLENGTH     (2)	// Max PRD Length is 65535 per port.
	#define DATABUFFERLEN (256)	// DMA Data Buffer Length
#else // Max Data Size Transfer 8K Bytes within 2 PRD Descriptors
	#define PRDLENGTH     (2)   	// Max PRD Length is 65535 per port.
	#define DATABUFFERLEN (4096) 	// DMA Data Buffer Length
#endif

#if ((PRDLENGTH > 8) | (WRITE_CMD_SLOT > LISTLENGTH-1) | (READ_CMD_SLT > LISTLENGTH-1))
#error PRDLENGTH ENTRY ERROR - PROGRAM HARD CODED FOR MAX VALUE OF 8 - CMD SLOT ENTRY ERROR
#endif
/////////////////////////////////////////////////////////////////////////////////////////////
// Maximum of 32 commands slots per port exist where each command occupies 8 DWs (64 Bytes).
// The structure 'CmdListHeader' defines a single command header definition.
// The start of the first Command List &CmdListHeader[0] needs to be programmed onto P0CLB.
//
// Command List Base Address should be 1K Byte Aligned.

typedef struct {
	UINT32 CmdLen:5;   // bits[4:0]
	UINT32 Atapi:1;    // bit[5]
	UINT32 Write:1;    // bit[6]
	UINT32 Prefetch:1; // bit[7]
	UINT32 Reset:1;    // bit[8]
	UINT32 Bist:1;     // bit[9]
	UINT32 Rok:1;      // bit[10]
	UINT32 Rsv:1;      // bit[11]
	UINT32 Pmp:4;      // bits[15:12]
	UINT32 Prdtl:16;   // bits[31:16]
}CmdListHeaderW0;

typedef struct {
	UINT32 PrdByteCnt; // bits[31:0]
}CmdListHeaderW1;


typedef struct {
//	UINT32 CmdTableAddLowRsv:7; // bit[6:0]
//	UINT32 CmdTableAddLow:25;   // bits[31:7]
	UINT32 CmdTableAddLow;      // bits[31:7]
}CmdListHeaderW2;

typedef struct {
	UINT32 CmdTableAddHigh;     // bits[31:0]
}CmdListHeaderW3;


typedef struct {
	CmdListHeaderW0 DW0;
	CmdListHeaderW1 DW1;
	CmdListHeaderW2 DW2;
	CmdListHeaderW3 DW3;
	UINT32          DW4;
	UINT32          DW5;
	UINT32          DW6;
	UINT32          DW7;
} CmdListHeader;

/////////////////////////////////////////////////////////////////////////////////////////////
// Command Table requires alignment of 128 Bytes. CTBA should be programmed with this
//  restriction.

// Command Table Data Structure
// Members: Command FIS
//          ATAPI Command
//          PRD Table

// Host to Device FIS Definition also named as Command FIS in AHCI Specification.
//
//-----------Command FIS-------------------------
typedef struct {
	UINT32 B0FisType:8;//bits[7:0]
	UINT32 BYTE1:8;       //bits[15:8]
	UINT32 B2Cmd:8;    //bits[23:16]
	UINT32 B3Feature:8;//bits[31:24]
}CmdFisWord0;

typedef struct {
	UINT32 B0LbaLow:8; //bits[7:0]
	UINT32 B1LbaMid:8; //bits[15:8]
	UINT32 B2LbaHigh:8;//bits[23:16]
	UINT32 B3Device:8; //bits[31:24]
}CmdFisWord1;

typedef struct {
	UINT32 B0LbaLowExp:8; //bits[7:0]
	UINT32 B1LbaMidExp:8; //bits[15:8]
	UINT32 B2LbaHighExp:8;//bits[23:16]
	UINT32 B3FeatureExp:8; //bits[31:24]
}CmdFisWord2;

typedef struct {
	UINT32 B0SecCnt:8;    //bits[7:0]
	UINT32 B1SecCntExp:8; //bits[15:8]
	UINT32 B2Rsv:8;       //bits[23:16]
	UINT32 B3Control:8;   //bits[31:24]
}CmdFisWord3;

typedef struct {
	UINT32 DWResv; //bits[31:0]
}CmdFisWord4;

typedef struct {
	CmdFisWord0 DW0;
	CmdFisWord1 DW1;
	CmdFisWord2 DW2;
	CmdFisWord3 DW3;
	CmdFisWord4 DW4;
	UINT32      DW5;
	UINT32      DW6;
	UINT32      DW7;
	UINT32      DW8;
	UINT32      DW9;
	UINT32      DW10;
	UINT32      DW11;
	UINT32      DW12;
	UINT32      DW13;
	UINT32      DW14;
	UINT32      DW15;
}CommandFIS;

//-----------Command FIS end ATAPI Command ------

// ATAPI Command Data Structure
typedef struct {
	UINT32 ATAPI[4];
}Atapi;

//-----------ATAPI Command end PRDT -------------
// Physical Region Descriptor Table Data Structure
typedef struct {
	UINT32 DbaLow;   // bits[31:0]
}DbaAddressLow;

typedef struct {
	UINT32 DbaHigh;  // bits[31:0]
}DbaAddressHigh;

typedef struct {
	UINT32 DW2Reserved; // bits[31:0]
}PrdtRsv;

typedef struct {
	UINT32 DataBC:22;   // bits[21:0]
}DataByteCnt;


typedef struct {
	DbaAddressLow  DW0;
	DbaAddressHigh DW1;
	PrdtRsv        DW2;
	DataByteCnt    DW3;
}PRDT;
//-----------PRDT end ---------------------------

//-----------Command Table Data Structure -------
typedef struct {
	CommandFIS cfis;
	Atapi      atapi;
	UINT32     Rsv[12];
	PRDT       prdTable[8];	// Have forced this size to 8 in order to meet the minimum
							//  required size for Command Table.
}CommandTable;

//-----------Command Table Data Structure end ---
/////////////////////////////////////////////////////////////////////////////////////////////
// Receive FIS requires the Receive FIS to be 256 byte aligned. P0FB should be programmed
//  with this restriction.
//
// RECEIVE FIS Data Structure
// Members: DMA Setup FIS (DSFIS)
//          PIO Setup FIS (PSFIS)
//          D2H Register FIS (RFIS)
//          Set Device Bits FIS (SDBFIS)
//          Unknown FIS (UFIS)

//-----------DMA Setup FIS-----------------------

typedef struct {
	UINT32 B0FisType:8;// bits[7:0]
	UINT32 BYTE1:8;    // bits[15:8]
	UINT32 B2Rsv:8;    // bits[23:16]
	UINT32 B3Rsv:8;    // bits[31:24]
}DsfisW0;



typedef struct {
	DsfisW0 DW0;				// 4 x 4B	= 16B
	UINT32 DW1DmaBuffLow;		// 4B
	UINT32 DW2DmaBuffHigh;		// 4B
	UINT32 DW3Rsv;				// 4B
	UINT32 DW4DmaBuffOffset;	// 4B
	UINT32 DW5DmaXfrCnt;		// 4B
	UINT32 DW6Rsv;				// 4B
}DMASetupFis;					// = 40B

//-----------DMA Setup FIS end PIO Setup FIS ----
typedef struct {
	UINT32 B0FisType:8;	  // bits[7:0]
	UINT32 BYTE1:8;       // bits[15:8]
	UINT32 B2Status:8;    // bits[23:16]
	UINT32 B3Errror:8;	  // bits[31:24]
}PioSetupDW0;			  // 16B

typedef struct {
	UINT32 B0LbaLow:8; 	// bits[7:0]
	UINT32 B1LbaMid:8; 	// bits[15:8]
	UINT32 B2LbaHigh:8;	// bits[23:16]
	UINT32 B3Device:8; 	// bits[31:24]
}PioSetupDW1;

typedef struct {
	UINT32 B0LbaLowExp:8; // bits[7:0]
	UINT32 B1LbaMidExp:8; // bits[15:8]
	UINT32 B2LbaHighExp:8;// bits[23:16]
	UINT32 B3Rsv:8;       // bits[31:24]
}PioSetupDW2;


typedef struct {
	UINT32 B0SecCnt:8;    // bits[7:0]
	UINT32 B1SecCntExp:8; // bits[15:8]
	UINT32 B2Rsv:8;       // bits[23:16]
	UINT32 B3Estatus:8;   // bits[31:24]
}PioSetupDW3;

typedef struct {
	UINT32 HW0XferCnt:16; // bits[15:0]
	UINT32 HW1Rsv:16;     // bits[31:16]
}PioSetupDW4;

typedef struct {
	PioSetupDW0 DW0;	// 16B
	PioSetupDW1 DW1;	// 16B
	PioSetupDW2 DW2;	// 16B
	PioSetupDW3 DW3;	// 16B
	PioSetupDW4 DW4;	// 16B
}PIOSetupFis;			// 80B

//-----------PIO Setup FIS end D2H Reg FIS-------
typedef struct {
	UINT32 B0FisType:8;// bits[7:0]
	UINT32 BYTE1:8;    // bits[15:8]
	UINT32 B2Status:8; // bits[23:16]
	UINT32 B3Errror:8; // bits[31:24]
}D2HRegDW0;

typedef struct {
	UINT32 B0LbaLow:8; // bits[7:0]
	UINT32 B1LbaMid:8; // bits[15:8]
	UINT32 B2LbaHigh:8;// bits[23:16]
	UINT32 B3Device:8; // bits[31:24]
}D2HRegDW1;

typedef struct {
	UINT32 B0LbaLowExp:8; // bits[7:0]
	UINT32 B1LbaMidExp:8; // bits[15:8]
	UINT32 B2LbaHighExp:8;// bits[23:16]
	UINT32 B3Rsv:8;       // bits[31:24]
}D2HRegDW2;


typedef struct {
	UINT32 B0SecCnt:8;    // bits[7:0]
	UINT32 B1SecCntExp:8; // bits[15:8]
	UINT32 HW1Rsv:16;     // bits[31:16]
}D2HRegDW3;

typedef struct {
	UINT32 W0Rsv;         // bits[31:0]
}D2HRegDW4;

typedef struct {
	D2HRegDW0 DW0;
	D2HRegDW1 DW1;
	D2HRegDW2 DW2;
	D2HRegDW3 DW3;
	D2HRegDW4 DW4;
}D2HRegFis;

//-----------D2H Reg FIS end Set Device Bits FIS-
// The Set Device Bit FIS definition does not contain the 2nd Word required
//  for Native Command Queueing. This second word is the SACTVE register and
//  the AHCI takes care of updating SACTIVE register at its location.
typedef struct {
	UINT32 B0FisType:8;// bits[7:0]
	UINT32 BYTE1:8;    // bits[15:8]
	UINT32 B2Status:8; // bits[23:16]
	UINT32 B3Errror:8; // bits[31:24]
}SetDevBitsDW0;

typedef struct {
	UINT32 SActive;    // bits[31:0]
}SetDevBitsDW1;

typedef struct {
	SetDevBitsDW0 DW0;
	SetDevBitsDW1 DW1;
}SetDevBitsFis;

//-----------Set Device Bits FIS end Unkonwn FIS-
typedef struct {
	UINT32 UserDefined; // bits[31:0]
}UnknownDWx;


typedef struct {
	UnknownDWx DW[16]; // 16 Words (Max 64 Bytes allowed)
}UnknownFis;

//-----------Unkonw FIS end----------------------
//-----------Receive Register FIS Structure------
typedef struct {
	DMASetupFis   DSFIS;		// 40B
	UINT32        Rsv1;			// 4B
	PIOSetupFis   PSFIS;		// 80B
	UINT32        Rsv2[3];
	D2HRegFis     RFIS;
	UINT32        Rsv3;
	SetDevBitsFis SDBFIS;
	UnknownFis    UFIS;
}ReceiveFis;

//-----------Receive Register FIS Structure End--
extern unsigned char prdTableDataBuff[LISTLENGTH][PRDLENGTH][DATABUFFERLEN];

extern CmdListHeader CmdLists[];
extern CommandTable CmdTable[];
extern ReceiveFis RcvFis;
extern UINT16 cmdSlot2Use;

/////////////////////////////////////////////////////////////////////////
// Used to Configure PHY and Speed
// PHY Configuration for Silicon is Different than for QT. SILICON PHY
//  requires more careful programming based on Board Design and need
//  to use values that is good for Internal, Intermediate, or Exterme
//  Envirnoment. The _SILICON_ #define differentiates between Internal
//  and QT timing. For Slicon keep the #define. For QT, Comment it.
/////////////////////////////////////////////////////////////////////////
#if _NO_SPEED_NEGOTIATION_RESTRICTION_
		#define DESIRED_SPEED	GOASFASTASDEVICE	// Go As fast as the Device Can Go.
#elif _GEN1_
		#define DESIRED_SPEED	GEN1	// Go As fast as GEN1
#elif _GEN2_
		#define DESIRED_SPEED	GEN2	// Go As fast as GEN2
#else
		#define DESIRED_SPEED	GOASFASTASDEVICE	// Go As fast as the Device Can Go.
#endif

//#define DESIRED_SPEED			(GEN1)        // GOASFASTASDEVICE, GEN1, GEN2
#define DEVICE_LBA_ADDRESS      (0x00000002) // Dev28bitLbaAddress = 28-Bit LBA Address
#define WAIT_500_MILLISECONDS   (50)		  // This should be set to 500 once the ONE_MS_VALUE is programmed correctly.
#define WAIT_1_MILLISECOND      (1)
#define ONE_MS_VALUE			 (1)		  // Number of CPU Cycles needed to generate a millisecond wait time.
//* Note: Use this rule to program P0DMACR Register:
//*       Burst Size <= Transaction Size
//*  The followings is the combination of allowed options.
//*  Note: transaction value =3 and burst value = 0 or 4 gives the max throughput.
//*
//*       transaction       burst
//*           3          4,3,2,1,0
//*           2          3,2,1,0
//*           1          2,1,0
//*           0          1,0

#define DMA_BURST_LENGTH        (0x4)        // [0x0 - 0x4] Burst=2^(n-1) i.e., 0x4=> 2^(4-1)=8
#define DMA_TRANACTION_LENGTH   (0x3)        // [0x0 - 0x3] TransLength=2^n i.e., 0xA=> 2^3=8
// CCC_CTL REGs Fields
#define CCC_TIMEOUT_MS_VALUE    (0x1234)     // 16-bit Field
#define CCC_NUM_OF_CMDS         (0xCD)       // 8-bit Field
// 1MS Timer Reg Field
#define ONE_MS_TIMER_RESOLUTION (100000)     // [0 - 0x3FFFFF]

typedef enum {
	_NO=0,
	_YES=1,
	_NOT_DETERMINED=2
}sataCapability;

typedef enum {
	GOASFASTASDEVICE=0,
	GEN1=1,
	GEN2=2,
	COMMNOTESTABLISHED=0xFF
}interfaceSpeed;

typedef enum {
	GLOBALint=0x11,
	PORTint=0x22	// PortInt=LocalInt
}intType;

typedef enum {
	ENABLE=0x33,
	DISABLE=0x55
}intState;

typedef enum {
	INTFIELDS=0x77,
	ERRORFIELDS=0x88
}intOrErrorDiag;

typedef enum {
	SETINT=0x44,
	CLEARINT=0x66
}intStatus;

typedef enum {
	QUEUED_CMD=0xAB,
	NON_QUEUED_CMD=0xDC
}cmdType;

typedef enum {
	DATA_DIR_RD=0xBA,
	DATA_DIR_WR=0xCD
}dataXferDir;

typedef enum {
	PIO_PROTOCOL,
	DMA_PROTOCOL
}xferProtocol;


typedef enum {
	USE_POLLING=8,
	USE_INT_HANDLER
} interruptHandlingMethod;


typedef enum {
	supportPmOnly=0xBB,
	supportSmOnly=0xCC,
	supportBothPm=0xDD,
	supportNone  =0xEE,
	notDetermined=0xFF
}pwrMgmtCapability;


typedef enum {
	BYTE_COUNT_EQUAL=0x1A,
	BYTE_COUNT_NOT_EQUAL=0xA1
}compareResult;

/*
typedef enum {
	ENABLE=0x33,
	DISABLE=0x55
}lowPwrState;
*/
typedef intState lowPwrState;


typedef enum {
	hostPTnoRestricition             = 0xCF,
	hostPTtoPartialNotAllowed        = 0xDF,
	hostPTtoSlumberNotAllowed        = 0xEF,
	hostPTtoPartialSlumberNotAllowed = 0xFF,
	HostPTisNotDetermined            = 0x00
}hostPwrMgmtState;


typedef enum {
	hostCfgSlumberState = 0xCF,
	hostCfgPartialState = 0xDF,
	hostCfgDisPwrDwn    = 0xEF,
	notAccessed         = 0xFF
}hostPwrMgmtConfig;

typedef struct {
	UINT32  capSMPS:1;      // bit[0] // Freon has bonded out an input pin for this purpose. So, set it to 1.
	UINT32  capSSS:1;       // bit[1] // Need to set this to 1 regardless to the support of SSS or not since we are supporting a single HBA port. If using a PM then need to enable this bit one at a time per PM port basis.
	UINT32  piPi:2;         // bits[3:2] // For Freon this should always be set to 1 since bit 0 corresponds to a sigle HBA port support. When using PM, use Command Based switching to access multiple ports.
	UINT32  p0cmdCpd:1;     // bit[4] // We have bonded out the two signals (one input and one output) for Detecting Device that are Bus Powered
	UINT32  p0cmdEsp:1;     // bit[5] // This is most likely needs to be set to Zero since it is mutually exclusive with MPSP.
	UINT32  p0cmdMpsp:1;    // bit[6] // We have bonded out a pin (input) to detect a change on a switch or line
	UINT32  p0cmdHpcp:1;    // bit[7] // Since we support CPD and MPSP by default we will have to support Hot Plugable Capability.
	UINT32  rsv:24;         // bits[24:8] Reserved and populate if need to add more control
}FirmwareCtrlFeatures;

// Sector Count   Sector Count
// LBA Low        Sector Number
// LBA Mid        Cylinder Low
// LBA High       Cylinder High

// The following structures are subsets of command FIS that I have interest of and are not here to define the actual Command FIS. See sata_ahci.h for FIS defintion.
typedef struct {
	UINT8 cfisType;
	UINT8 cfisByte1;
	UINT8 cfisCmd;
	UINT8 cfisFeature;
	UINT8 cfisDw1SecNumLbaLow;
	UINT8 cfisDw1CylLowLbaMid;
	UINT8 cfisDw1CylHighLbahigh;
	UINT8 cfisDw1Dev;
	UINT8 cfisDw2SecNumLbaLowExp;
	UINT8 cfisDw2CylLowLbaMidExp;
	UINT8 cfisDw2CylHighLbahighExp;
	UINT8 cfisDw2FeatureExp;
	UINT8 cfisDw3SecCnt;
	UINT8 cfisDw3SecCntExp;
	UINT8 cfisDw3Ctrl;
}cmdFis;

// The following structures are subsets of DMA Setup FIS that I have interest of and are not here to define the actual FIS. See sata_ahci.h for FIS defintion.
typedef struct {
	UINT8 dsfisType;
	UINT8 dsfisByte1;
	UINT32 dsfisDw1DmaBuffLow;
	UINT32 dsfisDw2DmaBuffHigh;
	UINT32 dsfisDw4DmaBuffOffset;
	UINT32 dsfisDw5DmaXferCnt;
}dsFis;

// The following structures are subsets of Set Device Bits FIS that I have interest of and are not here to define the actual FIS. See sata_ahci.h for FIS defintion.
typedef struct {
	UINT8 sdbfisType;
	UINT8 sdbfisByte1;
	UINT8 sdbfisStatus;
	UINT8 sdbfisError;
}sdbFis;

// The following structures are subsets of Unknown FIS that I have interest of and are not here to define the actual FIS. See sata_ahci.h for FIS defintion.
typedef struct {
	UINT32 ufisWord[16];
}uFis;

// CCC
#define AHCI_CCCCTL_EN_SHIFT         (0)
#define AHCI_CCCCTL_INT_SHIFT        (3)
#define AHCI_CCCCTL_CC_SHIFT         (8)
#define AHCI_CCCCTL_TV_SHIFT         (16)


#define AHCI_CCCCTL_EN               (0x1    << AHCI_CCCCTL_EN_SHIFT)
#define AHCI_CCCCTL_INT              (0x1f   << AHCI_CCCCTL_INT_SHIFT)
#define AHCI_CCCCTL_CC               (0xff   << AHCI_CCCCTL_CC_SHIFT)
#define AHCI_CCCCTL_TV               (0xffff << AHCI_CCCCTL_TV_SHIFT)

// P0CMD
#define	AHCI_PxCMD_ST_SHIFT	   (0)
#define	AHCI_PxCMD_SUD_SHIFT   (1)
#define	AHCI_PxCMD_POD_SHIFT   (2)
#define	AHCI_PxCMD_CLO_SHIFT   (3)
#define	AHCI_PxCMD_FRE_SHIFT   (4)
#define	AHCI_PxCMD_CCS_SHIFT   (8)
#define	AHCI_PxCMD_MPSS_SHIFT  (13)
#define	AHCI_PxCMD_FR_SHIFT    (14)
#define	AHCI_PxCMD_CR_SHIFT    (15)
#define	AHCI_PxCMD_CPS_SHIFT   (16)
#define	AHCI_PxCMD_PMA_SHIFT   (17)
#define	AHCI_PxCMD_HPCP_SHIFT  (18)
#define	AHCI_PxCMD_MPSP_SHIFT  (19)
#define	AHCI_PxCMD_CPD_SHIFT   (20)
#define	AHCI_PxCMD_ESP_SHIFT   (21)
#define	AHCI_PxCMD_ATAPI_SHIFT (24)
#define	AHCI_PxCMD_DLAE_SHIFT  (25)
#define	AHCI_PxCMD_ALPE_SHIFT  (26)
#define	AHCI_PxCMD_ASP_SHIFT   (27)
#define	AHCI_PxCMD_ICC_SHIFT   (28)

/* port command and status bits */
#define	AHCI_PxCMD_ST	  (0x1 << AHCI_PxCMD_ST_SHIFT)   /* Start */
#define	AHCI_PxCMD_SUD	  (0x1 << AHCI_PxCMD_SUD_SHIFT)  /* Spin-up device */
#define	AHCI_PxCMD_POD	  (0x1 << AHCI_PxCMD_POD_SHIFT)  /* Power on device */
#define	AHCI_PxCMD_CLO	  (0x1 << AHCI_PxCMD_CLO_SHIFT)  /* Command list override */
#define	AHCI_PxCMD_FRE	  (0x1 << AHCI_PxCMD_FRE_SHIFT)  /* FIS receive enable */
#define	AHCI_PxCMD_CCS	  (0x1f << AHCI_PxCMD_CCS_SHIFT) /* Current command slot */
			/* Mechanical presence switch state */
#define	AHCI_PxCMD_MPSS	  (0x1 << AHCI_PxCMD_MPSS_SHIFT)
#define	AHCI_PxCMD_FR	  (0x1 << AHCI_PxCMD_FR_SHIFT)   /* FIS receiving running */
#define	AHCI_PxCMD_CR	  (0x1 << AHCI_PxCMD_CR_SHIFT)   /* Command list running */
#define	AHCI_PxCMD_CPS	  (0x1 << AHCI_PxCMD_CPS_SHIFT)  /* Cold presence state */
#define	AHCI_PxCMD_PMA	  (0x1 << AHCI_PxCMD_PMA_SHIFT)  /* Port multiplier attached */
#define	AHCI_PxCMD_HPCP	  (0x1 << AHCI_PxCMD_HPCP_SHIFT) /* Hot plug capable port */
			/* Mechanical presence switch attached to port */
#define	AHCI_PxCMD_MPSP	  (0x1 << AHCI_PxCMD_MPSP_SHIFT)
#define	AHCI_PxCMD_CPD	  (0x1 << AHCI_PxCMD_CPD_SHIFT)  /* Cold presence detection */
#define	AHCI_PxCMD_ESP	  (0x1 << AHCI_PxCMD_ESP_SHIFT)  /* External SATA port */
#define	AHCI_PxCMD_ATAPI  (0x1 << AHCI_PxCMD_ATAPI_SHIFT)/* Device is ATAPI */
#define	AHCI_PxCMD_DLAE	  (0x1 << AHCI_PxCMD_DLAE_SHIFT) /* Drive LED on ATAPI enable */
/* Aggressive link power magament enable */
#define	AHCI_PxCMD_ALPE	  (0x1 << AHCI_PxCMD_ALPE_SHIFT)
#define	AHCI_PxCMD_ASP	  (0x1 << AHCI_PxCMD_ASP_SHIFT)  /* Aggressive slumber/partial */
/* Interface communication control */
#define	AHCI_PxCMD_ICC	  (0xf << AHCI_PxCMD_ICC_SHIFT)

// P0SCTL and P0SSTS
#define AHCI_PxSCTL_PxSSTS_DET_SHIFT   (0)
#define AHCI_PxSCTL_PxSSTS_SPD_SHIFT   (4)
#define AHCI_PxSCTL_PxSSTS_IPM_SHIFT   (8)

#define AHCI_PxSCTL_PxSSTS_DET	       (0xf << AHCI_PxSCTL_PxSSTS_DET_SHIFT)
#define AHCI_PxSCTL_PxSSTS_SPD	       (0xf << AHCI_PxSCTL_PxSSTS_SPD_SHIFT)
#define AHCI_PxSCTL_PxSSTS_IPM	       (0xf << AHCI_PxSCTL_PxSSTS_IPM_SHIFT)

/*
     1   0    0    0    0     0    0    0    0    1    0    0    0    0    0    0

  |                   |                   |                   |                   |
    31   30   29   28   27   26   25   24   23   22   21   20   19   18   17   16
  +----+----+----+----+----+----+----+----+----+----+-----------------------------+
  |CPDS|TFES|HBFS|HBDS|IFS |INFS|RSV |OFS |IPMS|PRCS|          RESERVED           |
  +----+----+----+----+----+----+----+----+----+----+-----------------------------+


     0   0    0    0    0     0    0    0    1    1    0    0    0    0    0    0
  |                   |                   |                   |                   |
   15                                       07   06   05   04   03   02   01   00
  +---------------------------------------+----+----+----+----+----+----+----+----+
  |             RESERVED                  |DMPS|PCS |DPS |UFS |SDBS|DSS |PSS |DHRS|
  +---------------------------------------+----+----+----+----+----+----+----+----+

*/

// P0IS
#define AHCI_P0IS_DHRS_SHIFT     (0)
#define AHCI_P0IS_PSS_SHIFT      (1)
#define AHCI_P0IS_DSS_SHIFT      (2)
#define AHCI_P0IS_SDBS_SHIFT     (3)
#define AHCI_P0IS_UFS_SHIFT      (4)
#define AHCI_P0IS_DPS_SHIFT      (5)
#define AHCI_P0IS_PCS_SHIFT      (6)
#define AHCI_P0IS_DMPS_SHIFT     (7)
#define AHCI_P0IS_PRCS_SHIFT     (22)
#define AHCI_P0IS_IPMS_SHIFT     (23)
#define AHCI_P0IS_OFS_SHIFT      (24)
#define AHCI_P0IS_INFS_SHIFT     (26)
#define AHCI_P0IS_IFS_SHIFT      (27)
#define AHCI_P0IS_HBDS_SHIFT     (28)
#define AHCI_P0IS_HBFS_SHIFT     (29)
#define AHCI_P0IS_TFES_SHIFT     (30)
#define AHCI_P0IS_CPDS_SHIFT     (31)


#define AHCI_P0IS_DHRS           (0x1 << AHCI_P0IS_DHRS_SHIFT)
#define AHCI_P0IS_PSS            (0x1 << AHCI_P0IS_PSS_SHIFT)
#define AHCI_P0IS_DSS            (0x1 << AHCI_P0IS_DSS_SHIFT)
#define AHCI_P0IS_SDBS           (0x1 << AHCI_P0IS_SDBS_SHIFT)
#define AHCI_P0IS_UFS            (0x1 << AHCI_P0IS_UFS_SHIFT)
#define AHCI_P0IS_DPS            (0x1 << AHCI_P0IS_DPS_SHIFT)
#define AHCI_P0IS_PCS            (0x1 << AHCI_P0IS_PCS_SHIFT)
#define AHCI_P0IS_DMPS           (0x1 << AHCI_P0IS_DMPS_SHIFT)
#define AHCI_P0IS_PRCS           (0x1 << AHCI_P0IS_PRCS_SHIFT)
#define AHCI_P0IS_IPMS           (0x1 << AHCI_P0IS_IPMS_SHIFT)
#define AHCI_P0IS_OFS            (0x1 << AHCI_P0IS_OFS_SHIFT)
#define AHCI_P0IS_INFS           (0x1 << AHCI_P0IS_INFS_SHIFT)
#define AHCI_P0IS_IFS            (0x1 << AHCI_P0IS_IFS_SHIFT)
#define AHCI_P0IS_HBDS           (0x1 << AHCI_P0IS_HBDS_SHIFT)
#define AHCI_P0IS_HBFS           (0x1 << AHCI_P0IS_HBFS_SHIFT)
#define AHCI_P0IS_TFES           (0x1 << AHCI_P0IS_TFES_SHIFT)
#define AHCI_P0IS_CPDS           (0x1 << AHCI_P0IS_CPDS_SHIFT)

#define AHCI_P0IS_ALL_INTERRUPTS (0xFDC000FF)

// PxTFD
//   7     6     5     4     3     2     1    0
//+-----+-----------------+-----+----------+------+
//| BSY |       CS        | DRQ |    CS    | ERR  |
//+-----+-----------------+-----+----------+------+
// BSY=BUSY
// CS=Command Specific
// DRQ=Data Request
// ERR=Error
#define AHCI_PxTFD_STS_SHIFT             (0)
#define AHCI_PxTFD_STS_BSY_DRQ_ERR_SHIFT (0)
#define AHCI_PxTFD_STS_ERR_SHIFT         (0)
#define AHCI_PxTFD_STS_DRQ_SHIFT         (3)
#define AHCI_PxTFD_STS_BSY_SHIFT         (7)
#define AHCI_PxTFD_ERR_SHIFT             (8)

#define AHCI_PxTFD_STS                   (0xff<< AHCI_PxTFD_STS_SHIFT)
#define AHCI_PxTFD_STS_BSY_DRQ_ERR       (0x88<< AHCI_PxTFD_STS_BSY_DRQ_ERR_SHIFT)
#define AHCI_PxTFD_STS_ERR	              (0x1 << AHCI_PxTFD_STS_ERR_SHIFT)
#define AHCI_PxTFD_STS_DRQ	              (0x1 << AHCI_PxTFD_STS_DRQ_SHIFT)
#define AHCI_PxTFD_STS_BSY	              (0x1 << AHCI_PxTFD_STS_BSY_SHIFT)
#define AHCI_PxTFD_ERR                   (0xff<< AHCI_PxTFD_ERR_SHIFT)



// GHC
#define AHCI_GHC_HR_SHIFT   (0)
#define AHCI_GHC_IE_SHIFT   (1)
#define AHCI_GHC_AE_SHIFT   (31)

#define AHCI_GHC_HR        (0x1 << AHCI_GHC_HR_SHIFT) // HBA Reset/Release
#define AHCI_GHC_IE	        (0x1 << AHCI_GHC_IE_SHIFT) // Global Int Enable/Disable
#define AHCI_GHC_AE	        (0x1 << AHCI_GHC_AE_SHIFT) // AHCI/Legacy


// IE
#define AHCI_P0IE_DHRE_SHIFT (0)
#define AHCI_P0IE_PSE_SHIFT  (1)
#define AHCI_P0IE_DSE_SHIFT  (2)
#define AHCI_P0IE_SDBE_SHIFT (3)
#define AHCI_P0IE_UFE_SHIFT  (4)
#define AHCI_P0IE_DPE_SHIFT  (5)
#define AHCI_P0IE_PCE_SHIFT  (6)
#define AHCI_P0IE_DMPE_SHIFT (7)
#define AHCI_P0IE_PRCE_SHIFT (22)
#define AHCI_P0IE_IPME_SHIFT (23)
#define AHCI_P0IE_OFE_SHIFT  (24)
#define AHCI_P0IE_INFE_SHIFT (26)
#define AHCI_P0IE_IFE_SHIFT  (27)
#define AHCI_P0IE_HBDE_SHIFT (28)
#define AHCI_P0IE_HBFE_SHIFT (29)
#define AHCI_P0IE_TFEE_SHIFT (30)
#define AHCI_P0IE_CPDE_SHIFT (31)

#define AHCI_P0IE_DHRE       (0x1 << AHCI_P0IE_DHRE_SHIFT)
#define AHCI_P0IE_PSE        (0x1 << AHCI_P0IE_PSE_SHIFT)
#define AHCI_P0IE_DSE        (0x1 << AHCI_P0IE_DSE_SHIFT)
#define AHCI_P0IE_SDBE       (0x1 << AHCI_P0IE_SDBE_SHIFT)
#define AHCI_P0IE_UFE        (0x1 << AHCI_P0IE_UFE_SHIFT)
#define AHCI_P0IE_DPE        (0x1 << AHCI_P0IE_DPE_SHIFT)
#define AHCI_P0IE_PCE        (0x1 << AHCI_P0IE_PCE_SHIFT)
#define AHCI_P0IE_DMPE       (0x1 << AHCI_P0IE_DMPE_SHIFT)
#define AHCI_P0IE_PRCE       (0x1 << AHCI_P0IE_PRCE_SHIFT)
#define AHCI_P0IE_IPME       (0x1 << AHCI_P0IE_IPME_SHIFT)
#define AHCI_P0IE_OFE        (0x1 << AHCI_P0IE_OFE_SHIFT)
#define AHCI_P0IE_INFE       (0x1 << AHCI_P0IE_INFE_SHIFT)
#define AHCI_P0IE_IFE        (0x1 << AHCI_P0IE_IFE_SHIFT)
#define AHCI_P0IE_HBDE       (0x1 << AHCI_P0IE_HBDE_SHIFT)
#define AHCI_P0IE_HBFE       (0x1 << AHCI_P0IE_HBFE_SHIFT)
#define AHCI_P0IE_TFEE       (0x1 << AHCI_P0IE_TFEE_SHIFT)
#define AHCI_P0IE_CPDE       (0x1 << AHCI_P0IE_CPDE_SHIFT)
#define AHCI_P0IE_ALL_FIELDS (0xFDC000FF)

// PHY
#define AHCI_P0PHYCR_MPY_SHIFT       (0)
#define AHCI_P0PHYCR_LB_SHIFT        (4)
#define AHCI_P0PHYCR_LOS_SHIFT       (6)
#define AHCI_P0PHYCR_RXINVPOLE_SHIFT (7)
#define AHCI_P0PHYCR_RXTERM_SHIFT    (8)
#define AHCI_P0PHYCR_RXCDR_SHIFT     (10)
#define AHCI_P0PHYCR_RXEQ_SHIFT      (13)
#define AHCI_P0PHYCR_TXINVPOLE_SHIFT (17)
#define AHCI_P0PHYCR_TXCM_SHIFT      (18)
#define AHCI_P0PHYCR_TXSWING_SHIFT   (19)
#define AHCI_P0PHYCR_TXDE_SHIFT      (22)
#define AHCI_P0PHYCR_OVERRIDE_SHIFT  (30)
#define AHCI_P0PHYCR_ENPLL_SHIFT     (31)

#define AHCI_P0PHYCR_MPY		     (0xf << AHCI_P0PHYCR_MPY_SHIFT)
#define AHCI_P0PHYCR_LB			     (0x3 << AHCI_P0PHYCR_LB_SHIFT)
#define AHCI_P0PHYCR_LOS		     (0x1 << AHCI_P0PHYCR_LOS_SHIFT)
#define AHCI_P0PHYCR_RXINVPOLE	     (0x1 << AHCI_P0PHYCR_RXINVPOLE_SHIFT)
#define AHCI_P0PHYCR_RXTERM		     (0x3 << AHCI_P0PHYCR_RXTERM_SHIFT)
#define AHCI_P0PHYCR_RXCDR		     (0x7 << AHCI_P0PHYCR_RXCDR_SHIFT)
#define AHCI_P0PHYCR_RXEQ		     (0xf << AHCI_P0PHYCR_RXEQ_SHIFT)
#define AHCI_P0PHYCR_TXINVPOLE	     (0x1 << AHCI_P0PHYCR_TXINVPOLE_SHIFT)
#define AHCI_P0PHYCR_TXCM		     (0x1 << AHCI_P0PHYCR_TXCM_SHIFT)
#define AHCI_P0PHYCR_TXSWING	     (0x7 << AHCI_P0PHYCR_TXSWING_SHIFT)
#define AHCI_P0PHYCR_TXDE		     (0xf << AHCI_P0PHYCR_TXDE_SHIFT)
#define AHCI_P0PHYCR_OVERRIDE	     (0x1 << AHCI_P0PHYCR_OVERRIDE_SHIFT)
#define AHCI_P0PHYCR_ENPLL		     (0x1 << AHCI_P0PHYCR_ENPLL_SHIFT)

// DMA
#define AHCI_P0DMACR_TXTS_SHIFT      (0)
#define AHCI_P0DMACR_RXTS_SHIFT      (4)
#define AHCI_P0DMACR_TXABL_SHIFT     (8)
#define AHCI_P0DMACR_RXABL_SHIFT     (12)


#define AHCI_P0DMACR_TXTS		     (0xf << AHCI_P0DMACR_TXTS_SHIFT)
#define AHCI_P0DMACR_RXTS 		     (0xf << AHCI_P0DMACR_RXTS_SHIFT)
#define AHCI_P0DMACR_TXABL 		     (0xf << AHCI_P0DMACR_TXABL_SHIFT)
#define AHCI_P0DMACR_RXABL 		     (0xf << AHCI_P0DMACR_RXABL_SHIFT)

// ATA Commands
// Note: READ_DMA_QUEUED and WRITE_DMA_QUEUED Legacy Commands do not
//        work with AHCI Controller. Need to use strictly
//        FPDMA_READ and FPDMA_WRITE Command for Native Command Queuing.
#define ATA_CMD_READ_SECTOR          (0x20) // PIO Reaad Sector(s) Command
#define ATA_CMD_WRITE_SECTOR         (0x30) // PIO Write Sector(s) Command
#define ATA_CMD_READ_DMA_EXT         (0x25) // Read DMA Ext: Used when using 48-Bit Addressing
#define ATA_CMD_WRITE_DMA_EXT        (0x35) // Write DMA Ext: Used when using 48-Bit Addressing
#define ATA_CMD_READ_DMA             (0xC8) // Read DMA: Used when using 28-Bit Addressing
#define ATA_CMD_WRITE_DMA            (0xCA) // Write DMA: Used when using 28-Bit Addressing
#define ATA_CMD_READ_DMA_QUEUED      (0xC7) // Legacy Queued Read Command (Not to be used by AHCI)
#define ATA_CMD_WRITE_DMA_QUEUED     (0xCC) // Legacy Queued Write Command (Not to be used by AHCI)
#define ATA_CMD_FPDMA_READ           (0x60) // Queued Read Command: First Party DMA Read Cmd
#define ATA_CMD_FPDMA_WRITE          (0x61) // Queued Write Command: First Party DMA Write Cmd
#define ATA_CMD_IDENTIFY_DEVICE      (0xEC) // Receive Device Parameter info using PIO Read Protocol.
#define ATA_CMD_SET_FEATURES		 (0xEF)  // Set Feature Command (Used to Disable IORDY)
											 //  Invoking this Cmd to ensure proper operation of
											 //  SATA Core when invoking Non-Data Command.

// SUB Command of SET FEATURE to be written onot Features Reg.
#define ATA_SUB_CMD_XFER_MODE        (0x03)	// ATA Feature - Sub Command
#define SATA_FEATURE_ENABLE          (0x10) // SATA Feature - Sub Command
#define SATA_FEATURE_DISABLE         (0x90) // SATA Feature - Sbu Command

// Sector Count Register Contain Features to be Enabled/Disabled
#define ATA_SUB_CMD_XFER_MODE_VALUE  (0x01) // ATA Sub Command Specific
#define SATA_DEV_INITIATED_PWR_CAP   (0x03) // SATA Sub Command Specific


#define DEVICE_REG_USE_LBA_ADDRESSING (0x1 << 6) // This is Device Head Register not Control
#define CMDFIS_BYTE1_C_IS_CMD_UPDATE  (0x1 << 7)
#define CMDFIS_BYTE1_C_IS_CTRL_UPDATE (0x0 << 7)

// ATA Good Status Signature after Power Up or Port Reset
#define AHCI_P0SIG_SIG_ATA_DEV_GOOD_STAT     (0x00000101) // LBAhigh:LBAmid:LBAlow:SECcnt
#define AHCI_P0SIG_SIG_PACKET_DEV_GOOD_STAT  (0xEB140101) // LBAhigh:LBAmid:LBAlow:SECcnt
// Device
extern FirmwareCtrlFeatures swCtrlFeatures;

char placeHbaInIdle(void);
void waitForXms(UINT32);
char spinUpDeviceAndWaitForInitToComplete(void);
void setSataSpeed(UINT8);
void invokeHBAReset(void);
char chceckSysMemorySize(void);
void performFirmwareInit(void);
void initPhyAndDmaPars(void);

#endif
