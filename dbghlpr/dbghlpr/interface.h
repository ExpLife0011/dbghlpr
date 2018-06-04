#ifndef __DEFINE_DBG_CORE__
#define __DEFINE_DBG_CORE__

#include <list>
#include <map>
#include <set>

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

///
///
///
#define __IN
#define __OUT

namespace dbg
{
	typedef enum _tag_interface_code
	{
		IID_DEFAULT_API_CORE = 0xC0000001,
		IID_WINAPI_CORE, // n
		IID_REMOTEAPI_CORE, // n
		IID_DISASSEMBLER_UTIL1,
		IID_DEFAULT_UTIL,
		IID_DRVAPI_CORE, // n
		IID_EMULATOR_API_CORE,
		IID_DISASSEMBLER_UTIL2,
		IID_EMULATOR_UTIL
	}CORE_IID;

	typedef enum _tag_end_code
	{
		X86_END_CODE_ERR = 0xC0DE000E,
		X86_END_CODE_RET = 0xC0DE0000,
		X86_END_CODE_JMP,
		X86_END_CODE_NUL
	}END_CODE;

	typedef enum _tag_object_id
	{
		TARGET_CONTROLLER_ID = 0xD000001,
		TARGET_PROCESS_HANDLE_ID,
		TARGET_THREAD_HANDLE_ID,
		EMULATOR_HANDLE_ID
	}OBJECT_ID;

	typedef enum _tag_emulator_error_code
	{
		EM_CODE_ERR = 0xEC000001,
		EM_READ_ERR,
		EM_WRITE_ERR,
		EM_FETCH_ERR
	}EMULATOR_ERR;

	typedef enum _tag_processor_type
	{
		X86_PROCESSOR_BIT_TYPE = 32,
		X64_PROCESSOR_BIT_TYPE = 64
	}PROCESSOR_TYPE;

	///
	/// interface object
	///
	class object
	{
	public :
		virtual void get_uuid(uuid_type *iid) = 0;
	};

	typedef object * api_handle;
	typedef object * util_handle;

	///
	/// api core
	/// 
	class  api : public object // api core
	{
	public :
		typedef struct _tag_stack_frame_type
		{
			dbg::PROCESSOR_TYPE type;

			unsigned long long instruction_offset;
			unsigned long long return_offset;
			unsigned long long frame_offset;
			unsigned long long stack_offset;
			unsigned long long func_table_entry;
			unsigned long long params[4];
			unsigned long long reserved[6];
			int bool_virtual;
			unsigned long frame_number;
		} stack_frame_type, *stack_frame_type_ptr;

		typedef struct _tag_uc_memory_info_type
		{
			unsigned long long base_address;
			unsigned long long end_address;
		}uc_memory_t;

		virtual ~api() {}

		static dbg::api *create(uuid_type id);

		virtual void *get_object(unsigned long id) = 0;
		virtual void get_uuid(uuid_type *iid) = 0;

		virtual bool open(char *path) = 0;
		virtual bool open(unsigned long ctx) = 0;

		virtual bool close() = 0;

		virtual bool get_thread_id_list(std::list<unsigned long> &tid_list) = 0;
		virtual bool select_thread(unsigned long tid) = 0;
		virtual unsigned long current_thread_id() = 0;

		virtual void * virtual_alloc(unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type) = 0;
		virtual int virtual_free(void *ptr, unsigned long size, unsigned long free_type) = 0;

		virtual bool query_virtual_memory(unsigned long long virtual_address, void *out_memory_info) = 0;

		virtual unsigned long __stdcall read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size) = 0;
		virtual unsigned long __stdcall write_virtual_memory(unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size) = 0;

		virtual bool __stdcall get_thread_context(cpu_context_type *context) = 0;
		virtual bool __stdcall set_thread_context(cpu_context_type *context) = 0;

		virtual bool get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp) = 0;
		virtual bool stack_trace(unsigned long tid, stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count) = 0;

		virtual bool suspend(unsigned long tid) = 0;
		virtual bool resume(unsigned long tid) = 0;

		virtual bool write_dump(char *path) = 0;
	};

	///
	/// util
	///
	class util : public object
	{
	public:
		virtual ~util() {}

		static dbg::util *create(uuid_type id);

		//
		typedef struct _tag_x86_operand_information_type
		{
			__OUT long operand_type;
			__OUT long long value;
		}operand_t;

		typedef struct _tag_x86_disasm_context_type
		{
			__OUT unsigned long instruction_id;
			__OUT unsigned long instruction_size;
			__OUT unsigned long instruction_group;

			__OUT unsigned long operand_count;
			__OUT operand_t operands[8];
		}x86_disasm_context_type;

		typedef struct _tag_code_block_type
		{
			std::map<unsigned long long, x86_disasm_context_type *> address_map;
		}code;

		typedef bool(*analyze_callback_type)(dbg::api_handle, dbg::util_handle, void *, unsigned long long, void *context);

		//
		//
		//
		static x86_disasm_context_type *create_segment();

		virtual void get_uuid(uuid_type *iid) = 0;

		virtual bool check(unsigned long long ptr, unsigned long long base, unsigned long long end);
		virtual bool calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end);

		virtual bool disasm(unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, x86_disasm_context_type *context) = 0;
		virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size) = 0;

		virtual bool find_reference_value(dbg::api *api_handle, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map) = 0;

		virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_safe) = 0;
		virtual unsigned long browse(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe) = 0;
		virtual unsigned long run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set) = 0;
	};

#define CreateDefaultApiHandle()			dbg::linker::api::create(dbg::CORE_IID::IID_DEFAULT_API_CORE)	
#define CreateDisasmUtil1Handle()			dbg::linker::util::create(dbg::CORE_IID::IID_DISASSEMBLER_UTIL1)
#define CreateDisasmUtil2Handle()			dbg::linker::util::create(dbg::CORE_IID::IID_DISASSEMBLER_UTIL2)
#define CreateDefaultUtilHandle()			dbg::linker::util::create(dbg::CORE_IID::IID_DEFAULT_UTIL)
#define CreateEmulatorApiHandle()			dbg::linker::api::create(dbg::CORE_IID::IID_EMULATOR_API_CORE)
#define CreateEmulatorApiUtilHandle()		dbg::linker::util::create(dbg::CORE_IID::IID_EMULATOR_UTIL)
#define CreateRemoteApiHandle()				dbg::linker::api::create(dbg::CORE_IID::IID_REMOTEAPI_CORE)

	namespace linker
	{
		namespace api
		{
			__declspec(dllexport) api_handle create(unsigned long id);

			__declspec(dllexport) bool open(api_handle handle, char *path);
			__declspec(dllexport) bool open(api_handle handle, unsigned long pid);

			__declspec(dllexport) bool close(api_handle handle);
			__declspec(dllexport) bool get_thread_id_list(api_handle handle, std::list<unsigned long> &tid_list);

			__declspec(dllexport) bool select_thread(api_handle handle, unsigned long tid);
			__declspec(dllexport) unsigned long current_thread_id(api_handle handle);

			__declspec(dllexport) void * virtual_alloc(api_handle handle, unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type);
			__declspec(dllexport) int virtual_free(api_handle handle, void *ptr, unsigned long size, unsigned long free_type);

			__declspec(dllexport) bool query_virtual_memory(api_handle handle, unsigned long long virtual_address, void *out_memory_info);
			__declspec(dllexport) unsigned long __stdcall read_virtual_memory(api_handle handle, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size);
			__declspec(dllexport) unsigned long __stdcall write_virtual_memory(api_handle handle, unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size);

			__declspec(dllexport) bool __stdcall get_thread_context(api_handle handle, cpu_context_type *context);
			__declspec(dllexport) bool __stdcall set_thread_context(api_handle handle, cpu_context_type *context);

			__declspec(dllexport) bool get_symbol_name(api_handle handle, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp);
			__declspec(dllexport) bool stack_trace(api_handle handle, unsigned long tid, dbg::api::stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count);

			__declspec(dllexport) bool write_dump(api_handle handle, char *path);

			__declspec(dllexport) bool suspend(api_handle api_handle, unsigned long tid);
			__declspec(dllexport) bool resume(api_handle api_handle, unsigned long tid);
		}

		namespace util
		{
			__declspec(dllexport) util_handle create(unsigned long id);

			__declspec(dllexport) bool disasm(util_handle handle, unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, dbg::util::x86_disasm_context_type *context);
			__declspec(dllexport) unsigned long long mnemonic_str(util_handle handle, void *api_handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size);

			__declspec(dllexport) bool find_reference_value(util_handle handle, api_handle api, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map);

			__declspec(dllexport) unsigned long trace(util_handle handle, void *api_handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_safe);
			__declspec(dllexport) unsigned long browse(util_handle handle, void *api_handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe);
			__declspec(dllexport) unsigned long run(util_handle handle, void *api_handle, dbg::util::analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set);
		}
	}
}

//
// disasm id
//
namespace DISASSEMBLER_1_UTIL_ID
{
	typedef enum x86_reg {
		X86_REG_INVALID = 0,
		X86_REG_AH, X86_REG_AL, X86_REG_AX, X86_REG_BH, X86_REG_BL,
		X86_REG_BP, X86_REG_BPL, X86_REG_BX, X86_REG_CH, X86_REG_CL,
		X86_REG_CS, X86_REG_CX, X86_REG_DH, X86_REG_DI, X86_REG_DIL,
		X86_REG_DL, X86_REG_DS, X86_REG_DX, X86_REG_EAX, X86_REG_EBP,
		X86_REG_EBX, X86_REG_ECX, X86_REG_EDI, X86_REG_EDX, X86_REG_EFLAGS,
		X86_REG_EIP, X86_REG_EIZ, X86_REG_ES, X86_REG_ESI, X86_REG_ESP,
		X86_REG_FPSW, X86_REG_FS, X86_REG_GS, X86_REG_IP, X86_REG_RAX,
		X86_REG_RBP, X86_REG_RBX, X86_REG_RCX, X86_REG_RDI, X86_REG_RDX,
		X86_REG_RIP, X86_REG_RIZ, X86_REG_RSI, X86_REG_RSP, X86_REG_SI,
		X86_REG_SIL, X86_REG_SP, X86_REG_SPL, X86_REG_SS, X86_REG_CR0,
		X86_REG_CR1, X86_REG_CR2, X86_REG_CR3, X86_REG_CR4, X86_REG_CR5,
		X86_REG_CR6, X86_REG_CR7, X86_REG_CR8, X86_REG_CR9, X86_REG_CR10,
		X86_REG_CR11, X86_REG_CR12, X86_REG_CR13, X86_REG_CR14, X86_REG_CR15,
		X86_REG_DR0, X86_REG_DR1, X86_REG_DR2, X86_REG_DR3, X86_REG_DR4,
		X86_REG_DR5, X86_REG_DR6, X86_REG_DR7, X86_REG_FP0, X86_REG_FP1,
		X86_REG_FP2, X86_REG_FP3, X86_REG_FP4, X86_REG_FP5, X86_REG_FP6, X86_REG_FP7,
		X86_REG_K0, X86_REG_K1, X86_REG_K2, X86_REG_K3, X86_REG_K4,
		X86_REG_K5, X86_REG_K6, X86_REG_K7, X86_REG_MM0, X86_REG_MM1,
		X86_REG_MM2, X86_REG_MM3, X86_REG_MM4, X86_REG_MM5, X86_REG_MM6,
		X86_REG_MM7, X86_REG_R8, X86_REG_R9, X86_REG_R10, X86_REG_R11,
		X86_REG_R12, X86_REG_R13, X86_REG_R14, X86_REG_R15,
		X86_REG_ST0, X86_REG_ST1, X86_REG_ST2, X86_REG_ST3,
		X86_REG_ST4, X86_REG_ST5, X86_REG_ST6, X86_REG_ST7,
		X86_REG_XMM0, X86_REG_XMM1, X86_REG_XMM2, X86_REG_XMM3, X86_REG_XMM4,
		X86_REG_XMM5, X86_REG_XMM6, X86_REG_XMM7, X86_REG_XMM8, X86_REG_XMM9,
		X86_REG_XMM10, X86_REG_XMM11, X86_REG_XMM12, X86_REG_XMM13, X86_REG_XMM14,
		X86_REG_XMM15, X86_REG_XMM16, X86_REG_XMM17, X86_REG_XMM18, X86_REG_XMM19,
		X86_REG_XMM20, X86_REG_XMM21, X86_REG_XMM22, X86_REG_XMM23, X86_REG_XMM24,
		X86_REG_XMM25, X86_REG_XMM26, X86_REG_XMM27, X86_REG_XMM28, X86_REG_XMM29,
		X86_REG_XMM30, X86_REG_XMM31, X86_REG_YMM0, X86_REG_YMM1, X86_REG_YMM2,
		X86_REG_YMM3, X86_REG_YMM4, X86_REG_YMM5, X86_REG_YMM6, X86_REG_YMM7,
		X86_REG_YMM8, X86_REG_YMM9, X86_REG_YMM10, X86_REG_YMM11, X86_REG_YMM12,
		X86_REG_YMM13, X86_REG_YMM14, X86_REG_YMM15, X86_REG_YMM16, X86_REG_YMM17,
		X86_REG_YMM18, X86_REG_YMM19, X86_REG_YMM20, X86_REG_YMM21, X86_REG_YMM22,
		X86_REG_YMM23, X86_REG_YMM24, X86_REG_YMM25, X86_REG_YMM26, X86_REG_YMM27,
		X86_REG_YMM28, X86_REG_YMM29, X86_REG_YMM30, X86_REG_YMM31, X86_REG_ZMM0,
		X86_REG_ZMM1, X86_REG_ZMM2, X86_REG_ZMM3, X86_REG_ZMM4, X86_REG_ZMM5,
		X86_REG_ZMM6, X86_REG_ZMM7, X86_REG_ZMM8, X86_REG_ZMM9, X86_REG_ZMM10,
		X86_REG_ZMM11, X86_REG_ZMM12, X86_REG_ZMM13, X86_REG_ZMM14, X86_REG_ZMM15,
		X86_REG_ZMM16, X86_REG_ZMM17, X86_REG_ZMM18, X86_REG_ZMM19, X86_REG_ZMM20,
		X86_REG_ZMM21, X86_REG_ZMM22, X86_REG_ZMM23, X86_REG_ZMM24, X86_REG_ZMM25,
		X86_REG_ZMM26, X86_REG_ZMM27, X86_REG_ZMM28, X86_REG_ZMM29, X86_REG_ZMM30,
		X86_REG_ZMM31, X86_REG_R8B, X86_REG_R9B, X86_REG_R10B, X86_REG_R11B,
		X86_REG_R12B, X86_REG_R13B, X86_REG_R14B, X86_REG_R15B, X86_REG_R8D,
		X86_REG_R9D, X86_REG_R10D, X86_REG_R11D, X86_REG_R12D, X86_REG_R13D,
		X86_REG_R14D, X86_REG_R15D, X86_REG_R8W, X86_REG_R9W, X86_REG_R10W,
		X86_REG_R11W, X86_REG_R12W, X86_REG_R13W, X86_REG_R14W, X86_REG_R15W,

		X86_REG_ENDING		// <-- mark the end of the list of registers
	} x86_reg;

