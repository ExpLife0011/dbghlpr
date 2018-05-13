#ifndef __DEFINE_DBG_CORE__
#define __DEFINE_DBG_CORE__

typedef struct uuid_type {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} uuid_type;

typedef enum
{
	DBG_X86_REG_IDX_GS
	, DBG_X86_REG_IDX_FS
	, DBG_X86_REG_IDX_ES
	, DBG_X86_REG_IDX_DS
	, DBG_X86_REG_IDX_EDI
	, DBG_X86_REG_IDX_ESI
	, DBG_X86_REG_IDX_EBX
	, DBG_X86_REG_IDX_EDX
	, DBG_X86_REG_IDX_ECX
	, DBG_X86_REG_IDX_EAX
	, DBG_X86_REG_IDX_EBP
	, DBG_X86_REG_IDX_EIP
	, DBG_X86_REG_IDX_CS
	, DBG_X86_REG_IDX_EFL
	, DBG_X86_REG_IDX_ESP
	, DBG_X86_REG_IDX_SS
	, DBG_X86_REG_IDX_DR0
	, DBG_X86_REG_IDX_DR1
	, DBG_X86_REG_IDX_DR2
	, DBG_X86_REG_IDX_DR3
	, DBG_X86_REG_IDX_DR6
	, DBG_X86_REG_IDX_DR7
	, DBG_X86_REG_IDX_DI
	, DBG_X86_REG_IDX_SI
	, DBG_X86_REG_IDX_BX
	, DBG_X86_REG_IDX_DX
	, DBG_X86_REG_IDX_CX
	, DBG_X86_REG_IDX_AX
	, DBG_X86_REG_IDX_BP
	, DBG_X86_REG_IDX_IP
	, DBG_X86_REG_IDX_FL
	, DBG_X86_REG_IDX_SP
	, DBG_X86_REG_IDX_BL
	, DBG_X86_REG_IDX_DL
	, DBG_X86_REG_IDX_CL
	, DBG_X86_REG_IDX_AL
	, DBG_X86_REG_IDX_BH
	, DBG_X86_REG_IDX_DH
	, DBG_X86_REG_IDX_CH
	, DBG_X86_REG_IDX_AH
	, DBG_X86_REG_IDX_FPCW
	, DBG_X86_REG_IDX_FPSW
	, DBG_X86_REG_IDX_FPTW
	, DBG_X86_REG_IDX_FOPCODE
	, DBG_X86_REG_IDX_FPIP
	, DBG_X86_REG_IDX_FPIPSEL
	, DBG_X86_REG_IDX_FPDP
	, DBG_X86_REG_IDX_FPDPSEL
	, DBG_X86_REG_IDX_ST0
	, DBG_X86_REG_IDX_ST1
	, DBG_X86_REG_IDX_ST2
	, DBG_X86_REG_IDX_ST3
	, DBG_X86_REG_IDX_ST4
	, DBG_X86_REG_IDX_ST5
	, DBG_X86_REG_IDX_ST6
	, DBG_X86_REG_IDX_ST7
	, DBG_X86_REG_IDX_MM0
	, DBG_X86_REG_IDX_MM1
	, DBG_X86_REG_IDX_MM2
	, DBG_X86_REG_IDX_MM3
	, DBG_X86_REG_IDX_MM4
	, DBG_X86_REG_IDX_MM5
	, DBG_X86_REG_IDX_MM6
	, DBG_X86_REG_IDX_MM7
	, DBG_X86_REG_IDX_MXCSR
	, DBG_X86_REG_IDX_XMM0
	, DBG_X86_REG_IDX_XMM1
	, DBG_X86_REG_IDX_XMM2
	, DBG_X86_REG_IDX_XMM3
	, DBG_X86_REG_IDX_XMM4
	, DBG_X86_REG_IDX_XMM5
	, DBG_X86_REG_IDX_XMM6
	, DBG_X86_REG_IDX_XMM7
	, DBG_X86_REG_IDX_IOPL
	, DBG_X86_REG_IDX_OF
	, DBG_X86_REG_IDX_DF
	, DBG_X86_REG_IDX_IF
	, DBG_X86_REG_IDX_TF
	, DBG_X86_REG_IDX_SF
	, DBG_X86_REG_IDX_ZF
	, DBG_X86_REG_IDX_AF
	, DBG_X86_REG_IDX_PF
	, DBG_X86_REG_IDX_CF
	, DBG_X86_REG_IDX_VIP
	, DBG_X86_REG_IDX_VIF
	, DBG_X86_REG_IDX_XMM0L
	, DBG_X86_REG_IDX_XMM1L
	, DBG_X86_REG_IDX_XMM2L
	, DBG_X86_REG_IDX_XMM3L
	, DBG_X86_REG_IDX_XMM4L
	, DBG_X86_REG_IDX_XMM5L
	, DBG_X86_REG_IDX_XMM6L
	, DBG_X86_REG_IDX_XMM7L
	, DBG_X86_REG_IDX_XMM0H
	, DBG_X86_REG_IDX_XMM1H
	, DBG_X86_REG_IDX_XMM2H
	, DBG_X86_REG_IDX_XMM3H
	, DBG_X86_REG_IDX_XMM4H
	, DBG_X86_REG_IDX_XMM5H
	, DBG_X86_REG_IDX_XMM6H
	, DBG_X86_REG_IDX_XMM7H
	, DBG_X86_REG_IDX_XMM0D0
	, DBG_X86_REG_IDX_XMM0D1
	, DBG_X86_REG_IDX_XMM0D2
	, DBG_X86_REG_IDX_XMM0D3
	, DBG_X86_REG_IDX_XMM1D0
	, DBG_X86_REG_IDX_XMM1D1
	, DBG_X86_REG_IDX_XMM1D2
	, DBG_X86_REG_IDX_XMM1D3
	, DBG_X86_REG_IDX_XMM2D0
	, DBG_X86_REG_IDX_XMM2D1
	, DBG_X86_REG_IDX_XMM2D2
	, DBG_X86_REG_IDX_XMM2D3
	, DBG_X86_REG_IDX_XMM3D0
	, DBG_X86_REG_IDX_XMM3D1
	, DBG_X86_REG_IDX_XMM3D2
	, DBG_X86_REG_IDX_XMM3D3
	, DBG_X86_REG_IDX_XMM4D0
	, DBG_X86_REG_IDX_XMM4D1
	, DBG_X86_REG_IDX_XMM4D2
	, DBG_X86_REG_IDX_XMM4D3
	, DBG_X86_REG_IDX_XMM5D0
	, DBG_X86_REG_IDX_XMM5D1
	, DBG_X86_REG_IDX_XMM5D2
	, DBG_X86_REG_IDX_XMM5D3
	, DBG_X86_REG_IDX_XMM6D0
	, DBG_X86_REG_IDX_XMM6D1
	, DBG_X86_REG_IDX_XMM6D2
	, DBG_X86_REG_IDX_XMM6D3
	, DBG_X86_REG_IDX_XMM7D0
	, DBG_X86_REG_IDX_XMM7D1
	, DBG_X86_REG_IDX_XMM7D2
	, DBG_X86_REG_IDX_XMM7D3
	, DBG_X86_REG_IDX_YMM0
	, DBG_X86_REG_IDX_YMM1
	, DBG_X86_REG_IDX_YMM2
	, DBG_X86_REG_IDX_YMM3
	, DBG_X86_REG_IDX_YMM4
	, DBG_X86_REG_IDX_YMM5
	, DBG_X86_REG_IDX_YMM6
	, DBG_X86_REG_IDX_YMM7
	, DBG_X86_REG_IDX_YMM0L
	, DBG_X86_REG_IDX_YMM1L
	, DBG_X86_REG_IDX_YMM2L
	, DBG_X86_REG_IDX_YMM3L
	, DBG_X86_REG_IDX_YMM4L
	, DBG_X86_REG_IDX_YMM5L
	, DBG_X86_REG_IDX_YMM6L
	, DBG_X86_REG_IDX_YMM7L
	, DBG_X86_REG_IDX_YMM0H
	, DBG_X86_REG_IDX_YMM1H
	, DBG_X86_REG_IDX_YMM2H
	, DBG_X86_REG_IDX_YMM3H
	, DBG_X86_REG_IDX_YMM4H
	, DBG_X86_REG_IDX_YMM5H
	, DBG_X86_REG_IDX_YMM6H
	, DBG_X86_REG_IDX_YMM7H
	, DBG_X86_REG_IDX_YMM0D0
	, DBG_X86_REG_IDX_YMM0D1
	, DBG_X86_REG_IDX_YMM0D2
	, DBG_X86_REG_IDX_YMM0D3
	, DBG_X86_REG_IDX_YMM1D0
	, DBG_X86_REG_IDX_YMM1D1
	, DBG_X86_REG_IDX_YMM1D2
	, DBG_X86_REG_IDX_YMM1D3
	, DBG_X86_REG_IDX_YMM2D0
	, DBG_X86_REG_IDX_YMM2D1
	, DBG_X86_REG_IDX_YMM2D2
	, DBG_X86_REG_IDX_YMM2D3
	, DBG_X86_REG_IDX_YMM3D0
	, DBG_X86_REG_IDX_YMM3D1
	, DBG_X86_REG_IDX_YMM3D2
	, DBG_X86_REG_IDX_YMM3D3
	, DBG_X86_REG_IDX_YMM4D0
	, DBG_X86_REG_IDX_YMM4D1
	, DBG_X86_REG_IDX_YMM4D2
	, DBG_X86_REG_IDX_YMM4D3
	, DBG_X86_REG_IDX_YMM5D0
	, DBG_X86_REG_IDX_YMM5D1
	, DBG_X86_REG_IDX_YMM5D2
	, DBG_X86_REG_IDX_YMM5D3
	, DBG_X86_REG_IDX_YMM6D0
	, DBG_X86_REG_IDX_YMM6D1
	, DBG_X86_REG_IDX_YMM6D2
	, DBG_X86_REG_IDX_YMM6D3
	, DBG_X86_REG_IDX_YMM7D0
	, DBG_X86_REG_IDX_YMM7D1
	, DBG_X86_REG_IDX_YMM7D2
	, DBG_X86_REG_IDX_YMM7D3
}DBG_X86_REG_IDX;

