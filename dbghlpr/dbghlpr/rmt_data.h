#ifndef _DEFINE_REMOTE_DATA_TYPE_
#define _DEFINE_REMOTE_DATA_TYPE_

#include <Windows.h>
#include <DbgHelp.h>

#define __IN
#define __OUT

#define DEFILE_REMOTE_API(func_name)	void func_name(HANDLE return_event)

/*-- Remote API --*/
#define RMTDLL_SIGNAL_EVENT		L"7C470390-0BD6-455C-8A07-3B0654335E4B"
#define RMTDLL_RETURN_EVENT		L"3AAB7476-70B5-45D6-B994-17BF99B6E5D9"
#define RMTDLL_MAP_OBJECT		L"FC112FF1-AB0F-4AA1-9E6F-C158A1B5A387"

typedef struct _TAG_RmtDllDataType_
{
	WCHAR signal_event_name[MAX_PATH];
	WCHAR return_event_name[MAX_PATH];
	DWORD remote_api_iid;
}RmtDllDataType, *RmtDllDataTypePtr;

typedef enum _TAG_RMTAPI_IID
{
	IID_REMOTE_API_DISPATCHER_CLOSE,
	IID_REMOTE_API_OPEN_PROCESS,
	IID_REMOTE_API_OPEN_THREAD,
	IID_REMOTE_API_VIRTUAL_QUERY,
	IID_REMOTE_API_GET_THREAD_CONTEXT,
	IID_REMOTE_API_READ_PROCESS,
	IID_REMOTE_API_WRITE_PROCESS,
	IID_REMOTE_API_GET_TARGET_THREAD_TABLE,
	IID_REMOTE_API_CLOSE_HANDLE,
	IID_REMOTE_API_RESUME_THREAD,
	IID_REMOTE_API_SUSPEND_THREAD,
	IID_REMOTE_API_SYM_FROM_ADDR,
	IID_REMOTE_API_RESUME_PROCESS,
	IID_REMOTE_API_SUSPEND_PROCESS
}RmtApiId;

/*-- open handle --*/
#define CreateShareMemory(memory_size, memory_name)\
		CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, memory_size, memory_name);
#define OpenShareMemory(memory_name)\
		OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, memory_name);
/*-- get memory --*/
#define GetShareMemory(handle, memory_size)\
		MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, memory_size);

/*-- Open Process --*/
#define OPEN_PROCESS_DATA_TYPE		L"{5942CCEE-6F8C-4D85-82C8-36914CEF3515}"

typedef struct _Tag_OpenProcessDataT_
{
	struct _ARGUMENT_
	{
		__IN unsigned long dwDesiredAccess;
		__IN unsigned long  bInheritHandle;
		//_In_ DWORD dwProcessId;
	}arg;

	struct _RETURN_
	{
		__OUT void * handle;
		__OUT unsigned long LastError;
	}ret;
}OpenProcessDataT, *OpenProcessDataPtr;


/*-- Open Thread --*/
#define OPEN_THREAD_DATA_TYPR		L"{4587914F-B085-4E97-9DC2-1246FCD31D71}"

typedef struct _TAG_OpenThreadDataT_
{
	struct _ARGUMENT_
	{
		__IN unsigned long dwDesiredAccess;
		__IN unsigned long bInheritHandle;
		__IN unsigned long dwThreadId;
	}arg;
	struct _RETURN_
	{
		__OUT void * handle;
		__OUT unsigned long LastError;
	}ret;
}OpenThreadDataT, *OpenThreadDataPtr;


/*-- Get Target Process Thread Table --*/
#define GET_TARGET_THREAD_ID_TABLE		L"{60EFDF89-2FC9-4C28-978C-131231A807ED}"

typedef struct _TAG_GetTargetThreadIdTableDataT_
{
	struct _RETURN_
	{
		__OUT unsigned long tid_table[1024];
		__OUT unsigned long table_size;
		__OUT unsigned long LastError;
	}ret;
}GetTargetThreadIdTableDataT, *GetTargetThreadIdTableDataPtr;


/*-- GetThreadContext --*/
#define GET_THREAD_CONTEXT_DATA_TYPE       L"{D6FE2855-7D7B-4701-AABB-952EEC208071}"

typedef struct _TAG_GetThreadContextDataT_
{
	struct _ARGUMENT_
	{
		_In_    void * hThread;
		_Inout_ CONTEXT	context;
	}arg;
	struct _RETURN_
	{
		_Out_ unsigned long true_flase;
		_Out_ unsigned long LastError;
	}ret;
}GetThreadContextDataT, *GetThreadContextDataPtr;