	//> Operand type for instruction's operands
	typedef enum x86_op_type {
		X86_OP_INVALID = 0, // = CS_OP_INVALID (Uninitialized).
		X86_OP_REG, // = CS_OP_REG (Register operand).
		X86_OP_IMM, // = CS_OP_IMM (Immediate operand).
		X86_OP_MEM, // = CS_OP_MEM (Memory operand).
		X86_OP_FP,  //  = CS_OP_FP  (Floating-Point operand).
	} x86_op_type;

	//> AVX broadcast type
	typedef enum x86_avx_bcast {
		X86_AVX_BCAST_INVALID = 0,	// Uninitialized.
		X86_AVX_BCAST_2,	// AVX512 broadcast type {1to2}
		X86_AVX_BCAST_4,	// AVX512 broadcast type {1to4}
		X86_AVX_BCAST_8,	// AVX512 broadcast type {1to8}
		X86_AVX_BCAST_16,	// AVX512 broadcast type {1to16}
	} x86_avx_bcast;

	//> SSE Code Condition type
	typedef enum x86_sse_cc {
		X86_SSE_CC_INVALID = 0,	// Uninitialized.
		X86_SSE_CC_EQ,
		X86_SSE_CC_LT,
		X86_SSE_CC_LE,
		X86_SSE_CC_UNORD,
		X86_SSE_CC_NEQ,
		X86_SSE_CC_NLT,
		X86_SSE_CC_NLE,
		X86_SSE_CC_ORD,
		X86_SSE_CC_EQ_UQ,
		X86_SSE_CC_NGE,
		X86_SSE_CC_NGT,
		X86_SSE_CC_FALSE,
		X86_SSE_CC_NEQ_OQ,
		X86_SSE_CC_GE,
		X86_SSE_CC_GT,
		X86_SSE_CC_TRUE,
	} x86_sse_cc;

	//> AVX Code Condition type
	typedef enum x86_avx_cc {
		X86_AVX_CC_INVALID = 0,	// Uninitialized.
		X86_AVX_CC_EQ,
		X86_AVX_CC_LT,
		X86_AVX_CC_LE,
		X86_AVX_CC_UNORD,
		X86_AVX_CC_NEQ,
		X86_AVX_CC_NLT,
		X86_AVX_CC_NLE,
		X86_AVX_CC_ORD,
		X86_AVX_CC_EQ_UQ,
		X86_AVX_CC_NGE,
		X86_AVX_CC_NGT,
		X86_AVX_CC_FALSE,
		X86_AVX_CC_NEQ_OQ,
		X86_AVX_CC_GE,
		X86_AVX_CC_GT,
		X86_AVX_CC_TRUE,
		X86_AVX_CC_EQ_OS,
		X86_AVX_CC_LT_OQ,
		X86_AVX_CC_LE_OQ,
		X86_AVX_CC_UNORD_S,
		X86_AVX_CC_NEQ_US,
		X86_AVX_CC_NLT_UQ,
		X86_AVX_CC_NLE_UQ,
		X86_AVX_CC_ORD_S,
		X86_AVX_CC_EQ_US,
		X86_AVX_CC_NGE_UQ,
		X86_AVX_CC_NGT_UQ,
		X86_AVX_CC_FALSE_OS,
		X86_AVX_CC_NEQ_OS,
		X86_AVX_CC_GE_OQ,
		X86_AVX_CC_GT_OQ,
		X86_AVX_CC_TRUE_US,
	} x86_avx_cc;

	//> AVX static rounding mode type
	typedef enum x86_avx_rm {
		X86_AVX_RM_INVALID = 0,	// Uninitialized.
		X86_AVX_RM_RN,	// Round to nearest
		X86_AVX_RM_RD,	// Round down
		X86_AVX_RM_RU,	// Round up
		X86_AVX_RM_RZ,	// Round toward zero
	} x86_avx_rm;

	//> Instruction prefixes - to be used in cs_x86.prefix[]
	typedef enum x86_prefix {
		X86_PREFIX_LOCK = 0xf0,	// lock (cs_x86.prefix[0]
		X86_PREFIX_REP = 0xf3,	// rep (cs_x86.prefix[0]
		X86_PREFIX_REPNE = 0xf2,	// repne (cs_x86.prefix[0]

		X86_PREFIX_CS = 0x2e,	// segment override CS (cs_x86.prefix[1]
		X86_PREFIX_SS = 0x36,	// segment override SS (cs_x86.prefix[1]
		X86_PREFIX_DS = 0x3e,	// segment override DS (cs_x86.prefix[1]
		X86_PREFIX_ES = 0x26,	// segment override ES (cs_x86.prefix[1]
		X86_PREFIX_FS = 0x64,	// segment override FS (cs_x86.prefix[1]
		X86_PREFIX_GS = 0x65,	// segment override GS (cs_x86.prefix[1]

		X86_PREFIX_OPSIZE = 0x66,	// operand-size override (cs_x86.prefix[2]
		X86_PREFIX_ADDRSIZE = 0x67,	// address-size override (cs_x86.prefix[3]
	} x86_prefix;