typedef enum
{
	DBG_X64_REG_IDX_RAX
	, DBG_X64_REG_IDX_RCX
	, DBG_X64_REG_IDX_RDX
	, DBG_X64_REG_IDX_RBX
	, DBG_X64_REG_IDX_RSP
	, DBG_X64_REG_IDX_RBP
	, DBG_X64_REG_IDX_RSI
	, DBG_X64_REG_IDX_RDI
	, DBG_X64_REG_IDX_R8
	, DBG_X64_REG_IDX_R9
	, DBG_X64_REG_IDX_R10
	, DBG_X64_REG_IDX_R11
	, DBG_X64_REG_IDX_R12
	, DBG_X64_REG_IDX_R13
	, DBG_X64_REG_IDX_R14
	, DBG_X64_REG_IDX_R15
	, DBG_X64_REG_IDX_RIP
	, DBG_X64_REG_IDX_EFL
	, DBG_X64_REG_IDX_CS
	, DBG_X64_REG_IDX_DS
	, DBG_X64_REG_IDX_ES
	, DBG_X64_REG_IDX_FS
	, DBG_X64_REG_IDX_GS
	, DBG_X64_REG_IDX_SS
	, DBG_X64_REG_IDX_DR0
	, DBG_X64_REG_IDX_DR1
	, DBG_X64_REG_IDX_DR2
	, DBG_X64_REG_IDX_DR3
	, DBG_X64_REG_IDX_DR6
	, DBG_X64_REG_IDX_DR7
	, DBG_X64_REG_IDX_FPCW
	, DBG_X64_REG_IDX_FPSW
	, DBG_X64_REG_IDX_FPTW
	, DBG_X64_REG_IDX_ST0
	, DBG_X64_REG_IDX_ST1
	, DBG_X64_REG_IDX_ST2
	, DBG_X64_REG_IDX_ST3
	, DBG_X64_REG_IDX_ST4
	, DBG_X64_REG_IDX_ST5
	, DBG_X64_REG_IDX_ST6
	, DBG_X64_REG_IDX_ST7
	, DBG_X64_REG_IDX_MM0
	, DBG_X64_REG_IDX_MM1
	, DBG_X64_REG_IDX_MM2
	, DBG_X64_REG_IDX_MM3
	, DBG_X64_REG_IDX_MM4
	, DBG_X64_REG_IDX_MM5
	, DBG_X64_REG_IDX_MM6
	, DBG_X64_REG_IDX_MM7
	, DBG_X64_REG_IDX_MXCSR
	, DBG_X64_REG_IDX_XMM0
	, DBG_X64_REG_IDX_XMM1
	, DBG_X64_REG_IDX_XMM2
	, DBG_X64_REG_IDX_XMM3
	, DBG_X64_REG_IDX_XMM4
	, DBG_X64_REG_IDX_XMM5
	, DBG_X64_REG_IDX_XMM6
	, DBG_X64_REG_IDX_XMM7
	, DBG_X64_REG_IDX_XMM8
	, DBG_X64_REG_IDX_XMM9
	, DBG_X64_REG_IDX_XMM10
	, DBG_X64_REG_IDX_XMM11
	, DBG_X64_REG_IDX_XMM12
	, DBG_X64_REG_IDX_XMM13
	, DBG_X64_REG_IDX_XMM14
	, DBG_X64_REG_IDX_XMM15
	, DBG_X64_REG_IDX_XMM0D0
	, DBG_X64_REG_IDX_XMM0D1
	, DBG_X64_REG_IDX_XMM0D2
	, DBG_X64_REG_IDX_XMM0D3
	, DBG_X64_REG_IDX_XMM1D0
	, DBG_X64_REG_IDX_XMM1D1
	, DBG_X64_REG_IDX_XMM1D2
	, DBG_X64_REG_IDX_XMM1D3
	, DBG_X64_REG_IDX_XMM2D0
	, DBG_X64_REG_IDX_XMM2D1
	, DBG_X64_REG_IDX_XMM2D2
	, DBG_X64_REG_IDX_XMM2D3
	, DBG_X64_REG_IDX_XMM3D0
	, DBG_X64_REG_IDX_XMM3D1
	, DBG_X64_REG_IDX_XMM3D2
	, DBG_X64_REG_IDX_XMM3D3
	, DBG_X64_REG_IDX_XMM4D0
	, DBG_X64_REG_IDX_XMM4D1
	, DBG_X64_REG_IDX_XMM4D2
	, DBG_X64_REG_IDX_XMM4D3
	, DBG_X64_REG_IDX_XMM5D0
	, DBG_X64_REG_IDX_XMM5D1
	, DBG_X64_REG_IDX_XMM5D2
	, DBG_X64_REG_IDX_XMM5D3
	, DBG_X64_REG_IDX_XMM6D0
	, DBG_X64_REG_IDX_XMM6D1
	, DBG_X64_REG_IDX_XMM6D2
	, DBG_X64_REG_IDX_XMM6D3
	, DBG_X64_REG_IDX_XMM7D0
	, DBG_X64_REG_IDX_XMM7D1
	, DBG_X64_REG_IDX_XMM7D2
	, DBG_X64_REG_IDX_XMM7D3
	, DBG_X64_REG_IDX_XMM8D0
	, DBG_X64_REG_IDX_XMM8D1
	, DBG_X64_REG_IDX_XMM8D2
	, DBG_X64_REG_IDX_XMM8D3
	, DBG_X64_REG_IDX_XMM9D0
	, DBG_X64_REG_IDX_XMM9D1
	, DBG_X64_REG_IDX_XMM9D2
	, DBG_X64_REG_IDX_XMM9D3
	, DBG_X64_REG_IDX_XMM10D0
	, DBG_X64_REG_IDX_XMM10D1
	, DBG_X64_REG_IDX_XMM10D2
	, DBG_X64_REG_IDX_XMM10D3
	, DBG_X64_REG_IDX_XMM11D0
	, DBG_X64_REG_IDX_XMM11D1
	, DBG_X64_REG_IDX_XMM11D2
	, DBG_X64_REG_IDX_XMM11D3
	, DBG_X64_REG_IDX_XMM12D0
	, DBG_X64_REG_IDX_XMM12D1
	, DBG_X64_REG_IDX_XMM12D2
	, DBG_X64_REG_IDX_XMM12D3
	, DBG_X64_REG_IDX_XMM13D0
	, DBG_X64_REG_IDX_XMM13D1
	, DBG_X64_REG_IDX_XMM13D2
	, DBG_X64_REG_IDX_XMM13D3
	, DBG_X64_REG_IDX_XMM14D0
	, DBG_X64_REG_IDX_XMM14D1
	, DBG_X64_REG_IDX_XMM14D2
	, DBG_X64_REG_IDX_XMM14D3
	, DBG_X64_REG_IDX_XMM15D0
	, DBG_X64_REG_IDX_XMM15D1
	, DBG_X64_REG_IDX_XMM15D2
	, DBG_X64_REG_IDX_XMM15D3
	, DBG_X64_REG_IDX_XMM0L
	, DBG_X64_REG_IDX_XMM1L
	, DBG_X64_REG_IDX_XMM2L
	, DBG_X64_REG_IDX_XMM3L
	, DBG_X64_REG_IDX_XMM4L
	, DBG_X64_REG_IDX_XMM5L
	, DBG_X64_REG_IDX_XMM6L
	, DBG_X64_REG_IDX_XMM7L
	, DBG_X64_REG_IDX_XMM8L
	, DBG_X64_REG_IDX_XMM9L
	, DBG_X64_REG_IDX_XMM10L
	, DBG_X64_REG_IDX_XMM11L
	, DBG_X64_REG_IDX_XMM12L
	, DBG_X64_REG_IDX_XMM13L
	, DBG_X64_REG_IDX_XMM14L
	, DBG_X64_REG_IDX_XMM15L
	, DBG_X64_REG_IDX_XMM0H
	, DBG_X64_REG_IDX_XMM1H
	, DBG_X64_REG_IDX_XMM2H
	, DBG_X64_REG_IDX_XMM3H
	, DBG_X64_REG_IDX_XMM4H
	, DBG_X64_REG_IDX_XMM5H
	, DBG_X64_REG_IDX_XMM6H
	, DBG_X64_REG_IDX_XMM7H
	, DBG_X64_REG_IDX_XMM8H
	, DBG_X64_REG_IDX_XMM9H
	, DBG_X64_REG_IDX_XMM10H
	, DBG_X64_REG_IDX_XMM11H
	, DBG_X64_REG_IDX_XMM12H
	, DBG_X64_REG_IDX_XMM13H
	, DBG_X64_REG_IDX_XMM14H
	, DBG_X64_REG_IDX_XMM15H
	, DBG_X64_REG_IDX_YMM0
	, DBG_X64_REG_IDX_YMM1
	, DBG_X64_REG_IDX_YMM2
	, DBG_X64_REG_IDX_YMM3
	, DBG_X64_REG_IDX_YMM4
	, DBG_X64_REG_IDX_YMM5
	, DBG_X64_REG_IDX_YMM6
	, DBG_X64_REG_IDX_YMM7
	, DBG_X64_REG_IDX_YMM8
	, DBG_X64_REG_IDX_YMM9
	, DBG_X64_REG_IDX_YMM10
	, DBG_X64_REG_IDX_YMM11
	, DBG_X64_REG_IDX_YMM12
	, DBG_X64_REG_IDX_YMM13
	, DBG_X64_REG_IDX_YMM14
	, DBG_X64_REG_IDX_YMM15
	, DBG_X64_REG_IDX_YMM0D0
	, DBG_X64_REG_IDX_YMM0D1
	, DBG_X64_REG_IDX_YMM0D2
	, DBG_X64_REG_IDX_YMM0D3
	, DBG_X64_REG_IDX_YMM1D0
	, DBG_X64_REG_IDX_YMM1D1
	, DBG_X64_REG_IDX_YMM1D2
	, DBG_X64_REG_IDX_YMM1D3
	, DBG_X64_REG_IDX_YMM2D0
	, DBG_X64_REG_IDX_YMM2D1
	, DBG_X64_REG_IDX_YMM2D2
	, DBG_X64_REG_IDX_YMM2D3
	, DBG_X64_REG_IDX_YMM3D0
	, DBG_X64_REG_IDX_YMM3D1
	, DBG_X64_REG_IDX_YMM3D2
	, DBG_X64_REG_IDX_YMM3D3
	, DBG_X64_REG_IDX_YMM4D0
	, DBG_X64_REG_IDX_YMM4D1
	, DBG_X64_REG_IDX_YMM4D2
	, DBG_X64_REG_IDX_YMM4D3
	, DBG_X64_REG_IDX_YMM5D0
	, DBG_X64_REG_IDX_YMM5D1
	, DBG_X64_REG_IDX_YMM5D2
	, DBG_X64_REG_IDX_YMM5D3
	, DBG_X64_REG_IDX_YMM6D0
	, DBG_X64_REG_IDX_YMM6D1
	, DBG_X64_REG_IDX_YMM6D2
	, DBG_X64_REG_IDX_YMM6D3
	, DBG_X64_REG_IDX_YMM7D0
	, DBG_X64_REG_IDX_YMM7D1
	, DBG_X64_REG_IDX_YMM7D2
	, DBG_X64_REG_IDX_YMM7D3
	, DBG_X64_REG_IDX_YMM8D0
	, DBG_X64_REG_IDX_YMM8D1
	, DBG_X64_REG_IDX_YMM8D2
	, DBG_X64_REG_IDX_YMM8D3
	, DBG_X64_REG_IDX_YMM9D0
	, DBG_X64_REG_IDX_YMM9D1
	, DBG_X64_REG_IDX_YMM9D2
	, DBG_X64_REG_IDX_YMM9D3
	, DBG_X64_REG_IDX_YMM10D0
	, DBG_X64_REG_IDX_YMM10D1
	, DBG_X64_REG_IDX_YMM10D2
	, DBG_X64_REG_IDX_YMM10D3
	, DBG_X64_REG_IDX_YMM11D0
	, DBG_X64_REG_IDX_YMM11D1
	, DBG_X64_REG_IDX_YMM11D2
	, DBG_X64_REG_IDX_YMM11D3
	, DBG_X64_REG_IDX_YMM12D0
	, DBG_X64_REG_IDX_YMM12D1
	, DBG_X64_REG_IDX_YMM12D2
	, DBG_X64_REG_IDX_YMM12D3
	, DBG_X64_REG_IDX_YMM13D0
	, DBG_X64_REG_IDX_YMM13D1
	, DBG_X64_REG_IDX_YMM13D2
	, DBG_X64_REG_IDX_YMM13D3
	, DBG_X64_REG_IDX_YMM14D0
	, DBG_X64_REG_IDX_YMM14D1
	, DBG_X64_REG_IDX_YMM14D2
	, DBG_X64_REG_IDX_YMM14D3
	, DBG_X64_REG_IDX_YMM15D0
	, DBG_X64_REG_IDX_YMM15D1
	, DBG_X64_REG_IDX_YMM15D2
	, DBG_X64_REG_IDX_YMM15D3
	, DBG_X64_REG_IDX_YMM0L
	, DBG_X64_REG_IDX_YMM1L
	, DBG_X64_REG_IDX_YMM2L
	, DBG_X64_REG_IDX_YMM3L
	, DBG_X64_REG_IDX_YMM4L
	, DBG_X64_REG_IDX_YMM5L
	, DBG_X64_REG_IDX_YMM6L
	, DBG_X64_REG_IDX_YMM7L
	, DBG_X64_REG_IDX_YMM8L
	, DBG_X64_REG_IDX_YMM9L
	, DBG_X64_REG_IDX_YMM10L
	, DBG_X64_REG_IDX_YMM11L
	, DBG_X64_REG_IDX_YMM12L
	, DBG_X64_REG_IDX_YMM13L
	, DBG_X64_REG_IDX_YMM14L
	, DBG_X64_REG_IDX_YMM15L
	, DBG_X64_REG_IDX_YMM0H
	, DBG_X64_REG_IDX_YMM1H
	, DBG_X64_REG_IDX_YMM2H
	, DBG_X64_REG_IDX_YMM3H
	, DBG_X64_REG_IDX_YMM4H
	, DBG_X64_REG_IDX_YMM5H
	, DBG_X64_REG_IDX_YMM6H
	, DBG_X64_REG_IDX_YMM7H
	, DBG_X64_REG_IDX_YMM8H
	, DBG_X64_REG_IDX_YMM9H
	, DBG_X64_REG_IDX_YMM10H
	, DBG_X64_REG_IDX_YMM11H
	, DBG_X64_REG_IDX_YMM12H
	, DBG_X64_REG_IDX_YMM13H
	, DBG_X64_REG_IDX_YMM14H
	, DBG_X64_REG_IDX_YMM15H
	, DBG_X64_REG_IDX_EXFORM
	, DBG_X64_REG_IDX_EXTO
	, DBG_X64_REG_IDX_BRFROM
	, DBG_X64_REG_IDX_BRTO
	, DBG_X64_REG_IDX_EAX
	, DBG_X64_REG_IDX_ECX
	, DBG_X64_REG_IDX_EDX
	, DBG_X64_REG_IDX_EBX
	, DBG_X64_REG_IDX_ESP
	, DBG_X64_REG_IDX_EBP
	, DBG_X64_REG_IDX_ESI
	, DBG_X64_REG_IDX_EDI
	, DBG_X64_REG_IDX_R8D
	, DBG_X64_REG_IDX_R9D
	, DBG_X64_REG_IDX_R10D
	, DBG_X64_REG_IDX_R11D
	, DBG_X64_REG_IDX_R12D
	, DBG_X64_REG_IDX_R13D
	, DBG_X64_REG_IDX_R14D
	, DBG_X64_REG_IDX_R15D
	, DBG_X64_REG_IDX_EIP
	, DBG_X64_REG_IDX_AX
	, DBG_X64_REG_IDX_CX
	, DBG_X64_REG_IDX_DX
	, DBG_X64_REG_IDX_BX
	, DBG_X64_REG_IDX_SP
	, DBG_X64_REG_IDX_BP
	, DBG_X64_REG_IDX_SI
	, DBG_X64_REG_IDX_DI
	, DBG_X64_REG_IDX_R8W
	, DBG_X64_REG_IDX_R9W
	, DBG_X64_REG_IDX_R10W
	, DBG_X64_REG_IDX_R11W
	, DBG_X64_REG_IDX_R12W
	, DBG_X64_REG_IDX_R13W
	, DBG_X64_REG_IDX_R14W
	, DBG_X64_REG_IDX_R15W
	, DBG_X64_REG_IDX_IP
	, DBG_X64_REG_IDX_FL
	, DBG_X64_REG_IDX_AL
	, DBG_X64_REG_IDX_CL
	, DBG_X64_REG_IDX_DL
	, DBG_X64_REG_IDX_BL
	, DBG_X64_REG_IDX_SPL
	, DBG_X64_REG_IDX_BPL
	, DBG_X64_REG_IDX_SIL
	, DBG_X64_REG_IDX_DIL
	, DBG_X64_REG_IDX_R8B
	, DBG_X64_REG_IDX_R9B
	, DBG_X64_REG_IDX_R10B
	, DBG_X64_REG_IDX_R11B
	, DBG_X64_REG_IDX_R12B
	, DBG_X64_REG_IDX_R13B
	, DBG_X64_REG_IDX_R14B
	, DBG_X64_REG_IDX_R15B
	, DBG_X64_REG_IDX_AH
	, DBG_X64_REG_IDX_CH
	, DBG_X64_REG_IDX_DH
	, DBG_X64_REG_IDX_BH
	, DBG_X64_REG_IDX_IOPL
	, DBG_X64_REG_IDX_OF
	, DBG_X64_REG_IDX_DF
	, DBG_X64_REG_IDX_IF
	, DBG_X64_REG_IDX_TF
	, DBG_X64_REG_IDX_SF
	, DBG_X64_REG_IDX_ZF
	, DBG_X64_REG_IDX_AF
	, DBG_X64_REG_IDX_PF
	, DBG_X64_REG_IDX_CF
	, DBG_X64_REG_IDX_VIP
	, DBG_X64_REG_IDX_VIF
}DBG_X64_REG_IDX;