/*-- SetThreadContext --*/
#define SET_THREAD_CONTEXT_DATA_TYPE       L"{D19B5582-685E-473D-9B86-8654ADD39393}"

typedef struct _TAG_SetThreadContextDataT_
{
	struct _ARGUMENT_
	{
		_In_    void * hThread;
		_Inout_ CONTEXT	context;
	}arg;
	struct _RETURN_
	{
		_Out_ unsigned long LastError;
	}ret;
}SetThreadContextDataT, *SetThreadContextDataPtr;


/*-- VirtualQuery --*/
#define VIRTUAL_QUERY_DATA_STRUCT		L"{1EDDE402-E5A8-483B-BEDF-E8D5BB46F43D}"

typedef struct _TAG_VirtualQueryDataT_
{
	struct _ARGUMENT_
	{
		_In_opt_ void *lpAddress;
		_Out_    MEMORY_BASIC_INFORMATION Buffer;
		_In_     SIZE_T dwLength;
	}arg;
	struct _RETURN_
	{
		_Out_ SIZE_T return_size;
		_Out_ DWORD LastError;
	}ret;
}VirtualQueryDataT, *VirtualQueryDataPtr;


/*-- RW Memory Name --*/
#define READ_WRITE_MEMORY_BUFFER_NAME		L"{E724844A-C71E-4682-ADE7-F08148C8EF73}"

/*-- ReadProcessMemory --*/
#define READ_PROCESS_MEMORY_DATA_STRUCT		L"{E26B3819-FF08-45A0-ADCD-E23DA95C51C7}"

typedef struct _TAG_READ_PROCESS_MEMORY_
{
	struct _ARGUMENT_
	{
		_In_  HANDLE  hProcess;
		_In_  LPCVOID lpBaseAddress;
		_Out_ LPVOID  lpBuffer;
		_In_  SIZE_T  nSize;
		_Out_ SIZE_T  NumberOfBytesRead;
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD LastError;
	}ret;
}ReadProcessMemoryDataT, *ReadProcessMemoryDataPtr;

/*-- WriteProcessMemory --*/
#define WRITE_PROCESS_MEMORY_DATA_STRUCT		L"{3C40A970-1F77-4643-BEC0-F1F36D14BB3A}"

typedef struct _TAG_WRITE_PROCESS_MEMORY_
{
	struct _ARGUMENT_
	{
		_In_  HANDLE  hProcess;
		_In_  LPVOID  lpBaseAddress;
		_In_  LPVOID lpBuffer;
		_In_  SIZE_T  nSize;
		_Out_ SIZE_T  NumberOfBytesWritten;
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD LastError;
	}ret;
}WriteProcessMemoryDataT, *WriteProcessMemoryDataPtr;


/*-- CloseHandle --*/
#define CLOSE_HANDLE_DATA_TYPE		L"{4F26C054-D732-4B00-BA09-09FBCE64E63A}"

typedef struct _TAG_CloseHandleDataType_
{
	struct _ARGUMENT_
	{
		_In_ HANDLE hObject;
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD LastError;
	}ret;
}CloseHandleDataType, *CloseHandleDataPtr;


/*-- ResumeThread --*/
#define RESUME_THREAD_DATA_TYPE		L"{54F8BDDA-B478-4BF2-85CE-59516672D788}"

typedef struct _TAG_ResumeThreadDataType_
{
	struct _ARGUMENT_
	{
		_In_ HANDLE hThread;
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD suspend_count;
	}ret;
}ResumeThreadDataType, *ResumeThreadDataPtr;


/*-- SuspendThread --*/
#define SUSPEND_THREAD_DATA_STRUCT		L"{1731F236-22EB-4D32-9DB5-DAF30D4CF917}"

typedef struct _TAG_SuspendThreadDataType_
{
	struct _ARGUMENT_
	{
		_In_ HANDLE hThread;
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD suspend_count;
	}ret;
}SuspendThreadDataType, *SuspendThreadDataPtr;


/*-- Symbol --*/
#define SYM_FROM_ADDR_DATA_STRUCT		L"{ED13BDE7-9E6E-4A99-AF49-A117A71FF25D}"

typedef struct _TAG_SymFromAddrDataType_
{
	struct _ARGUMENT_
	{
		_In_      HANDLE       hProcess;
		_In_      DWORD64      Address;
		_Out_opt_ DWORD64     Displacement;
		_Inout_	  CHAR Symbol[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)]; // => 수정 필요
	}arg;
	struct _RETURN_
	{
		_Out_ DWORD LastError;
	}ret;
}SymFromAddrDataType, *SymFromAddrDataPtr;

#endif