	//> X86 instructions
	typedef enum x86_insn {
		X86_INS_INVALID = 0,

		X86_INS_AAA,
		X86_INS_AAD,
		X86_INS_AAM,
		X86_INS_AAS,
		X86_INS_FABS,
		X86_INS_ADC,
		X86_INS_ADCX,
		X86_INS_ADD,
		X86_INS_ADDPD,
		X86_INS_ADDPS,
		X86_INS_ADDSD,
		X86_INS_ADDSS,
		X86_INS_ADDSUBPD,
		X86_INS_ADDSUBPS,
		X86_INS_FADD,
		X86_INS_FIADD,
		X86_INS_FADDP,
		X86_INS_ADOX,
		X86_INS_AESDECLAST,
		X86_INS_AESDEC,
		X86_INS_AESENCLAST,
		X86_INS_AESENC,
		X86_INS_AESIMC,
		X86_INS_AESKEYGENASSIST,
		X86_INS_AND,
		X86_INS_ANDN,
		X86_INS_ANDNPD,
		X86_INS_ANDNPS,
		X86_INS_ANDPD,
		X86_INS_ANDPS,
		X86_INS_ARPL,
		X86_INS_BEXTR,
		X86_INS_BLCFILL,
		X86_INS_BLCI,
		X86_INS_BLCIC,
		X86_INS_BLCMSK,
		X86_INS_BLCS,
		X86_INS_BLENDPD,
		X86_INS_BLENDPS,
		X86_INS_BLENDVPD,
		X86_INS_BLENDVPS,
		X86_INS_BLSFILL,
		X86_INS_BLSI,
		X86_INS_BLSIC,
		X86_INS_BLSMSK,
		X86_INS_BLSR,
		X86_INS_BOUND,
		X86_INS_BSF,
		X86_INS_BSR,
		X86_INS_BSWAP,
		X86_INS_BT,
		X86_INS_BTC,
		X86_INS_BTR,
		X86_INS_BTS,
		X86_INS_BZHI,
		X86_INS_CALL,
		X86_INS_CBW,
		X86_INS_CDQ,
		X86_INS_CDQE,
		X86_INS_FCHS,
		X86_INS_CLAC,
		X86_INS_CLC,
		X86_INS_CLD,
		X86_INS_CLFLUSH,
		X86_INS_CLGI,
		X86_INS_CLI,
		X86_INS_CLTS,
		X86_INS_CMC,
		X86_INS_CMOVA,
		X86_INS_CMOVAE,
		X86_INS_CMOVB,
		X86_INS_CMOVBE,
		X86_INS_FCMOVBE,
		X86_INS_FCMOVB,
		X86_INS_CMOVE,
		X86_INS_FCMOVE,
		X86_INS_CMOVG,
		X86_INS_CMOVGE,
		X86_INS_CMOVL,
		X86_INS_CMOVLE,
		X86_INS_FCMOVNBE,
		X86_INS_FCMOVNB,
		X86_INS_CMOVNE,
		X86_INS_FCMOVNE,
		X86_INS_CMOVNO,
		X86_INS_CMOVNP,
		X86_INS_FCMOVNU,
		X86_INS_CMOVNS,
		X86_INS_CMOVO,
		X86_INS_CMOVP,
		X86_INS_FCMOVU,
		X86_INS_CMOVS,
		X86_INS_CMP,
		X86_INS_CMPPD,
		X86_INS_CMPPS,
		X86_INS_CMPSB,
		X86_INS_CMPSD,
		X86_INS_CMPSQ,
		X86_INS_CMPSS,
		X86_INS_CMPSW,
		X86_INS_CMPXCHG16B,
		X86_INS_CMPXCHG,
		X86_INS_CMPXCHG8B,
		X86_INS_COMISD,
		X86_INS_COMISS,
		X86_INS_FCOMP,
		X86_INS_FCOMPI,
		X86_INS_FCOMI,
		X86_INS_FCOM,
		X86_INS_FCOS,
		X86_INS_CPUID,
		X86_INS_CQO,
		X86_INS_CRC32,
		X86_INS_CVTDQ2PD,
		X86_INS_CVTDQ2PS,
		X86_INS_CVTPD2DQ,
		X86_INS_CVTPD2PS,
		X86_INS_CVTPS2DQ,
		X86_INS_CVTPS2PD,
		X86_INS_CVTSD2SI,
		X86_INS_CVTSD2SS,
		X86_INS_CVTSI2SD,
		X86_INS_CVTSI2SS,
		X86_INS_CVTSS2SD,
		X86_INS_CVTSS2SI,
		X86_INS_CVTTPD2DQ,
		X86_INS_CVTTPS2DQ,
		X86_INS_CVTTSD2SI,
		X86_INS_CVTTSS2SI,
		X86_INS_CWD,
		X86_INS_CWDE,
		X86_INS_DAA,
		X86_INS_DAS,
		X86_INS_DATA16,
		X86_INS_DEC,
		X86_INS_DIV,
		X86_INS_DIVPD,
		X86_INS_DIVPS,
		X86_INS_FDIVR,
		X86_INS_FIDIVR,
		X86_INS_FDIVRP,
		X86_INS_DIVSD,
		X86_INS_DIVSS,
		X86_INS_FDIV,
		X86_INS_FIDIV,
		X86_INS_FDIVP,
		X86_INS_DPPD,
		X86_INS_DPPS,
		X86_INS_RET,
		X86_INS_ENCLS,
		X86_INS_ENCLU,
		X86_INS_ENTER,
		X86_INS_EXTRACTPS,
		X86_INS_EXTRQ,
		X86_INS_F2XM1,
		X86_INS_LCALL,
		X86_INS_LJMP,
		X86_INS_FBLD,
		X86_INS_FBSTP,
		X86_INS_FCOMPP,
		X86_INS_FDECSTP,
		X86_INS_FEMMS,
		X86_INS_FFREE,
		X86_INS_FICOM,
		X86_INS_FICOMP,
		X86_INS_FINCSTP,
		X86_INS_FLDCW,
		X86_INS_FLDENV,
		X86_INS_FLDL2E,
		X86_INS_FLDL2T,
		X86_INS_FLDLG2,
		X86_INS_FLDLN2,
		X86_INS_FLDPI,
		X86_INS_FNCLEX,
		X86_INS_FNINIT,
		X86_INS_FNOP,
		X86_INS_FNSTCW,
		X86_INS_FNSTSW,
		X86_INS_FPATAN,
		X86_INS_FPREM,
		X86_INS_FPREM1,
		X86_INS_FPTAN,
		X86_INS_FRNDINT,
		X86_INS_FRSTOR,
		X86_INS_FNSAVE,
		X86_INS_FSCALE,
		X86_INS_FSETPM,
		X86_INS_FSINCOS,
		X86_INS_FNSTENV,
		X86_INS_FXAM,
		X86_INS_FXRSTOR,
		X86_INS_FXRSTOR64,
		X86_INS_FXSAVE,
		X86_INS_FXSAVE64,
		X86_INS_FXTRACT,
		X86_INS_FYL2X,
		X86_INS_FYL2XP1,
		X86_INS_MOVAPD,
		X86_INS_MOVAPS,
		X86_INS_ORPD,
		X86_INS_ORPS,
		X86_INS_VMOVAPD,
		X86_INS_VMOVAPS,
		X86_INS_XORPD,
		X86_INS_XORPS,
		X86_INS_GETSEC,
		X86_INS_HADDPD,
		X86_INS_HADDPS,
		X86_INS_HLT,
		X86_INS_HSUBPD,
		X86_INS_HSUBPS,
		X86_INS_IDIV,
		X86_INS_FILD,
		X86_INS_IMUL,
		X86_INS_IN,
		X86_INS_INC,
		X86_INS_INSB,
		X86_INS_INSERTPS,
		X86_INS_INSERTQ,
		X86_INS_INSD,
		X86_INS_INSW,
		X86_INS_INT,
		X86_INS_INT1,
		X86_INS_INT3,
		X86_INS_INTO,
		X86_INS_INVD,
		X86_INS_INVEPT,
		X86_INS_INVLPG,
		X86_INS_INVLPGA,
		X86_INS_INVPCID,
		X86_INS_INVVPID,
		X86_INS_IRET,
		X86_INS_IRETD,
		X86_INS_IRETQ,
		X86_INS_FISTTP,
		X86_INS_FIST,
		X86_INS_FISTP,
		X86_INS_UCOMISD,
		X86_INS_UCOMISS,
		X86_INS_VCMP,
		X86_INS_VCOMISD,
		X86_INS_VCOMISS,
		X86_INS_VCVTSD2SS,
		X86_INS_VCVTSI2SD,
		X86_INS_VCVTSI2SS,
		X86_INS_VCVTSS2SD,
		X86_INS_VCVTTSD2SI,
		X86_INS_VCVTTSD2USI,
		X86_INS_VCVTTSS2SI,
		X86_INS_VCVTTSS2USI,
		X86_INS_VCVTUSI2SD,
		X86_INS_VCVTUSI2SS,
		X86_INS_VUCOMISD,
		X86_INS_VUCOMISS,
		X86_INS_JAE,
		X86_INS_JA,
		X86_INS_JBE,
		X86_INS_JB,
		X86_INS_JCXZ,
		X86_INS_JECXZ,
		X86_INS_JE,
		X86_INS_JGE,
		X86_INS_JG,
		X86_INS_JLE,
		X86_INS_JL,
		X86_INS_JMP,
		X86_INS_JNE,
		X86_INS_JNO,
		X86_INS_JNP,
		X86_INS_JNS,
		X86_INS_JO,
		X86_INS_JP,
		X86_INS_JRCXZ,
		X86_INS_JS,
		X86_INS_KANDB,
		X86_INS_KANDD,
		X86_INS_KANDNB,
		X86_INS_KANDND,
		X86_INS_KANDNQ,
		X86_INS_KANDNW,
		X86_INS_KANDQ,
		X86_INS_KANDW,
		X86_INS_KMOVB,
		X86_INS_KMOVD,
		X86_INS_KMOVQ,
		X86_INS_KMOVW,
		X86_INS_KNOTB,
		X86_INS_KNOTD,
		X86_INS_KNOTQ,
		X86_INS_KNOTW,
		X86_INS_KORB,
		X86_INS_KORD,
		X86_INS_KORQ,
		X86_INS_KORTESTW,
		X86_INS_KORW,
		X86_INS_KSHIFTLW,
		X86_INS_KSHIFTRW,
		X86_INS_KUNPCKBW,
		X86_INS_KXNORB,
		X86_INS_KXNORD,
		X86_INS_KXNORQ,
		X86_INS_KXNORW,
		X86_INS_KXORB,
		X86_INS_KXORD,
		X86_INS_KXORQ,
		X86_INS_KXORW,
		X86_INS_LAHF,
		X86_INS_LAR,
		X86_INS_LDDQU,
		X86_INS_LDMXCSR,
		X86_INS_LDS,
		X86_INS_FLDZ,
		X86_INS_FLD1,
		X86_INS_FLD,
		X86_INS_LEA,
		X86_INS_LEAVE,
		X86_INS_LES,
		X86_INS_LFENCE,
		X86_INS_LFS,
		X86_INS_LGDT,
		X86_INS_LGS,
		X86_INS_LIDT,
		X86_INS_LLDT,
		X86_INS_LMSW,
		X86_INS_OR,
		X86_INS_SUB,
		X86_INS_XOR,
		X86_INS_LODSB,
		X86_INS_LODSD,
		X86_INS_LODSQ,
		X86_INS_LODSW,
		X86_INS_LOOP,
		X86_INS_LOOPE,
		X86_INS_LOOPNE,
		X86_INS_RETF,
		X86_INS_RETFQ,
		X86_INS_LSL,
		X86_INS_LSS,
		X86_INS_LTR,
		X86_INS_XADD,
		X86_INS_LZCNT,
		X86_INS_MASKMOVDQU,
		X86_INS_MAXPD,
		X86_INS_MAXPS,
		X86_INS_MAXSD,
		X86_INS_MAXSS,
		X86_INS_MFENCE,
		X86_INS_MINPD,
		X86_INS_MINPS,
		X86_INS_MINSD,
		X86_INS_MINSS,
		X86_INS_CVTPD2PI,
		X86_INS_CVTPI2PD,
		X86_INS_CVTPI2PS,
		X86_INS_CVTPS2PI,
		X86_INS_CVTTPD2PI,
		X86_INS_CVTTPS2PI,
		X86_INS_EMMS,
		X86_INS_MASKMOVQ,
		X86_INS_MOVD,
		X86_INS_MOVDQ2Q,
		X86_INS_MOVNTQ,
		X86_INS_MOVQ2DQ,
		X86_INS_MOVQ,
		X86_INS_PABSB,
		X86_INS_PABSD,
		X86_INS_PABSW,
		X86_INS_PACKSSDW,
		X86_INS_PACKSSWB,
		X86_INS_PACKUSWB,
		X86_INS_PADDB,
		X86_INS_PADDD,
		X86_INS_PADDQ,
		X86_INS_PADDSB,
		X86_INS_PADDSW,
		X86_INS_PADDUSB,
		X86_INS_PADDUSW,
		X86_INS_PADDW,
		X86_INS_PALIGNR,
		X86_INS_PANDN,
		X86_INS_PAND,
		X86_INS_PAVGB,
		X86_INS_PAVGW,
		X86_INS_PCMPEQB,
		X86_INS_PCMPEQD,
		X86_INS_PCMPEQW,
		X86_INS_PCMPGTB,
		X86_INS_PCMPGTD,
		X86_INS_PCMPGTW,
		X86_INS_PEXTRW,
		X86_INS_PHADDSW,
		X86_INS_PHADDW,
		X86_INS_PHADDD,
		X86_INS_PHSUBD,
		X86_INS_PHSUBSW,
		X86_INS_PHSUBW,
		X86_INS_PINSRW,
		X86_INS_PMADDUBSW,
		X86_INS_PMADDWD,
		X86_INS_PMAXSW,
		X86_INS_PMAXUB,
		X86_INS_PMINSW,
		X86_INS_PMINUB,
		X86_INS_PMOVMSKB,
		X86_INS_PMULHRSW,
		X86_INS_PMULHUW,
		X86_INS_PMULHW,
		X86_INS_PMULLW,
		X86_INS_PMULUDQ,
		X86_INS_POR,
		X86_INS_PSADBW,
		X86_INS_PSHUFB,
		X86_INS_PSHUFW,
		X86_INS_PSIGNB,
		X86_INS_PSIGND,
		X86_INS_PSIGNW,
		X86_INS_PSLLD,
		X86_INS_PSLLQ,
		X86_INS_PSLLW,
		X86_INS_PSRAD,
		X86_INS_PSRAW,
		X86_INS_PSRLD,
		X86_INS_PSRLQ,
		X86_INS_PSRLW,
		X86_INS_PSUBB,
		X86_INS_PSUBD,
		X86_INS_PSUBQ,
		X86_INS_PSUBSB,
		X86_INS_PSUBSW,
		X86_INS_PSUBUSB,
		X86_INS_PSUBUSW,
		X86_INS_PSUBW,
		X86_INS_PUNPCKHBW,
		X86_INS_PUNPCKHDQ,
		X86_INS_PUNPCKHWD,
		X86_INS_PUNPCKLBW,
		X86_INS_PUNPCKLDQ,
		X86_INS_PUNPCKLWD,
		X86_INS_PXOR,
		X86_INS_MONITOR,
		X86_INS_MONTMUL,
		X86_INS_MOV,
		X86_INS_MOVABS,
		X86_INS_MOVBE,
		X86_INS_MOVDDUP,
		X86_INS_MOVDQA,
		X86_INS_MOVDQU,
		X86_INS_MOVHLPS,
		X86_INS_MOVHPD,
		X86_INS_MOVHPS,
		X86_INS_MOVLHPS,
		X86_INS_MOVLPD,
		X86_INS_MOVLPS,
		X86_INS_MOVMSKPD,
		X86_INS_MOVMSKPS,
		X86_INS_MOVNTDQA,
		X86_INS_MOVNTDQ,
		X86_INS_MOVNTI,
		X86_INS_MOVNTPD,
		X86_INS_MOVNTPS,
		X86_INS_MOVNTSD,
		X86_INS_MOVNTSS,
		X86_INS_MOVSB,
		X86_INS_MOVSD,
		X86_INS_MOVSHDUP,
		X86_INS_MOVSLDUP,
		X86_INS_MOVSQ,
		X86_INS_MOVSS,
		X86_INS_MOVSW,
		X86_INS_MOVSX,
		X86_INS_MOVSXD,
		X86_INS_MOVUPD,
		X86_INS_MOVUPS,
		X86_INS_MOVZX,
		X86_INS_MPSADBW,
		X86_INS_MUL,
		X86_INS_MULPD,
		X86_INS_MULPS,
		X86_INS_MULSD,
		X86_INS_MULSS,
		X86_INS_MULX,
		X86_INS_FMUL,
		X86_INS_FIMUL,
		X86_INS_FMULP,
		X86_INS_MWAIT,
		X86_INS_NEG,
		X86_INS_NOP,
		X86_INS_NOT,
		X86_INS_OUT,
		X86_INS_OUTSB,
		X86_INS_OUTSD,
		X86_INS_OUTSW,
		X86_INS_PACKUSDW,
		X86_INS_PAUSE,
		X86_INS_PAVGUSB,
		X86_INS_PBLENDVB,
		X86_INS_PBLENDW,
		X86_INS_PCLMULQDQ,
		X86_INS_PCMPEQQ,
		X86_INS_PCMPESTRI,
		X86_INS_PCMPESTRM,
		X86_INS_PCMPGTQ,
		X86_INS_PCMPISTRI,
		X86_INS_PCMPISTRM,
		X86_INS_PDEP,
		X86_INS_PEXT,
		X86_INS_PEXTRB,
		X86_INS_PEXTRD,
		X86_INS_PEXTRQ,
		X86_INS_PF2ID,
		X86_INS_PF2IW,
		X86_INS_PFACC,
		X86_INS_PFADD,
		X86_INS_PFCMPEQ,
		X86_INS_PFCMPGE,
		X86_INS_PFCMPGT,
		X86_INS_PFMAX,
		X86_INS_PFMIN,
		X86_INS_PFMUL,
		X86_INS_PFNACC,
		X86_INS_PFPNACC,
		X86_INS_PFRCPIT1,
		X86_INS_PFRCPIT2,
		X86_INS_PFRCP,
		X86_INS_PFRSQIT1,
		X86_INS_PFRSQRT,
		X86_INS_PFSUBR,
		X86_INS_PFSUB,
		X86_INS_PHMINPOSUW,
		X86_INS_PI2FD,
		X86_INS_PI2FW,
		X86_INS_PINSRB,
		X86_INS_PINSRD,
		X86_INS_PINSRQ,
		X86_INS_PMAXSB,
		X86_INS_PMAXSD,
		X86_INS_PMAXUD,
		X86_INS_PMAXUW,
		X86_INS_PMINSB,
		X86_INS_PMINSD,
		X86_INS_PMINUD,
		X86_INS_PMINUW,
		X86_INS_PMOVSXBD,
		X86_INS_PMOVSXBQ,
		X86_INS_PMOVSXBW,
		X86_INS_PMOVSXDQ,
		X86_INS_PMOVSXWD,
		X86_INS_PMOVSXWQ,
		X86_INS_PMOVZXBD,
		X86_INS_PMOVZXBQ,
		X86_INS_PMOVZXBW,
		X86_INS_PMOVZXDQ,
		X86_INS_PMOVZXWD,
		X86_INS_PMOVZXWQ,
		X86_INS_PMULDQ,
		X86_INS_PMULHRW,
		X86_INS_PMULLD,
		X86_INS_POP,
		X86_INS_POPAW,
		X86_INS_POPAL,
		X86_INS_POPCNT,
		X86_INS_POPF,
		X86_INS_POPFD,
		X86_INS_POPFQ,
		X86_INS_PREFETCH,
		X86_INS_PREFETCHNTA,
		X86_INS_PREFETCHT0,
		X86_INS_PREFETCHT1,
		X86_INS_PREFETCHT2,
		X86_INS_PREFETCHW,
		X86_INS_PSHUFD,
		X86_INS_PSHUFHW,
		X86_INS_PSHUFLW,
		X86_INS_PSLLDQ,
		X86_INS_PSRLDQ,
		X86_INS_PSWAPD,
		X86_INS_PTEST,
		X86_INS_PUNPCKHQDQ,
		X86_INS_PUNPCKLQDQ,
		X86_INS_PUSH,
		X86_INS_PUSHAW,
		X86_INS_PUSHAL,
		X86_INS_PUSHF,
		X86_INS_PUSHFD,
		X86_INS_PUSHFQ,
		X86_INS_RCL,
		X86_INS_RCPPS,
		X86_INS_RCPSS,
		X86_INS_RCR,
		X86_INS_RDFSBASE,
		X86_INS_RDGSBASE,
		X86_INS_RDMSR,
		X86_INS_RDPMC,
		X86_INS_RDRAND,
		X86_INS_RDSEED,
		X86_INS_RDTSC,
		X86_INS_RDTSCP,
		X86_INS_ROL,
		X86_INS_ROR,
		X86_INS_RORX,
		X86_INS_ROUNDPD,
		X86_INS_ROUNDPS,
		X86_INS_ROUNDSD,
		X86_INS_ROUNDSS,
		X86_INS_RSM,
		X86_INS_RSQRTPS,
		X86_INS_RSQRTSS,
		X86_INS_SAHF,
		X86_INS_SAL,
		X86_INS_SALC,
		X86_INS_SAR,
		X86_INS_SARX,
		X86_INS_SBB,
		X86_INS_SCASB,
		X86_INS_SCASD,
		X86_INS_SCASQ,
		X86_INS_SCASW,
		X86_INS_SETAE,
		X86_INS_SETA,
		X86_INS_SETBE,
		X86_INS_SETB,
		X86_INS_SETE,
		X86_INS_SETGE,
		X86_INS_SETG,
		X86_INS_SETLE,
		X86_INS_SETL,
		X86_INS_SETNE,
		X86_INS_SETNO,
		X86_INS_SETNP,
		X86_INS_SETNS,
		X86_INS_SETO,
		X86_INS_SETP,
		X86_INS_SETS,
		X86_INS_SFENCE,
		X86_INS_SGDT,
		X86_INS_SHA1MSG1,
		X86_INS_SHA1MSG2,
		X86_INS_SHA1NEXTE,
		X86_INS_SHA1RNDS4,
		X86_INS_SHA256MSG1,
		X86_INS_SHA256MSG2,
		X86_INS_SHA256RNDS2,
		X86_INS_SHL,
		X86_INS_SHLD,
		X86_INS_SHLX,
		X86_INS_SHR,
		X86_INS_SHRD,
		X86_INS_SHRX,
		X86_INS_SHUFPD,
		X86_INS_SHUFPS,
		X86_INS_SIDT,
		X86_INS_FSIN,
		X86_INS_SKINIT,
		X86_INS_SLDT,
		X86_INS_SMSW,
		X86_INS_SQRTPD,
		X86_INS_SQRTPS,
		X86_INS_SQRTSD,
		X86_INS_SQRTSS,
		X86_INS_FSQRT,
		X86_INS_STAC,
		X86_INS_STC,
		X86_INS_STD,
		X86_INS_STGI,
		X86_INS_STI,
		X86_INS_STMXCSR,
		X86_INS_STOSB,
		X86_INS_STOSD,
		X86_INS_STOSQ,
		X86_INS_STOSW,
		X86_INS_STR,
		X86_INS_FST,
		X86_INS_FSTP,
		X86_INS_FSTPNCE,
		X86_INS_SUBPD,
		X86_INS_SUBPS,
		X86_INS_FSUBR,
		X86_INS_FISUBR,
		X86_INS_FSUBRP,
		X86_INS_SUBSD,
		X86_INS_SUBSS,
		X86_INS_FSUB,
		X86_INS_FISUB,
		X86_INS_FSUBP,
		X86_INS_SWAPGS,
		X86_INS_SYSCALL,
		X86_INS_SYSENTER,
		X86_INS_SYSEXIT,
		X86_INS_SYSRET,
		X86_INS_T1MSKC,
		X86_INS_TEST,
		X86_INS_UD2,
		X86_INS_FTST,
		X86_INS_TZCNT,
		X86_INS_TZMSK,
		X86_INS_FUCOMPI,
		X86_INS_FUCOMI,
		X86_INS_FUCOMPP,
		X86_INS_FUCOMP,
		X86_INS_FUCOM,
		X86_INS_UD2B,
		X86_INS_UNPCKHPD,
		X86_INS_UNPCKHPS,
		X86_INS_UNPCKLPD,
		X86_INS_UNPCKLPS,
		X86_INS_VADDPD,
		X86_INS_VADDPS,
		X86_INS_VADDSD,
		X86_INS_VADDSS,
		X86_INS_VADDSUBPD,
		X86_INS_VADDSUBPS,
		X86_INS_VAESDECLAST,
		X86_INS_VAESDEC,
		X86_INS_VAESENCLAST,
		X86_INS_VAESENC,
		X86_INS_VAESIMC,
		X86_INS_VAESKEYGENASSIST,
		X86_INS_VALIGND,
		X86_INS_VALIGNQ,
		X86_INS_VANDNPD,
		X86_INS_VANDNPS,
		X86_INS_VANDPD,
		X86_INS_VANDPS,
		X86_INS_VBLENDMPD,
		X86_INS_VBLENDMPS,
		X86_INS_VBLENDPD,
		X86_INS_VBLENDPS,
		X86_INS_VBLENDVPD,
		X86_INS_VBLENDVPS,
		X86_INS_VBROADCASTF128,
		X86_INS_VBROADCASTI128,
		X86_INS_VBROADCASTI32X4,
		X86_INS_VBROADCASTI64X4,
		X86_INS_VBROADCASTSD,
		X86_INS_VBROADCASTSS,
		X86_INS_VCMPPD,
		X86_INS_VCMPPS,
		X86_INS_VCMPSD,
		X86_INS_VCMPSS,
		X86_INS_VCVTDQ2PD,
		X86_INS_VCVTDQ2PS,
		X86_INS_VCVTPD2DQX,
		X86_INS_VCVTPD2DQ,
		X86_INS_VCVTPD2PSX,
		X86_INS_VCVTPD2PS,
		X86_INS_VCVTPD2UDQ,
		X86_INS_VCVTPH2PS,
		X86_INS_VCVTPS2DQ,
		X86_INS_VCVTPS2PD,
		X86_INS_VCVTPS2PH,
		X86_INS_VCVTPS2UDQ,
		X86_INS_VCVTSD2SI,
		X86_INS_VCVTSD2USI,
		X86_INS_VCVTSS2SI,
		X86_INS_VCVTSS2USI,
		X86_INS_VCVTTPD2DQX,
		X86_INS_VCVTTPD2DQ,
		X86_INS_VCVTTPD2UDQ,
		X86_INS_VCVTTPS2DQ,
		X86_INS_VCVTTPS2UDQ,
		X86_INS_VCVTUDQ2PD,
		X86_INS_VCVTUDQ2PS,
		X86_INS_VDIVPD,
		X86_INS_VDIVPS,
		X86_INS_VDIVSD,
		X86_INS_VDIVSS,
		X86_INS_VDPPD,
		X86_INS_VDPPS,
		X86_INS_VERR,
		X86_INS_VERW,
		X86_INS_VEXTRACTF128,
		X86_INS_VEXTRACTF32X4,
		X86_INS_VEXTRACTF64X4,
		X86_INS_VEXTRACTI128,
		X86_INS_VEXTRACTI32X4,
		X86_INS_VEXTRACTI64X4,
		X86_INS_VEXTRACTPS,
		X86_INS_VFMADD132PD,
		X86_INS_VFMADD132PS,
		X86_INS_VFMADD213PD,
		X86_INS_VFMADD213PS,
		X86_INS_VFMADDPD,
		X86_INS_VFMADD231PD,
		X86_INS_VFMADDPS,
		X86_INS_VFMADD231PS,
		X86_INS_VFMADDSD,
		X86_INS_VFMADD213SD,
		X86_INS_VFMADD132SD,
		X86_INS_VFMADD231SD,
		X86_INS_VFMADDSS,
		X86_INS_VFMADD213SS,
		X86_INS_VFMADD132SS,
		X86_INS_VFMADD231SS,
		X86_INS_VFMADDSUB132PD,
		X86_INS_VFMADDSUB132PS,
		X86_INS_VFMADDSUB213PD,
		X86_INS_VFMADDSUB213PS,
		X86_INS_VFMADDSUBPD,
		X86_INS_VFMADDSUB231PD,
		X86_INS_VFMADDSUBPS,
		X86_INS_VFMADDSUB231PS,
		X86_INS_VFMSUB132PD,
		X86_INS_VFMSUB132PS,
		X86_INS_VFMSUB213PD,
		X86_INS_VFMSUB213PS,
		X86_INS_VFMSUBADD132PD,
		X86_INS_VFMSUBADD132PS,
		X86_INS_VFMSUBADD213PD,
		X86_INS_VFMSUBADD213PS,
		X86_INS_VFMSUBADDPD,
		X86_INS_VFMSUBADD231PD,
		X86_INS_VFMSUBADDPS,
		X86_INS_VFMSUBADD231PS,
		X86_INS_VFMSUBPD,
		X86_INS_VFMSUB231PD,
		X86_INS_VFMSUBPS,
		X86_INS_VFMSUB231PS,
		X86_INS_VFMSUBSD,
		X86_INS_VFMSUB213SD,
		X86_INS_VFMSUB132SD,
		X86_INS_VFMSUB231SD,
		X86_INS_VFMSUBSS,
		X86_INS_VFMSUB213SS,
		X86_INS_VFMSUB132SS,
		X86_INS_VFMSUB231SS,
		X86_INS_VFNMADD132PD,
		X86_INS_VFNMADD132PS,
		X86_INS_VFNMADD213PD,
		X86_INS_VFNMADD213PS,
		X86_INS_VFNMADDPD,
		X86_INS_VFNMADD231PD,
		X86_INS_VFNMADDPS,
		X86_INS_VFNMADD231PS,
		X86_INS_VFNMADDSD,
		X86_INS_VFNMADD213SD,
		X86_INS_VFNMADD132SD,
		X86_INS_VFNMADD231SD,
		X86_INS_VFNMADDSS,
		X86_INS_VFNMADD213SS,
		X86_INS_VFNMADD132SS,
		X86_INS_VFNMADD231SS,
		X86_INS_VFNMSUB132PD,
		X86_INS_VFNMSUB132PS,
		X86_INS_VFNMSUB213PD,
		X86_INS_VFNMSUB213PS,
		X86_INS_VFNMSUBPD,
		X86_INS_VFNMSUB231PD,
		X86_INS_VFNMSUBPS,
		X86_INS_VFNMSUB231PS,
		X86_INS_VFNMSUBSD,
		X86_INS_VFNMSUB213SD,
		X86_INS_VFNMSUB132SD,
		X86_INS_VFNMSUB231SD,
		X86_INS_VFNMSUBSS,
		X86_INS_VFNMSUB213SS,
		X86_INS_VFNMSUB132SS,
		X86_INS_VFNMSUB231SS,
		X86_INS_VFRCZPD,
		X86_INS_VFRCZPS,
		X86_INS_VFRCZSD,
		X86_INS_VFRCZSS,
		X86_INS_VORPD,
		X86_INS_VORPS,
		X86_INS_VXORPD,
		X86_INS_VXORPS,
		X86_INS_VGATHERDPD,
		X86_INS_VGATHERDPS,
		X86_INS_VGATHERPF0DPD,
		X86_INS_VGATHERPF0DPS,
		X86_INS_VGATHERPF0QPD,
		X86_INS_VGATHERPF0QPS,
		X86_INS_VGATHERPF1DPD,
		X86_INS_VGATHERPF1DPS,
		X86_INS_VGATHERPF1QPD,
		X86_INS_VGATHERPF1QPS,
		X86_INS_VGATHERQPD,
		X86_INS_VGATHERQPS,
		X86_INS_VHADDPD,
		X86_INS_VHADDPS,
		X86_INS_VHSUBPD,
		X86_INS_VHSUBPS,
		X86_INS_VINSERTF128,
		X86_INS_VINSERTF32X4,
		X86_INS_VINSERTF64X4,
		X86_INS_VINSERTI128,
		X86_INS_VINSERTI32X4,
		X86_INS_VINSERTI64X4,
		X86_INS_VINSERTPS,
		X86_INS_VLDDQU,
		X86_INS_VLDMXCSR,
		X86_INS_VMASKMOVDQU,
		X86_INS_VMASKMOVPD,
		X86_INS_VMASKMOVPS,
		X86_INS_VMAXPD,
		X86_INS_VMAXPS,
		X86_INS_VMAXSD,
		X86_INS_VMAXSS,
		X86_INS_VMCALL,
		X86_INS_VMCLEAR,
		X86_INS_VMFUNC,
		X86_INS_VMINPD,
		X86_INS_VMINPS,
		X86_INS_VMINSD,
		X86_INS_VMINSS,
		X86_INS_VMLAUNCH,
		X86_INS_VMLOAD,
		X86_INS_VMMCALL,
		X86_INS_VMOVQ,
		X86_INS_VMOVDDUP,
		X86_INS_VMOVD,
		X86_INS_VMOVDQA32,
		X86_INS_VMOVDQA64,
		X86_INS_VMOVDQA,
		X86_INS_VMOVDQU16,
		X86_INS_VMOVDQU32,
		X86_INS_VMOVDQU64,
		X86_INS_VMOVDQU8,
		X86_INS_VMOVDQU,
		X86_INS_VMOVHLPS,
		X86_INS_VMOVHPD,
		X86_INS_VMOVHPS,
		X86_INS_VMOVLHPS,
		X86_INS_VMOVLPD,
		X86_INS_VMOVLPS,
		X86_INS_VMOVMSKPD,
		X86_INS_VMOVMSKPS,
		X86_INS_VMOVNTDQA,
		X86_INS_VMOVNTDQ,
		X86_INS_VMOVNTPD,
		X86_INS_VMOVNTPS,
		X86_INS_VMOVSD,
		X86_INS_VMOVSHDUP,
		X86_INS_VMOVSLDUP,
		X86_INS_VMOVSS,
		X86_INS_VMOVUPD,
		X86_INS_VMOVUPS,
		X86_INS_VMPSADBW,
		X86_INS_VMPTRLD,
		X86_INS_VMPTRST,
		X86_INS_VMREAD,
		X86_INS_VMRESUME,
		X86_INS_VMRUN,
		X86_INS_VMSAVE,
		X86_INS_VMULPD,
		X86_INS_VMULPS,
		X86_INS_VMULSD,
		X86_INS_VMULSS,
		X86_INS_VMWRITE,
		X86_INS_VMXOFF,
		X86_INS_VMXON,
		X86_INS_VPABSB,
		X86_INS_VPABSD,
		X86_INS_VPABSQ,
		X86_INS_VPABSW,
		X86_INS_VPACKSSDW,
		X86_INS_VPACKSSWB,
		X86_INS_VPACKUSDW,
		X86_INS_VPACKUSWB,
		X86_INS_VPADDB,
		X86_INS_VPADDD,
		X86_INS_VPADDQ,
		X86_INS_VPADDSB,
		X86_INS_VPADDSW,
		X86_INS_VPADDUSB,
		X86_INS_VPADDUSW,
		X86_INS_VPADDW,
		X86_INS_VPALIGNR,
		X86_INS_VPANDD,
		X86_INS_VPANDND,
		X86_INS_VPANDNQ,
		X86_INS_VPANDN,
		X86_INS_VPANDQ,
		X86_INS_VPAND,
		X86_INS_VPAVGB,
		X86_INS_VPAVGW,
		X86_INS_VPBLENDD,
		X86_INS_VPBLENDMD,
		X86_INS_VPBLENDMQ,
		X86_INS_VPBLENDVB,
		X86_INS_VPBLENDW,
		X86_INS_VPBROADCASTB,
		X86_INS_VPBROADCASTD,
		X86_INS_VPBROADCASTMB2Q,
		X86_INS_VPBROADCASTMW2D,
		X86_INS_VPBROADCASTQ,
		X86_INS_VPBROADCASTW,
		X86_INS_VPCLMULQDQ,
		X86_INS_VPCMOV,
		X86_INS_VPCMP,
		X86_INS_VPCMPD,
		X86_INS_VPCMPEQB,
		X86_INS_VPCMPEQD,
		X86_INS_VPCMPEQQ,
		X86_INS_VPCMPEQW,
		X86_INS_VPCMPESTRI,
		X86_INS_VPCMPESTRM,
		X86_INS_VPCMPGTB,
		X86_INS_VPCMPGTD,
		X86_INS_VPCMPGTQ,
		X86_INS_VPCMPGTW,
		X86_INS_VPCMPISTRI,
		X86_INS_VPCMPISTRM,
		X86_INS_VPCMPQ,
		X86_INS_VPCMPUD,
		X86_INS_VPCMPUQ,
		X86_INS_VPCOMB,
		X86_INS_VPCOMD,
		X86_INS_VPCOMQ,
		X86_INS_VPCOMUB,
		X86_INS_VPCOMUD,
		X86_INS_VPCOMUQ,
		X86_INS_VPCOMUW,
		X86_INS_VPCOMW,
		X86_INS_VPCONFLICTD,
		X86_INS_VPCONFLICTQ,
		X86_INS_VPERM2F128,
		X86_INS_VPERM2I128,
		X86_INS_VPERMD,
		X86_INS_VPERMI2D,
		X86_INS_VPERMI2PD,
		X86_INS_VPERMI2PS,
		X86_INS_VPERMI2Q,
		X86_INS_VPERMIL2PD,
		X86_INS_VPERMIL2PS,
		X86_INS_VPERMILPD,
		X86_INS_VPERMILPS,
		X86_INS_VPERMPD,
		X86_INS_VPERMPS,
		X86_INS_VPERMQ,
		X86_INS_VPERMT2D,
		X86_INS_VPERMT2PD,
		X86_INS_VPERMT2PS,
		X86_INS_VPERMT2Q,
		X86_INS_VPEXTRB,
		X86_INS_VPEXTRD,
		X86_INS_VPEXTRQ,
		X86_INS_VPEXTRW,
		X86_INS_VPGATHERDD,
		X86_INS_VPGATHERDQ,
		X86_INS_VPGATHERQD,
		X86_INS_VPGATHERQQ,
		X86_INS_VPHADDBD,
		X86_INS_VPHADDBQ,
		X86_INS_VPHADDBW,
		X86_INS_VPHADDDQ,
		X86_INS_VPHADDD,
		X86_INS_VPHADDSW,
		X86_INS_VPHADDUBD,
		X86_INS_VPHADDUBQ,
		X86_INS_VPHADDUBW,
		X86_INS_VPHADDUDQ,
		X86_INS_VPHADDUWD,
		X86_INS_VPHADDUWQ,
		X86_INS_VPHADDWD,
		X86_INS_VPHADDWQ,
		X86_INS_VPHADDW,
		X86_INS_VPHMINPOSUW,
		X86_INS_VPHSUBBW,
		X86_INS_VPHSUBDQ,
		X86_INS_VPHSUBD,
		X86_INS_VPHSUBSW,
		X86_INS_VPHSUBWD,
		X86_INS_VPHSUBW,
		X86_INS_VPINSRB,
		X86_INS_VPINSRD,
		X86_INS_VPINSRQ,
		X86_INS_VPINSRW,
		X86_INS_VPLZCNTD,
		X86_INS_VPLZCNTQ,
		X86_INS_VPMACSDD,
		X86_INS_VPMACSDQH,
		X86_INS_VPMACSDQL,
		X86_INS_VPMACSSDD,
		X86_INS_VPMACSSDQH,
		X86_INS_VPMACSSDQL,
		X86_INS_VPMACSSWD,
		X86_INS_VPMACSSWW,
		X86_INS_VPMACSWD,
		X86_INS_VPMACSWW,
		X86_INS_VPMADCSSWD,
		X86_INS_VPMADCSWD,
		X86_INS_VPMADDUBSW,
		X86_INS_VPMADDWD,
		X86_INS_VPMASKMOVD,
		X86_INS_VPMASKMOVQ,
		X86_INS_VPMAXSB,
		X86_INS_VPMAXSD,
		X86_INS_VPMAXSQ,
		X86_INS_VPMAXSW,
		X86_INS_VPMAXUB,
		X86_INS_VPMAXUD,
		X86_INS_VPMAXUQ,
		X86_INS_VPMAXUW,
		X86_INS_VPMINSB,
		X86_INS_VPMINSD,
		X86_INS_VPMINSQ,
		X86_INS_VPMINSW,
		X86_INS_VPMINUB,
		X86_INS_VPMINUD,
		X86_INS_VPMINUQ,
		X86_INS_VPMINUW,
		X86_INS_VPMOVDB,
		X86_INS_VPMOVDW,
		X86_INS_VPMOVMSKB,
		X86_INS_VPMOVQB,
		X86_INS_VPMOVQD,
		X86_INS_VPMOVQW,
		X86_INS_VPMOVSDB,
		X86_INS_VPMOVSDW,
		X86_INS_VPMOVSQB,
		X86_INS_VPMOVSQD,
		X86_INS_VPMOVSQW,
		X86_INS_VPMOVSXBD,
		X86_INS_VPMOVSXBQ,
		X86_INS_VPMOVSXBW,
		X86_INS_VPMOVSXDQ,
		X86_INS_VPMOVSXWD,
		X86_INS_VPMOVSXWQ,
		X86_INS_VPMOVUSDB,
		X86_INS_VPMOVUSDW,
		X86_INS_VPMOVUSQB,
		X86_INS_VPMOVUSQD,
		X86_INS_VPMOVUSQW,
		X86_INS_VPMOVZXBD,
		X86_INS_VPMOVZXBQ,
		X86_INS_VPMOVZXBW,
		X86_INS_VPMOVZXDQ,
		X86_INS_VPMOVZXWD,
		X86_INS_VPMOVZXWQ,
		X86_INS_VPMULDQ,
		X86_INS_VPMULHRSW,
		X86_INS_VPMULHUW,
		X86_INS_VPMULHW,
		X86_INS_VPMULLD,
		X86_INS_VPMULLW,
		X86_INS_VPMULUDQ,
		X86_INS_VPORD,
		X86_INS_VPORQ,
		X86_INS_VPOR,
		X86_INS_VPPERM,
		X86_INS_VPROTB,
		X86_INS_VPROTD,
		X86_INS_VPROTQ,
		X86_INS_VPROTW,
		X86_INS_VPSADBW,
		X86_INS_VPSCATTERDD,
		X86_INS_VPSCATTERDQ,
		X86_INS_VPSCATTERQD,
		X86_INS_VPSCATTERQQ,
		X86_INS_VPSHAB,
		X86_INS_VPSHAD,
		X86_INS_VPSHAQ,
		X86_INS_VPSHAW,
		X86_INS_VPSHLB,
		X86_INS_VPSHLD,
		X86_INS_VPSHLQ,
		X86_INS_VPSHLW,
		X86_INS_VPSHUFB,
		X86_INS_VPSHUFD,
		X86_INS_VPSHUFHW,
		X86_INS_VPSHUFLW,
		X86_INS_VPSIGNB,
		X86_INS_VPSIGND,
		X86_INS_VPSIGNW,
		X86_INS_VPSLLDQ,
		X86_INS_VPSLLD,
		X86_INS_VPSLLQ,
		X86_INS_VPSLLVD,
		X86_INS_VPSLLVQ,
		X86_INS_VPSLLW,
		X86_INS_VPSRAD,
		X86_INS_VPSRAQ,
		X86_INS_VPSRAVD,
		X86_INS_VPSRAVQ,
		X86_INS_VPSRAW,
		X86_INS_VPSRLDQ,
		X86_INS_VPSRLD,
		X86_INS_VPSRLQ,
		X86_INS_VPSRLVD,
		X86_INS_VPSRLVQ,
		X86_INS_VPSRLW,
		X86_INS_VPSUBB,
		X86_INS_VPSUBD,
		X86_INS_VPSUBQ,
		X86_INS_VPSUBSB,
		X86_INS_VPSUBSW,
		X86_INS_VPSUBUSB,
		X86_INS_VPSUBUSW,
		X86_INS_VPSUBW,
		X86_INS_VPTESTMD,
		X86_INS_VPTESTMQ,
		X86_INS_VPTESTNMD,
		X86_INS_VPTESTNMQ,
		X86_INS_VPTEST,
		X86_INS_VPUNPCKHBW,
		X86_INS_VPUNPCKHDQ,
		X86_INS_VPUNPCKHQDQ,
		X86_INS_VPUNPCKHWD,
		X86_INS_VPUNPCKLBW,
		X86_INS_VPUNPCKLDQ,
		X86_INS_VPUNPCKLQDQ,
		X86_INS_VPUNPCKLWD,
		X86_INS_VPXORD,
		X86_INS_VPXORQ,
		X86_INS_VPXOR,
		X86_INS_VRCP14PD,
		X86_INS_VRCP14PS,
		X86_INS_VRCP14SD,
		X86_INS_VRCP14SS,
		X86_INS_VRCP28PD,
		X86_INS_VRCP28PS,
		X86_INS_VRCP28SD,
		X86_INS_VRCP28SS,
		X86_INS_VRCPPS,
		X86_INS_VRCPSS,
		X86_INS_VRNDSCALEPD,
		X86_INS_VRNDSCALEPS,
		X86_INS_VRNDSCALESD,
		X86_INS_VRNDSCALESS,
		X86_INS_VROUNDPD,
		X86_INS_VROUNDPS,
		X86_INS_VROUNDSD,
		X86_INS_VROUNDSS,
		X86_INS_VRSQRT14PD,
		X86_INS_VRSQRT14PS,
		X86_INS_VRSQRT14SD,
		X86_INS_VRSQRT14SS,
		X86_INS_VRSQRT28PD,
		X86_INS_VRSQRT28PS,
		X86_INS_VRSQRT28SD,
		X86_INS_VRSQRT28SS,
		X86_INS_VRSQRTPS,
		X86_INS_VRSQRTSS,
		X86_INS_VSCATTERDPD,
		X86_INS_VSCATTERDPS,
		X86_INS_VSCATTERPF0DPD,
		X86_INS_VSCATTERPF0DPS,
		X86_INS_VSCATTERPF0QPD,
		X86_INS_VSCATTERPF0QPS,
		X86_INS_VSCATTERPF1DPD,
		X86_INS_VSCATTERPF1DPS,
		X86_INS_VSCATTERPF1QPD,
		X86_INS_VSCATTERPF1QPS,
		X86_INS_VSCATTERQPD,
		X86_INS_VSCATTERQPS,
		X86_INS_VSHUFPD,
		X86_INS_VSHUFPS,
		X86_INS_VSQRTPD,
		X86_INS_VSQRTPS,
		X86_INS_VSQRTSD,
		X86_INS_VSQRTSS,
		X86_INS_VSTMXCSR,
		X86_INS_VSUBPD,
		X86_INS_VSUBPS,
		X86_INS_VSUBSD,
		X86_INS_VSUBSS,
		X86_INS_VTESTPD,
		X86_INS_VTESTPS,
		X86_INS_VUNPCKHPD,
		X86_INS_VUNPCKHPS,
		X86_INS_VUNPCKLPD,
		X86_INS_VUNPCKLPS,
		X86_INS_VZEROALL,
		X86_INS_VZEROUPPER,
		X86_INS_WAIT,
		X86_INS_WBINVD,
		X86_INS_WRFSBASE,
		X86_INS_WRGSBASE,
		X86_INS_WRMSR,
		X86_INS_XABORT,
		X86_INS_XACQUIRE,
		X86_INS_XBEGIN,
		X86_INS_XCHG,
		X86_INS_FXCH,
		X86_INS_XCRYPTCBC,
		X86_INS_XCRYPTCFB,
		X86_INS_XCRYPTCTR,
		X86_INS_XCRYPTECB,
		X86_INS_XCRYPTOFB,
		X86_INS_XEND,
		X86_INS_XGETBV,
		X86_INS_XLATB,
		X86_INS_XRELEASE,
		X86_INS_XRSTOR,
		X86_INS_XRSTOR64,
		X86_INS_XSAVE,
		X86_INS_XSAVE64,
		X86_INS_XSAVEOPT,
		X86_INS_XSAVEOPT64,
		X86_INS_XSETBV,
		X86_INS_XSHA1,
		X86_INS_XSHA256,
		X86_INS_XSTORE,
		X86_INS_XTEST,

		X86_INS_ENDING,	// mark the end of the list of insn
	} x86_insn;