typedef struct __DBG_THREAD_CONTEXT
{
	unsigned long long rax;
	unsigned long long rcx;
	unsigned long long rdx;
	unsigned long long rbx;

	unsigned long long rsp;
	unsigned long long rbp;

	unsigned long long rsi;
	unsigned long long rdi;

	unsigned long long r8;
	unsigned long long r9;
	unsigned long long r10;
	unsigned long long r11;
	unsigned long long r12;
	unsigned long long r13;
	unsigned long long r14;
	unsigned long long r15;

	unsigned long long rip;

	unsigned long efl;

	unsigned long cs;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
	unsigned long ss;

	unsigned long long dr0;
	unsigned long long dr1;
	unsigned long long dr2;
	unsigned long long dr3;
	unsigned long long dr6;
	unsigned long long dr7;

	unsigned long long fpcw;
	unsigned long long fpsw;
	unsigned long long fptw;

	unsigned long long st0;
	unsigned long long st1;
	unsigned long long st2;
	unsigned long long st3;
	unsigned long long st4;
	unsigned long long st5;
	unsigned long long st6;
	unsigned long long st7;

	unsigned long long mm0;
	unsigned long long mm1;
	unsigned long long mm2;
	unsigned long long mm3;
	unsigned long long mm4;
	unsigned long long mm5;
	unsigned long long mm6;
	unsigned long long mm7;

	unsigned long long mxcsr;

	unsigned long long xmm0;
	unsigned long long xmm1;
	unsigned long long xmm2;
	unsigned long long xmm3;
	unsigned long long xmm4;
	unsigned long long xmm5;
	unsigned long long xmm6;
	unsigned long long xmm7;
	unsigned long long xmm8;
	unsigned long long xmm9;
	unsigned long long xmm10;
	unsigned long long xmm11;
	unsigned long long xmm12;
	unsigned long long xmm13;
	unsigned long long xmm14;
	unsigned long long xmm15;

	unsigned long long ymm0;
	unsigned long long ymm1;
	unsigned long long ymm2;
	unsigned long long ymm3;
	unsigned long long ymm4;
	unsigned long long ymm5;
	unsigned long long ymm6;
	unsigned long long ymm7;
	unsigned long long ymm8;
	unsigned long long ymm9;
	unsigned long long ymm10;
	unsigned long long ymm11;
	unsigned long long ymm12;
	unsigned long long ymm13;
	unsigned long long ymm14;
	unsigned long long ymm15;

	unsigned long long iopl;
	unsigned long long vip;
	unsigned long long vif;
}cpu_context_type, *cpu_context_type_ptr;

