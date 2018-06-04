#define _CRT_SECURE_NO_WARNINGS

/*
	> shard_ptr로 수정 필요
*/
#include <windows.h>
#include <strsafe.h>

#include "rmt_data.h"

typedef void(*rmt_api_type)(HANDLE);
rmt_api_type rmt_api_dispatch_table[100];
void set_rmt_api_dispatch_table();

void suspend();
void resume();

void __stdcall rmt_dispatcher(void *args)
{
	WCHAR wchar_pid[MAX_PATH];
	ZeroMemory(wchar_pid, MAX_PATH);
	_itow(GetCurrentProcessId(), wchar_pid, 10);

	WCHAR remote_handle_name[MAX_PATH];
	ZeroMemory(remote_handle_name, MAX_PATH);
	StringCbCopy(remote_handle_name, MAX_PATH, RMTDLL_MAP_OBJECT);
	StringCbCat(remote_handle_name, MAX_PATH, L"-");
	StringCbCat(remote_handle_name, MAX_PATH, wchar_pid);

	/*-- open rmt handle --*/
	HANDLE remote_handlde = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, remote_handle_name);
	if (!remote_handlde)
	{
		return;
	}

	PDWORD remote_iid = (PDWORD)MapViewOfFile(remote_handlde, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DWORD));
	if (!remote_iid)
	{
		return;
	}

	/*-- open evnet --*/
	WCHAR signal_event_name[MAX_PATH];
	ZeroMemory(signal_event_name, MAX_PATH);
	StringCbCopy(signal_event_name, MAX_PATH, RMTDLL_SIGNAL_EVENT);
	StringCbCat(signal_event_name, MAX_PATH, L"-");
	StringCbCat(signal_event_name, MAX_PATH, wchar_pid);

	WCHAR return_event_name[MAX_PATH];
	ZeroMemory(return_event_name, MAX_PATH);
	StringCbCopy(return_event_name, MAX_PATH, RMTDLL_RETURN_EVENT);
	StringCbCat(return_event_name, MAX_PATH, L"-");
	StringCbCat(return_event_name, MAX_PATH, wchar_pid);

	HANDLE signal_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, signal_event_name);
	HANDLE return_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, return_event_name);
	if (!signal_event && !return_event)
	{
		return;
	}
	set_rmt_api_dispatch_table();

	do
	{
		WaitForSingleObject(signal_event, INFINITE);

		if (!rmt_api_dispatch_table[*remote_iid])
		{
			break;
		}
		rmt_api_dispatch_table[*remote_iid](return_event);
	} while (true);

	SetEvent(return_event);

	UnmapViewOfFile(remote_iid);
	CloseHandle(remote_handlde);
	CloseHandle(signal_event_name);
	CloseHandle(return_event_name);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
#if 0
		if (AllocConsole())
		{
			freopen("CONOUT$", "w", stdout);
			SetConsoleTitle(L"clog");
			SetConsoleTextAttribute(
				GetStdHandle(STD_OUTPUT_HANDLE)
				, FOREGROUND_RED
				| FOREGROUND_GREEN
				| FOREGROUND_BLUE
			);
		}
#endif

		DisableThreadLibraryCalls(hinstDLL);

		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)GetModuleHandle(nullptr);
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((unsigned long long)dos + dos->e_lfanew);

#if 1
		if (dos->e_magic == IMAGE_DOS_SIGNATURE && nt->Signature == IMAGE_NT_SIGNATURE)
		{
			suspend();

			unsigned long long entry = nt->OptionalHeader.ImageBase + nt->OptionalHeader.AddressOfEntryPoint;
			DWORD tid = 0;
			HANDLE dummy = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)entry, nullptr, CREATE_SUSPENDED, &tid);
			if (!dummy)
			{
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rmt_dispatcher, lpvReserved, 0, &tid);
			}
			else
			{
				if (QueueUserAPC((PAPCFUNC)rmt_dispatcher, dummy, (ULONG_PTR)lpvReserved) == 0)
				{
					tid = 0;
					CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rmt_dispatcher, lpvReserved, 0, &tid);
				}
			}

			resume();
		}
		else
#endif
		{
			DWORD tid = 0;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rmt_dispatcher, lpvReserved, 0, &tid);
		}
	}

	return TRUE;
}

//
// set dispatch table
//
DEFINE_REMOTE_API(RmtApiTemp);
DEFINE_REMOTE_API(RmtOpenProcess);
DEFINE_REMOTE_API(RmtOpenThread);
DEFINE_REMOTE_API(RmtVirtualQuery);
DEFINE_REMOTE_API(RmtGetThreadContext);
DEFINE_REMOTE_API(RmtCloseHandle);
DEFINE_REMOTE_API(RmtResumeThread);
DEFINE_REMOTE_API(RmtSuspendThread);
DEFINE_REMOTE_API(RmtReadProcessMemory);
DEFINE_REMOTE_API(RmtWriteProcessMemory);
DEFINE_REMOTE_API(RmtSymFromAddr);
DEFINE_REMOTE_API(RmtResumeProcess);
DEFINE_REMOTE_API(RmtSuspendProcess);
DEFINE_REMOTE_API(RmtGetTargetThreadId);

void set_rmt_api_dispatch_table()
{
	for (int i = 0; i < sizeof(rmt_api_dispatch_table); ++i)
	{
		rmt_api_dispatch_table[i] = RmtApiTemp;
	}

	rmt_api_dispatch_table[IID_REMOTE_API_DISPATCHER_CLOSE] = nullptr;

	rmt_api_dispatch_table[IID_REMOTE_API_OPEN_PROCESS] = RmtOpenProcess;
	rmt_api_dispatch_table[IID_REMOTE_API_OPEN_THREAD] = RmtOpenThread;
	rmt_api_dispatch_table[IID_REMOTE_API_VIRTUAL_QUERY] = RmtVirtualQuery;
	rmt_api_dispatch_table[IID_REMOTE_API_GET_THREAD_CONTEXT] = RmtGetThreadContext;

	rmt_api_dispatch_table[IID_REMOTE_API_READ_PROCESS] = RmtReadProcessMemory;
	rmt_api_dispatch_table[IID_REMOTE_API_WRITE_PROCESS] = RmtWriteProcessMemory;

	rmt_api_dispatch_table[IID_REMOTE_API_GET_TARGET_THREAD_TABLE] = RmtGetTargetThreadId;
	rmt_api_dispatch_table[IID_REMOTE_API_CLOSE_HANDLE] = RmtCloseHandle;
	rmt_api_dispatch_table[IID_REMOTE_API_RESUME_THREAD] = RmtResumeThread;
	rmt_api_dispatch_table[IID_REMOTE_API_SUSPEND_THREAD] = RmtSuspendThread;
	rmt_api_dispatch_table[IID_REMOTE_API_SYM_FROM_ADDR] = RmtSymFromAddr;

	rmt_api_dispatch_table[IID_REMOTE_API_RESUME_PROCESS] = RmtResumeProcess;
	rmt_api_dispatch_table[IID_REMOTE_API_SUSPEND_PROCESS] = RmtSuspendProcess;
	// ...
}