	//> Group of X86 instructions
	typedef enum  x86_insn_group {
		X86_GRP_INVALID = 0,
		X86_GRP_JUMP,
		X86_GRP_CALL,
		X86_GRP_RET,
		X86_GRP_INT,
		X86_GRP_IRET,

		X86_GRP_VM = 128,
		X86_GRP_3DNOW,
		X86_GRP_AES,
		X86_GRP_ADX,
		X86_GRP_AVX,
		X86_GRP_AVX2,
		X86_GRP_AVX512,
		X86_GRP_BMI,
		X86_GRP_BMI2,
		X86_GRP_CMOV,
		X86_GRP_F16C,
		X86_GRP_FMA,
		X86_GRP_FMA4,
		X86_GRP_FSGSBASE,
		X86_GRP_HLE,
		X86_GRP_MMX,
		X86_GRP_MODE32,
		X86_GRP_MODE64,
		X86_GRP_RTM,
		X86_GRP_SHA,
		X86_GRP_SSE1,
		X86_GRP_SSE2,
		X86_GRP_SSE3,
		X86_GRP_SSE41,
		X86_GRP_SSE42,
		X86_GRP_SSE4A,
		X86_GRP_SSSE3,
		X86_GRP_PCLMUL,
		X86_GRP_XOP,
		X86_GRP_CDI,
		X86_GRP_ERI,
		X86_GRP_TBM,
		X86_GRP_16BITMODE,
		X86_GRP_NOT64BITMODE,
		X86_GRP_SGX,
		X86_GRP_DQI,
		X86_GRP_BWI,
		X86_GRP_PFI,
		X86_GRP_VLX,
		X86_GRP_SMAP,
		X86_GRP_NOVLX,

		X86_GRP_ENDING
	} x86_insn_group;
}