//
//
//
#define DBGENG_DEBUGCONTROL_ID	0

namespace dbg
{
	class __declspec(dllexport) core // api core
	{
	public :
		virtual ~core() {}

		static dbg::core *create(uuid_type id);

		virtual void *get_instance(unsigned long id) = 0;
		virtual void get_uuid(uuid_type *iid) = 0;

		virtual bool open(char *path) = 0;
		virtual bool open(unsigned long pid) = 0;

		virtual bool query_virtual_memory(unsigned long long virtual_address, void *out_memory_info) = 0;
		virtual unsigned long __stdcall read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size) = 0;

		virtual bool __stdcall get_thread_context(cpu_context_type *context) = 0;
		virtual bool get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp) = 0;
	};

	class __declspec(dllexport) util
	{
	public:
		virtual ~util() {}

		static dbg::util *create(uuid_type id);

		///
		typedef struct _tag_cs_x86_operand_information
		{
			long operand_type;
			long long value;
		}cs_operand;

		typedef struct _tag_cs_x86_context_type
		{
			unsigned long instruction_id;
			unsigned long instruction_size;
			unsigned long instruction_group;

			unsigned long operand_count;
			cs_operand operands[8];

			unsigned char dump[16];
		}cs_x86_context;
		///
		typedef struct _tag_dbgeng_disasm_context_type
		{
			dbg::core *c;

			char *buffer;
			unsigned long size_of_buffer;

			unsigned long *size_of_name;
			unsigned long long *next_address;
		}dbgeng_disasm_context;
		///

		virtual void get_uuid(uuid_type *iid) = 0;

		virtual bool open(unsigned long arch, unsigned long mode) = 0;
		virtual bool disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size) = 0;
	};

#define IID_DBGENG_CORE		0
#define IID_WINAPI_CORE		1
#define IID_REMOTEAPI_CORE	2

#define IID_CS_UTIL			3
#define IID_DBGENG_UTIL		4

#define IID_DRVAPI_CORE		5

	namespace linker
	{
		namespace api
		{
			__declspec(dllexport) dbg::core *create(unsigned long id);

			__declspec(dllexport) bool open(dbg::core *c, char *path);
			__declspec(dllexport) bool open(dbg::core *c, unsigned long pid);

			__declspec(dllexport) bool query_virtual_memory(dbg::core *c, unsigned long long virtual_address, void *out_memory_info);
			__declspec(dllexport) unsigned long __stdcall read_virtual_memory(dbg::core *c, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size);

			__declspec(dllexport) bool __stdcall get_thread_context(dbg::core *c, cpu_context_type *context);
			__declspec(dllexport) bool get_symbol_name(dbg::core *c, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp);

			//
			//
			//
			__declspec(dllexport) bool open(unsigned long iid, char *path);
			__declspec(dllexport) bool open(unsigned long iid, unsigned long pid);

			__declspec(dllexport) bool query_virtual_memory(unsigned long iid, unsigned long long virtual_address, void *out_memory_info);
			__declspec(dllexport) unsigned long __stdcall read_virtual_memory(unsigned long iid, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size);

			__declspec(dllexport) bool __stdcall get_thread_context(unsigned long iid, cpu_context_type *context);
			__declspec(dllexport) bool get_symbol_name(unsigned long iid, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp);
		}

		namespace util
		{
			__declspec(dllexport) dbg::util *create(unsigned long id);
		}
	}
}

#endif