namespace DISASSEMBLER_2_UTIL_ID
{
	typedef enum { O_NONE, O_REG, O_IMM, O_IMM1, O_IMM2, O_DISP, O_SMEM, O_MEM, O_PC, O_PTR } _OperandType;

	typedef enum {
		I_UNDEFINED = 0, I_AAA = 66, I_AAD = 389, I_AAM = 384, I_AAS = 76, I_ADC = 31, I_ADD = 11, I_ADDPD = 3132,
		I_ADDPS = 3125, I_ADDSD = 3146, I_ADDSS = 3139, I_ADDSUBPD = 6416, I_ADDSUBPS = 6426,
		I_AESDEC = 9231, I_AESDECLAST = 9248, I_AESENC = 9189, I_AESENCLAST = 9206,
		I_AESIMC = 9172, I_AESKEYGENASSIST = 9817, I_AND = 41, I_ANDNPD = 3043, I_ANDNPS = 3035,
		I_ANDPD = 3012, I_ANDPS = 3005, I_ARPL = 111, I_BLENDPD = 9394, I_BLENDPS = 9375,
		I_BLENDVPD = 7641, I_BLENDVPS = 7631, I_BOUND = 104, I_BSF = 4368, I_BSR = 4380,
		I_BSWAP = 960, I_BT = 872, I_BTC = 934, I_BTR = 912, I_BTS = 887, I_CALL = 456,
		I_CALL_FAR = 260, I_CBW = 228, I_CDQ = 250, I_CDQE = 239, I_CLC = 492, I_CLD = 512,
		I_CLFLUSH = 4351, I_CLGI = 1855, I_CLI = 502, I_CLTS = 541, I_CMC = 487, I_CMOVA = 694,
		I_CMOVAE = 663, I_CMOVB = 656, I_CMOVBE = 686, I_CMOVG = 754, I_CMOVGE = 738,
		I_CMOVL = 731, I_CMOVLE = 746, I_CMOVNO = 648, I_CMOVNP = 723, I_CMOVNS = 708,
		I_CMOVNZ = 678, I_CMOVO = 641, I_CMOVP = 716, I_CMOVS = 701, I_CMOVZ = 671,
		I_CMP = 71, I_CMPEQPD = 4471, I_CMPEQPS = 4392, I_CMPEQSD = 4629, I_CMPEQSS = 4550,
		I_CMPLEPD = 4489, I_CMPLEPS = 4410, I_CMPLESD = 4647, I_CMPLESS = 4568, I_CMPLTPD = 4480,
		I_CMPLTPS = 4401, I_CMPLTSD = 4638, I_CMPLTSS = 4559, I_CMPNEQPD = 4510, I_CMPNEQPS = 4431,
		I_CMPNEQSD = 4668, I_CMPNEQSS = 4589, I_CMPNLEPD = 4530, I_CMPNLEPS = 4451,
		I_CMPNLESD = 4688, I_CMPNLESS = 4609, I_CMPNLTPD = 4520, I_CMPNLTPS = 4441,
		I_CMPNLTSD = 4678, I_CMPNLTSS = 4599, I_CMPORDPD = 4540, I_CMPORDPS = 4461,
		I_CMPORDSD = 4698, I_CMPORDSS = 4619, I_CMPS = 301, I_CMPUNORDPD = 4498, I_CMPUNORDPS = 4419,
		I_CMPUNORDSD = 4656, I_CMPUNORDSS = 4577, I_CMPXCHG = 898, I_CMPXCHG16B = 6395,
		I_CMPXCHG8B = 6384, I_COMISD = 2801, I_COMISS = 2793, I_CPUID = 865, I_CQO = 255,
		I_CRC32 = 9280, I_CVTDQ2PD = 6809, I_CVTDQ2PS = 3329, I_CVTPD2DQ = 6819, I_CVTPD2PI = 2703,
		I_CVTPD2PS = 3255, I_CVTPH2PS = 4183, I_CVTPI2PD = 2517, I_CVTPI2PS = 2507,
		I_CVTPS2DQ = 3339, I_CVTPS2PD = 3245, I_CVTPS2PH = 4193, I_CVTPS2PI = 2693,
		I_CVTSD2SI = 2723, I_CVTSD2SS = 3275, I_CVTSI2SD = 2537, I_CVTSI2SS = 2527,
		I_CVTSS2SD = 3265, I_CVTSS2SI = 2713, I_CVTTPD2DQ = 6798, I_CVTTPD2PI = 2636,
		I_CVTTPS2DQ = 3349, I_CVTTPS2PI = 2625, I_CVTTSD2SI = 2658, I_CVTTSS2SI = 2647,
		I_CWD = 245, I_CWDE = 233, I_DAA = 46, I_DAS = 56, I_DEC = 86, I_DIV = 1646,
		I_DIVPD = 3521, I_DIVPS = 3514, I_DIVSD = 3535, I_DIVSS = 3528, I_DPPD = 9637,
		I_DPPS = 9624, I_EMMS = 4122, I_ENTER = 340, I_EXTRACTPS = 9502, I_EXTRQ = 4158,
		I_F2XM1 = 1192, I_FABS = 1123, I_FADD = 1023, I_FADDP = 1549, I_FBLD = 1601,
		I_FBSTP = 1607, I_FCHS = 1117, I_FCLEX = 7311, I_FCMOVB = 1376, I_FCMOVBE = 1392,
		I_FCMOVE = 1384, I_FCMOVNB = 1445, I_FCMOVNBE = 1463, I_FCMOVNE = 1454, I_FCMOVNU = 1473,
		I_FCMOVU = 1401, I_FCOM = 1035, I_FCOMI = 1512, I_FCOMIP = 1623, I_FCOMP = 1041,
		I_FCOMPP = 1563, I_FCOS = 1311, I_FDECSTP = 1238, I_FDIV = 1061, I_FDIVP = 1594,
		I_FDIVR = 1067, I_FDIVRP = 1586, I_FEDISI = 1488, I_FEMMS = 574, I_FENI = 1482,
		I_FFREE = 1527, I_FIADD = 1317, I_FICOM = 1331, I_FICOMP = 1338, I_FIDIV = 1361,
		I_FIDIVR = 1368, I_FILD = 1418, I_FIMUL = 1324, I_FINCSTP = 1247, I_FINIT = 7326,
		I_FIST = 1432, I_FISTP = 1438, I_FISTTP = 1424, I_FISUB = 1346, I_FISUBR = 1353,
		I_FLD = 1074, I_FLD1 = 1141, I_FLDCW = 1098, I_FLDENV = 1090, I_FLDL2E = 1155,
		I_FLDL2T = 1147, I_FLDLG2 = 1170, I_FLDLN2 = 1178, I_FLDPI = 1163, I_FLDZ = 1186,
		I_FMUL = 1029, I_FMULP = 1556, I_FNCLEX = 7303, I_FNINIT = 7318, I_FNOP = 1111,
		I_FNSAVE = 7333, I_FNSTCW = 7288, I_FNSTENV = 7271, I_FNSTSW = 7348, I_FPATAN = 1213,
		I_FPREM = 1256, I_FPREM1 = 1230, I_FPTAN = 1206, I_FRNDINT = 1288, I_FRSTOR = 1519,
		I_FSAVE = 7341, I_FSCALE = 1297, I_FSETPM = 1496, I_FSIN = 1305, I_FSINCOS = 1279,
		I_FSQRT = 1272, I_FST = 1079, I_FSTCW = 7296, I_FSTENV = 7280, I_FSTP = 1084,
		I_FSTSW = 7356, I_FSUB = 1048, I_FSUBP = 1579, I_FSUBR = 1054, I_FSUBRP = 1571,
		I_FTST = 1129, I_FUCOM = 1534, I_FUCOMI = 1504, I_FUCOMIP = 1614, I_FUCOMP = 1541,
		I_FUCOMPP = 1409, I_FXAM = 1135, I_FXCH = 1105, I_FXRSTOR = 9914, I_FXRSTOR64 = 9923,
		I_FXSAVE = 9886, I_FXSAVE64 = 9894, I_FXTRACT = 1221, I_FYL2X = 1199, I_FYL2XP1 = 1263,
		I_GETSEC = 633, I_HADDPD = 4203, I_HADDPS = 4211, I_HLT = 482, I_HSUBPD = 4237,
		I_HSUBPS = 4245, I_IDIV = 1651, I_IMUL = 117, I_IN = 447, I_INC = 81, I_INS = 123,
		I_INSERTPS = 9569, I_INSERTQ = 4165, I_INT = 367, I_INT_3 = 360, I_INT1 = 476,
		I_INTO = 372, I_INVD = 555, I_INVEPT = 8306, I_INVLPG = 1727, I_INVLPGA = 1869,
		I_INVPCID = 8323, I_INVVPID = 8314, I_IRET = 378, I_JA = 166, I_JAE = 147,
		I_JB = 143, I_JBE = 161, I_JCXZ = 427, I_JECXZ = 433, I_JG = 202, I_JGE = 192,
		I_JL = 188, I_JLE = 197, I_JMP = 462, I_JMP_FAR = 467, I_JNO = 138, I_JNP = 183,
		I_JNS = 174, I_JNZ = 156, I_JO = 134, I_JP = 179, I_JRCXZ = 440, I_JS = 170,
		I_JZ = 152, I_LAHF = 289, I_LAR = 522, I_LDDQU = 7016, I_LDMXCSR = 9944, I_LDS = 335,
		I_LEA = 223, I_LEAVE = 347, I_LES = 330, I_LFENCE = 4287, I_LFS = 917, I_LGDT = 1703,
		I_LGS = 922, I_LIDT = 1709, I_LLDT = 1668, I_LMSW = 1721, I_LODS = 313, I_LOOP = 421,
		I_LOOPNZ = 406, I_LOOPZ = 414, I_LSL = 527, I_LSS = 907, I_LTR = 1674, I_LZCNT = 4385,
		I_MASKMOVDQU = 7141, I_MASKMOVQ = 7131, I_MAXPD = 3581, I_MAXPS = 3574, I_MAXSD = 3595,
		I_MAXSS = 3588, I_MFENCE = 4313, I_MINPD = 3461, I_MINPS = 3454, I_MINSD = 3475,
		I_MINSS = 3468, I_MONITOR = 1771, I_MOV = 218, I_MOVAPD = 2481, I_MOVAPS = 2473,
		I_MOVBE = 9273, I_MOVD = 3942, I_MOVDDUP = 2208, I_MOVDQ2Q = 6544, I_MOVDQA = 3968,
		I_MOVDQU = 3976, I_MOVHLPS = 2173, I_MOVHPD = 2367, I_MOVHPS = 2359, I_MOVLHPS = 2350,
		I_MOVLPD = 2190, I_MOVLPS = 2182, I_MOVMSKPD = 2837, I_MOVMSKPS = 2827, I_MOVNTDQ = 6871,
		I_MOVNTDQA = 7917, I_MOVNTI = 952, I_MOVNTPD = 2578, I_MOVNTPS = 2569, I_MOVNTQ = 6863,
		I_MOVNTSD = 2596, I_MOVNTSS = 2587, I_MOVQ = 3948, I_MOVQ2DQ = 6535, I_MOVS = 295,
		I_MOVSD = 2132, I_MOVSHDUP = 2375, I_MOVSLDUP = 2198, I_MOVSS = 2125, I_MOVSX = 939,
		I_MOVSXD = 10027, I_MOVUPD = 2117, I_MOVUPS = 2109, I_MOVZX = 927, I_MPSADBW = 9650,
		I_MUL = 1641, I_MULPD = 3192, I_MULPS = 3185, I_MULSD = 3206, I_MULSS = 3199,
		I_MWAIT = 1780, I_NEG = 1636, I_NOP = 581, I_NOT = 1631, I_OR = 27, I_ORPD = 3075,
		I_ORPS = 3069, I_OUT = 451, I_OUTS = 128, I_PABSB = 7710, I_PABSD = 7740, I_PABSW = 7725,
		I_PACKSSDW = 3871, I_PACKSSWB = 3703, I_PACKUSDW = 7938, I_PACKUSWB = 3781,
		I_PADDB = 7226, I_PADDD = 7256, I_PADDQ = 6503, I_PADDSB = 6952, I_PADDSW = 6969,
		I_PADDUSB = 6642, I_PADDUSW = 6661, I_PADDW = 7241, I_PALIGNR = 9432, I_PAND = 6629,
		I_PANDN = 6687, I_PAUSE = 10035, I_PAVGB = 6702, I_PAVGUSB = 2100, I_PAVGW = 6747,
		I_PBLENDVB = 7621, I_PBLENDW = 9413, I_PCLMULQDQ = 9669, I_PCMPEQB = 4065,
		I_PCMPEQD = 4103, I_PCMPEQQ = 7898, I_PCMPEQW = 4084, I_PCMPESTRI = 9748,
		I_PCMPESTRM = 9725, I_PCMPGTB = 3724, I_PCMPGTD = 3762, I_PCMPGTQ = 8109,
		I_PCMPGTW = 3743, I_PCMPISTRI = 9794, I_PCMPISTRM = 9771, I_PEXTRB = 9451,
		I_PEXTRD = 9468, I_PEXTRQ = 9476, I_PEXTRW = 6333, I_PF2ID = 1936, I_PF2IW = 1929,
		I_PFACC = 2050, I_PFADD = 1999, I_PFCMPEQ = 2057, I_PFCMPGE = 1960, I_PFCMPGT = 2006,
		I_PFMAX = 2015, I_PFMIN = 1969, I_PFMUL = 2066, I_PFNACC = 1943, I_PFPNACC = 1951,
		I_PFRCP = 1976, I_PFRCPIT1 = 2022, I_PFRCPIT2 = 2073, I_PFRSQIT1 = 2032, I_PFRSQRT = 1983,
		I_PFSUB = 1992, I_PFSUBR = 2042, I_PHADDD = 7397, I_PHADDSW = 7414, I_PHADDW = 7380,
		I_PHMINPOSUW = 8281, I_PHSUBD = 7473, I_PHSUBSW = 7490, I_PHSUBW = 7456, I_PI2FD = 1922,
		I_PI2FW = 1915, I_PINSRB = 9552, I_PINSRD = 9590, I_PINSRQ = 9598, I_PINSRW = 6316,
		I_PMADDUBSW = 7433, I_PMADDWD = 7095, I_PMAXSB = 8196, I_PMAXSD = 8213, I_PMAXSW = 6986,
		I_PMAXUB = 6670, I_PMAXUD = 8247, I_PMAXUW = 8230, I_PMINSB = 8128, I_PMINSD = 8145,
		I_PMINSW = 6924, I_PMINUB = 6612, I_PMINUD = 8179, I_PMINUW = 8162, I_PMOVMSKB = 6553,
		I_PMOVSXBD = 7776, I_PMOVSXBQ = 7797, I_PMOVSXBW = 7755, I_PMOVSXDQ = 7860,
		I_PMOVSXWD = 7818, I_PMOVSXWQ = 7839, I_PMOVZXBD = 8004, I_PMOVZXBQ = 8025,
		I_PMOVZXBW = 7983, I_PMOVZXDQ = 8088, I_PMOVZXWD = 8046, I_PMOVZXWQ = 8067,
		I_PMULDQ = 7881, I_PMULHRSW = 7560, I_PMULHRW = 2083, I_PMULHUW = 6762, I_PMULHW = 6781,
		I_PMULLD = 8264, I_PMULLW = 6518, I_PMULUDQ = 7076, I_POP = 22, I_POPA = 98,
		I_POPCNT = 4360, I_POPF = 277, I_POR = 6941, I_PREFETCH = 1894, I_PREFETCHNTA = 2424,
		I_PREFETCHT0 = 2437, I_PREFETCHT1 = 2449, I_PREFETCHT2 = 2461, I_PREFETCHW = 1904,
		I_PSADBW = 7114, I_PSHUFB = 7363, I_PSHUFD = 4010, I_PSHUFHW = 4018, I_PSHUFLW = 4027,
		I_PSHUFW = 4002, I_PSIGNB = 7509, I_PSIGND = 7543, I_PSIGNW = 7526, I_PSLLD = 7046,
		I_PSLLDQ = 9869, I_PSLLQ = 7061, I_PSLLW = 7031, I_PSRAD = 6732, I_PSRAW = 6717,
		I_PSRLD = 6473, I_PSRLDQ = 9852, I_PSRLQ = 6488, I_PSRLW = 6458, I_PSUBB = 7166,
		I_PSUBD = 7196, I_PSUBQ = 7211, I_PSUBSB = 6890, I_PSUBSW = 6907, I_PSUBUSB = 6574,
		I_PSUBUSW = 6593, I_PSUBW = 7181, I_PSWAPD = 2092, I_PTEST = 7651, I_PUNPCKHBW = 3802,
		I_PUNPCKHDQ = 3848, I_PUNPCKHQDQ = 3917, I_PUNPCKHWD = 3825, I_PUNPCKLBW = 3634,
		I_PUNPCKLDQ = 3680, I_PUNPCKLQDQ = 3892, I_PUNPCKLWD = 3657, I_PUSH = 16,
		I_PUSHA = 91, I_PUSHF = 270, I_PXOR = 7003, I_RCL = 977, I_RCPPS = 2975, I_RCPSS = 2982,
		I_RCR = 982, I_RDFSBASE = 9904, I_RDGSBASE = 9934, I_RDMSR = 600, I_RDPMC = 607,
		I_RDRAND = 10048, I_RDTSC = 593, I_RDTSCP = 1886, I_RET = 325, I_RETF = 354,
		I_ROL = 967, I_ROR = 972, I_ROUNDPD = 9318, I_ROUNDPS = 9299, I_ROUNDSD = 9356,
		I_ROUNDSS = 9337, I_RSM = 882, I_RSQRTPS = 2937, I_RSQRTSS = 2946, I_SAHF = 283,
		I_SAL = 997, I_SALC = 394, I_SAR = 1002, I_SBB = 36, I_SCAS = 319, I_SETA = 807,
		I_SETAE = 780, I_SETB = 774, I_SETBE = 800, I_SETG = 859, I_SETGE = 845, I_SETL = 839,
		I_SETLE = 852, I_SETNO = 767, I_SETNP = 832, I_SETNS = 819, I_SETNZ = 793,
		I_SETO = 761, I_SETP = 826, I_SETS = 813, I_SETZ = 787, I_SFENCE = 4343, I_SGDT = 1691,
		I_SHL = 987, I_SHLD = 876, I_SHR = 992, I_SHRD = 892, I_SHUFPD = 6358, I_SHUFPS = 6350,
		I_SIDT = 1697, I_SKINIT = 1861, I_SLDT = 1657, I_SMSW = 1715, I_SQRTPD = 2877,
		I_SQRTPS = 2869, I_SQRTSD = 2893, I_SQRTSS = 2885, I_STC = 497, I_STD = 517,
		I_STGI = 1849, I_STI = 507, I_STMXCSR = 9973, I_STOS = 307, I_STR = 1663, I_SUB = 51,
		I_SUBPD = 3401, I_SUBPS = 3394, I_SUBSD = 3415, I_SUBSS = 3408, I_SWAPGS = 1878,
		I_SYSCALL = 532, I_SYSENTER = 614, I_SYSEXIT = 624, I_SYSRET = 547, I_TEST = 206,
		I_TZCNT = 4373, I_UCOMISD = 2764, I_UCOMISS = 2755, I_UD2 = 569, I_UNPCKHPD = 2318,
		I_UNPCKHPS = 2308, I_UNPCKLPD = 2276, I_UNPCKLPS = 2266, I_VADDPD = 3161,
		I_VADDPS = 3153, I_VADDSD = 3177, I_VADDSS = 3169, I_VADDSUBPD = 6436, I_VADDSUBPS = 6447,
		I_VAESDEC = 9239, I_VAESDECLAST = 9260, I_VAESENC = 9197, I_VAESENCLAST = 9218,
		I_VAESIMC = 9180, I_VAESKEYGENASSIST = 9834, I_VANDNPD = 3060, I_VANDNPS = 3051,
		I_VANDPD = 3027, I_VANDPS = 3019, I_VBLENDPD = 9403, I_VBLENDPS = 9384, I_VBLENDVPD = 9703,
		I_VBLENDVPS = 9692, I_VBROADCASTF128 = 7694, I_VBROADCASTSD = 7680, I_VBROADCASTSS = 7666,
		I_VCMPEQPD = 5110, I_VCMPEQPS = 4708, I_VCMPEQSD = 5914, I_VCMPEQSS = 5512,
		I_VCMPEQ_OSPD = 5291, I_VCMPEQ_OSPS = 4889, I_VCMPEQ_OSSD = 6095, I_VCMPEQ_OSSS = 5693,
		I_VCMPEQ_UQPD = 5197, I_VCMPEQ_UQPS = 4795, I_VCMPEQ_UQSD = 6001, I_VCMPEQ_UQSS = 5599,
		I_VCMPEQ_USPD = 5400, I_VCMPEQ_USPS = 4998, I_VCMPEQ_USSD = 6204, I_VCMPEQ_USSS = 5802,
		I_VCMPFALSEPD = 5232, I_VCMPFALSEPS = 4830, I_VCMPFALSESD = 6036, I_VCMPFALSESS = 5634,
		I_VCMPFALSE_OSPD = 5441, I_VCMPFALSE_OSPS = 5039, I_VCMPFALSE_OSSD = 6245,
		I_VCMPFALSE_OSSS = 5843, I_VCMPGEPD = 5259, I_VCMPGEPS = 4857, I_VCMPGESD = 6063,
		I_VCMPGESS = 5661, I_VCMPGE_OQPD = 5471, I_VCMPGE_OQPS = 5069, I_VCMPGE_OQSD = 6275,
		I_VCMPGE_OQSS = 5873, I_VCMPGTPD = 5269, I_VCMPGTPS = 4867, I_VCMPGTSD = 6073,
		I_VCMPGTSS = 5671, I_VCMPGT_OQPD = 5484, I_VCMPGT_OQPS = 5082, I_VCMPGT_OQSD = 6288,
		I_VCMPGT_OQSS = 5886, I_VCMPLEPD = 5130, I_VCMPLEPS = 4728, I_VCMPLESD = 5934,
		I_VCMPLESS = 5532, I_VCMPLE_OQPD = 5317, I_VCMPLE_OQPS = 4915, I_VCMPLE_OQSD = 6121,
		I_VCMPLE_OQSS = 5719, I_VCMPLTPD = 5120, I_VCMPLTPS = 4718, I_VCMPLTSD = 5924,
		I_VCMPLTSS = 5522, I_VCMPLT_OQPD = 5304, I_VCMPLT_OQPS = 4902, I_VCMPLT_OQSD = 6108,
		I_VCMPLT_OQSS = 5706, I_VCMPNEQPD = 5153, I_VCMPNEQPS = 4751, I_VCMPNEQSD = 5957,
		I_VCMPNEQSS = 5555, I_VCMPNEQ_OQPD = 5245, I_VCMPNEQ_OQPS = 4843, I_VCMPNEQ_OQSD = 6049,
		I_VCMPNEQ_OQSS = 5647, I_VCMPNEQ_OSPD = 5457, I_VCMPNEQ_OSPS = 5055, I_VCMPNEQ_OSSD = 6261,
		I_VCMPNEQ_OSSS = 5859, I_VCMPNEQ_USPD = 5345, I_VCMPNEQ_USPS = 4943, I_VCMPNEQ_USSD = 6149,
		I_VCMPNEQ_USSS = 5747, I_VCMPNGEPD = 5210, I_VCMPNGEPS = 4808, I_VCMPNGESD = 6014,
		I_VCMPNGESS = 5612, I_VCMPNGE_UQPD = 5413, I_VCMPNGE_UQPS = 5011, I_VCMPNGE_UQSD = 6217,
		I_VCMPNGE_UQSS = 5815, I_VCMPNGTPD = 5221, I_VCMPNGTPS = 4819, I_VCMPNGTSD = 6025,
		I_VCMPNGTSS = 5623, I_VCMPNGT_UQPD = 5427, I_VCMPNGT_UQPS = 5025, I_VCMPNGT_UQSD = 6231,
		I_VCMPNGT_UQSS = 5829, I_VCMPNLEPD = 5175, I_VCMPNLEPS = 4773, I_VCMPNLESD = 5979,
		I_VCMPNLESS = 5577, I_VCMPNLE_UQPD = 5373, I_VCMPNLE_UQPS = 4971, I_VCMPNLE_UQSD = 6177,
		I_VCMPNLE_UQSS = 5775, I_VCMPNLTPD = 5164, I_VCMPNLTPS = 4762, I_VCMPNLTSD = 5968,
		I_VCMPNLTSS = 5566, I_VCMPNLT_UQPD = 5359, I_VCMPNLT_UQPS = 4957, I_VCMPNLT_UQSD = 6163,
		I_VCMPNLT_UQSS = 5761, I_VCMPORDPD = 5186, I_VCMPORDPS = 4784, I_VCMPORDSD = 5990,
		I_VCMPORDSS = 5588, I_VCMPORD_SPD = 5387, I_VCMPORD_SPS = 4985, I_VCMPORD_SSD = 6191,
		I_VCMPORD_SSS = 5789, I_VCMPTRUEPD = 5279, I_VCMPTRUEPS = 4877, I_VCMPTRUESD = 6083,
		I_VCMPTRUESS = 5681, I_VCMPTRUE_USPD = 5497, I_VCMPTRUE_USPS = 5095, I_VCMPTRUE_USSD = 6301,
		I_VCMPTRUE_USSS = 5899, I_VCMPUNORDPD = 5140, I_VCMPUNORDPS = 4738, I_VCMPUNORDSD = 5944,
		I_VCMPUNORDSS = 5542, I_VCMPUNORD_SPD = 5330, I_VCMPUNORD_SPS = 4928, I_VCMPUNORD_SSD = 6134,
		I_VCMPUNORD_SSS = 5732, I_VCOMISD = 2818, I_VCOMISS = 2809, I_VCVTDQ2PD = 6841,
		I_VCVTDQ2PS = 3360, I_VCVTPD2DQ = 6852, I_VCVTPD2PS = 3296, I_VCVTPS2DQ = 3371,
		I_VCVTPS2PD = 3285, I_VCVTSD2SI = 2744, I_VCVTSD2SS = 3318, I_VCVTSI2SD = 2558,
		I_VCVTSI2SS = 2547, I_VCVTSS2SD = 3307, I_VCVTSS2SI = 2733, I_VCVTTPD2DQ = 6829,
		I_VCVTTPS2DQ = 3382, I_VCVTTSD2SI = 2681, I_VCVTTSS2SI = 2669, I_VDIVPD = 3550,
		I_VDIVPS = 3542, I_VDIVSD = 3566, I_VDIVSS = 3558, I_VDPPD = 9643, I_VDPPS = 9630,
		I_VERR = 1679, I_VERW = 1685, I_VEXTRACTF128 = 9538, I_VEXTRACTPS = 9513,
		I_VFMADD132PD = 8409, I_VFMADD132PS = 8396, I_VFMADD132SD = 8435, I_VFMADD132SS = 8422,
		I_VFMADD213PD = 8689, I_VFMADD213PS = 8676, I_VFMADD213SD = 8715, I_VFMADD213SS = 8702,
		I_VFMADD231PD = 8969, I_VFMADD231PS = 8956, I_VFMADD231SD = 8995, I_VFMADD231SS = 8982,
		I_VFMADDSUB132PD = 8348, I_VFMADDSUB132PS = 8332, I_VFMADDSUB213PD = 8628,
		I_VFMADDSUB213PS = 8612, I_VFMADDSUB231PD = 8908, I_VFMADDSUB231PS = 8892,
		I_VFMSUB132PD = 8461, I_VFMSUB132PS = 8448, I_VFMSUB132SD = 8487, I_VFMSUB132SS = 8474,
		I_VFMSUB213PD = 8741, I_VFMSUB213PS = 8728, I_VFMSUB213SD = 8767, I_VFMSUB213SS = 8754,
		I_VFMSUB231PD = 9021, I_VFMSUB231PS = 9008, I_VFMSUB231SD = 9047, I_VFMSUB231SS = 9034,
		I_VFMSUBADD132PD = 8380, I_VFMSUBADD132PS = 8364, I_VFMSUBADD213PD = 8660,
		I_VFMSUBADD213PS = 8644, I_VFMSUBADD231PD = 8940, I_VFMSUBADD231PS = 8924,
		I_VFNMADD132PD = 8514, I_VFNMADD132PS = 8500, I_VFNMADD132SD = 8542, I_VFNMADD132SS = 8528,
		I_VFNMADD213PD = 8794, I_VFNMADD213PS = 8780, I_VFNMADD213SD = 8822, I_VFNMADD213SS = 8808,
		I_VFNMADD231PD = 9074, I_VFNMADD231PS = 9060, I_VFNMADD231SD = 9102, I_VFNMADD231SS = 9088,
		I_VFNMSUB132PD = 8570, I_VFNMSUB132PS = 8556, I_VFNMSUB132SD = 8598, I_VFNMSUB132SS = 8584,
		I_VFNMSUB213PD = 8850, I_VFNMSUB213PS = 8836, I_VFNMSUB213SD = 8878, I_VFNMSUB213SS = 8864,
		I_VFNMSUB231PD = 9130, I_VFNMSUB231PS = 9116, I_VFNMSUB231SD = 9158, I_VFNMSUB231SS = 9144,
		I_VHADDPD = 4219, I_VHADDPS = 4228, I_VHSUBPD = 4253, I_VHSUBPS = 4262, I_VINSERTF128 = 9525,
		I_VINSERTPS = 9579, I_VLDDQU = 7023, I_VLDMXCSR = 9963, I_VMASKMOVDQU = 7153,
		I_VMASKMOVPD = 7971, I_VMASKMOVPS = 7959, I_VMAXPD = 3610, I_VMAXPS = 3602,
		I_VMAXSD = 3626, I_VMAXSS = 3618, I_VMCALL = 1735, I_VMCLEAR = 10011, I_VMFUNC = 1803,
		I_VMINPD = 3490, I_VMINPS = 3482, I_VMINSD = 3506, I_VMINSS = 3498, I_VMLAUNCH = 1743,
		I_VMLOAD = 1833, I_VMMCALL = 1824, I_VMOVAPD = 2498, I_VMOVAPS = 2489, I_VMOVD = 3954,
		I_VMOVDDUP = 2256, I_VMOVDQA = 3984, I_VMOVDQU = 3993, I_VMOVHLPS = 2217,
		I_VMOVHPD = 2404, I_VMOVHPS = 2395, I_VMOVLHPS = 2385, I_VMOVLPD = 2236, I_VMOVLPS = 2227,
		I_VMOVMSKPD = 2858, I_VMOVMSKPS = 2847, I_VMOVNTDQ = 6880, I_VMOVNTDQA = 7927,
		I_VMOVNTPD = 2615, I_VMOVNTPS = 2605, I_VMOVQ = 3961, I_VMOVSD = 2165, I_VMOVSHDUP = 2413,
		I_VMOVSLDUP = 2245, I_VMOVSS = 2157, I_VMOVUPD = 2148, I_VMOVUPS = 2139, I_VMPSADBW = 9659,
		I_VMPTRLD = 10002, I_VMPTRST = 6407, I_VMREAD = 4150, I_VMRESUME = 1753, I_VMRUN = 1817,
		I_VMSAVE = 1841, I_VMULPD = 3221, I_VMULPS = 3213, I_VMULSD = 3237, I_VMULSS = 3229,
		I_VMWRITE = 4174, I_VMXOFF = 1763, I_VMXON = 10020, I_VORPD = 3088, I_VORPS = 3081,
		I_VPABSB = 7717, I_VPABSD = 7747, I_VPABSW = 7732, I_VPACKSSDW = 3881, I_VPACKSSWB = 3713,
		I_VPACKUSDW = 7948, I_VPACKUSWB = 3791, I_VPADDB = 7233, I_VPADDD = 7263,
		I_VPADDQ = 6510, I_VPADDSB = 6960, I_VPADDSW = 6977, I_VPADDUSW = 6651, I_VPADDW = 7248,
		I_VPALIGNR = 9441, I_VPAND = 6635, I_VPANDN = 6694, I_VPAVGB = 6709, I_VPAVGW = 6754,
		I_VPBLENDVB = 9714, I_VPBLENDW = 9422, I_VPCLMULQDQ = 9680, I_VPCMPEQB = 4074,
		I_VPCMPEQD = 4112, I_VPCMPEQQ = 7907, I_VPCMPEQW = 4093, I_VPCMPESTRI = 9759,
		I_VPCMPESTRM = 9736, I_VPCMPGTB = 3733, I_VPCMPGTD = 3771, I_VPCMPGTQ = 8118,
		I_VPCMPGTW = 3752, I_VPCMPISTRI = 9805, I_VPCMPISTRM = 9782, I_VPERM2F128 = 9287,
		I_VPERMILPD = 7592, I_VPERMILPS = 7581, I_VPEXTRB = 9459, I_VPEXTRD = 9484,
		I_VPEXTRQ = 9493, I_VPEXTRW = 6341, I_VPHADDD = 7405, I_VPHADDSW = 7423, I_VPHADDW = 7388,
		I_VPHMINPOSUW = 8293, I_VPHSUBD = 7481, I_VPHSUBSW = 7499, I_VPHSUBW = 7464,
		I_VPINSRB = 9560, I_VPINSRD = 9606, I_VPINSRQ = 9615, I_VPINSRW = 6324, I_VPMADDUBSW = 7444,
		I_VPMADDWD = 7104, I_VPMAXSB = 8204, I_VPMAXSD = 8221, I_VPMAXSW = 6994, I_VPMAXUB = 6678,
		I_VPMAXUD = 8255, I_VPMAXUW = 8238, I_VPMINSB = 8136, I_VPMINSD = 8153, I_VPMINSW = 6932,
		I_VPMINUB = 6620, I_VPMINUD = 8187, I_VPMINUW = 8170, I_VPMOVMSKB = 6563,
		I_VPMOVSXBD = 7786, I_VPMOVSXBQ = 7807, I_VPMOVSXBW = 7765, I_VPMOVSXDQ = 7870,
		I_VPMOVSXWD = 7828, I_VPMOVSXWQ = 7849, I_VPMOVZXBD = 8014, I_VPMOVZXBQ = 8035,
		I_VPMOVZXBW = 7993, I_VPMOVZXDQ = 8098, I_VPMOVZXWD = 8056, I_VPMOVZXWQ = 8077,
		I_VPMULDQ = 7889, I_VPMULHRSW = 7570, I_VPMULHUW = 6771, I_VPMULHW = 6789,
		I_VPMULLD = 8272, I_VPMULLW = 6526, I_VPMULUDQ = 7085, I_VPOR = 6946, I_VPSADBW = 7122,
		I_VPSHUFB = 7371, I_VPSHUFD = 4036, I_VPSHUFHW = 4045, I_VPSHUFLW = 4055,
		I_VPSIGNB = 7517, I_VPSIGND = 7551, I_VPSIGNW = 7534, I_VPSLLD = 7053, I_VPSLLDQ = 9877,
		I_VPSLLQ = 7068, I_VPSLLW = 7038, I_VPSRAD = 6739, I_VPSRAW = 6724, I_VPSRLD = 6480,
		I_VPSRLDQ = 9860, I_VPSRLQ = 6495, I_VPSRLW = 6465, I_VPSUBB = 7173, I_VPSUBD = 7203,
		I_VPSUBQ = 7218, I_VPSUBSB = 6898, I_VPSUBSW = 6915, I_VPSUBUSB = 6583, I_VPSUBUSW = 6602,
		I_VPSUBW = 7188, I_VPTEST = 7658, I_VPUNPCKHBW = 3813, I_VPUNPCKHDQ = 3859,
		I_VPUNPCKHQDQ = 3929, I_VPUNPCKHWD = 3836, I_VPUNPCKLBW = 3645, I_VPUNPCKLDQ = 3691,
		I_VPUNPCKLQDQ = 3904, I_VPUNPCKLWD = 3668, I_VPXOR = 7009, I_VRCPPS = 2989,
		I_VRCPSS = 2997, I_VROUNDPD = 9327, I_VROUNDPS = 9308, I_VROUNDSD = 9365,
		I_VROUNDSS = 9346, I_VRSQRTPS = 2955, I_VRSQRTSS = 2965, I_VSHUFPD = 6375,
		I_VSHUFPS = 6366, I_VSQRTPD = 2910, I_VSQRTPS = 2901, I_VSQRTSD = 2928, I_VSQRTSS = 2919,
		I_VSTMXCSR = 9992, I_VSUBPD = 3430, I_VSUBPS = 3422, I_VSUBSD = 3446, I_VSUBSS = 3438,
		I_VTESTPD = 7612, I_VTESTPS = 7603, I_VUCOMISD = 2783, I_VUCOMISS = 2773,
		I_VUNPCKHPD = 2339, I_VUNPCKHPS = 2328, I_VUNPCKLPD = 2297, I_VUNPCKLPS = 2286,
		I_VXORPD = 3117, I_VXORPS = 3109, I_VZEROALL = 4140, I_VZEROUPPER = 4128,
		I_WAIT = 10042, I_WBINVD = 561, I_WRFSBASE = 9953, I_WRGSBASE = 9982, I_WRMSR = 586,
		I_XABORT = 1007, I_XADD = 946, I_XBEGIN = 1015, I_XCHG = 212, I_XEND = 1811,
		I_XGETBV = 1787, I_XLAT = 400, I_XOR = 61, I_XORPD = 3102, I_XORPS = 3095,
		I_XRSTOR = 4295, I_XRSTOR64 = 4303, I_XSAVE = 4271, I_XSAVE64 = 4278, I_XSAVEOPT = 4321,
		I_XSAVEOPT64 = 4331, I_XSETBV = 1795, I__3DNOW = 10056
	} _InstructionType;

	typedef enum {
		R_RAX, R_RCX, R_RDX, R_RBX, R_RSP, R_RBP, R_RSI, R_RDI, R_R8, R_R9, R_R10, R_R11, R_R12, R_R13, R_R14, R_R15,
		R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI, R_R8D, R_R9D, R_R10D, R_R11D, R_R12D, R_R13D, R_R14D, R_R15D,
		R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI, R_R8W, R_R9W, R_R10W, R_R11W, R_R12W, R_R13W, R_R14W, R_R15W,
		R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH, R_R8B, R_R9B, R_R10B, R_R11B, R_R12B, R_R13B, R_R14B, R_R15B,
		R_SPL, R_BPL, R_SIL, R_DIL,
		R_ES, R_CS, R_SS, R_DS, R_FS, R_GS,
		R_RIP,
		R_ST0, R_ST1, R_ST2, R_ST3, R_ST4, R_ST5, R_ST6, R_ST7,
		R_MM0, R_MM1, R_MM2, R_MM3, R_MM4, R_MM5, R_MM6, R_MM7,
		R_XMM0, R_XMM1, R_XMM2, R_XMM3, R_XMM4, R_XMM5, R_XMM6, R_XMM7, R_XMM8, R_XMM9, R_XMM10, R_XMM11, R_XMM12, R_XMM13, R_XMM14, R_XMM15,
		R_YMM0, R_YMM1, R_YMM2, R_YMM3, R_YMM4, R_YMM5, R_YMM6, R_YMM7, R_YMM8, R_YMM9, R_YMM10, R_YMM11, R_YMM12, R_YMM13, R_YMM14, R_YMM15,
		R_CR0, R_UNUSED0, R_CR2, R_CR3, R_CR4, R_UNUSED1, R_UNUSED2, R_UNUSED3, R_CR8,
		R_DR0, R_DR1, R_DR2, R_DR3, R_UNUSED4, R_UNUSED5, R_DR6, R_DR7
	} _RegisterType;
}

#endif