#include <windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <strsafe.h>
#include <stdio.h>
#include <conio.h>
#include <memory>

#include "rmt_data.h"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

void suspend();
void resume();

/*-- Rmt API --*/
DEFINE_REMOTE_API(RmtOpenProcess)
{
	HANDLE h_share = OpenShareMemory(OPEN_PROCESS_DATA_TYPE);
	DWORD memory_size = sizeof(OpenProcessDataT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	OpenProcessDataPtr opdt = (OpenProcessDataPtr)GetShareMemory(h_share, memory_size);
	if (!opdt)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	HANDLE h_process = OpenProcess(opdt->arg.dwDesiredAccess, opdt->arg.bInheritHandle, GetCurrentProcessId());
	if (!h_process)
	{
		opdt->ret.LastError = GetLastError();
		h_process = GetCurrentProcess();
	}

	SymInitialize(h_process, NULL, TRUE);

	opdt->ret.handle = h_process;
	UnmapViewOfFile(opdt);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtOpenThread)
{
	HANDLE h_share = OpenShareMemory(OPEN_THREAD_DATA_TYPR);
	DWORD memory_size = sizeof(OpenThreadDataT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	OpenThreadDataPtr otdp = (OpenThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!otdp)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	HANDLE h_thread = OpenThread(otdp->arg.dwDesiredAccess, otdp->arg.bInheritHandle, otdp->arg.dwThreadId);
	if (!h_thread)
	{
		otdp->ret.LastError = GetLastError();
	}

	otdp->ret.handle = h_thread;

	UnmapViewOfFile(otdp);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtVirtualQuery)
{
	HANDLE h_share = OpenShareMemory(VIRTUAL_QUERY_DATA_STRUCT);
	DWORD memory_size = sizeof(VirtualQueryDataT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	VirtualQueryDataPtr vqdp = (VirtualQueryDataPtr)GetShareMemory(h_share, memory_size);
	if (!vqdp)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	vqdp->ret.return_size = VirtualQuery(vqdp->arg.lpAddress, &vqdp->arg.Buffer, vqdp->arg.dwLength);
	if (vqdp->ret.return_size != sizeof(vqdp->arg.Buffer))
	{
		vqdp->ret.LastError = GetLastError();
		UnmapViewOfFile(vqdp);
		CloseHandle(h_share);

		SetEvent(return_event);
		return;
	}

	vqdp->ret.LastError = 0;

	UnmapViewOfFile(vqdp);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtGetThreadContext)
{
	HANDLE h_share = OpenShareMemory(GET_THREAD_CONTEXT_DATA_TYPE);
	DWORD memory_size = sizeof(GetThreadContextDataT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	GetThreadContextDataPtr gtc = (GetThreadContextDataPtr)GetShareMemory(h_share, memory_size);
	if (!gtc)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	gtc->ret.true_flase = GetThreadContext(gtc->arg.hThread, &gtc->arg.context);
	if (!gtc->ret.true_flase)
	{
		gtc->ret.LastError = GetLastError();
	}

	UnmapViewOfFile(gtc);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtCloseHandle)
{
	HANDLE h_share = OpenShareMemory(CLOSE_HANDLE_DATA_TYPE);
	DWORD memory_size = sizeof(CloseHandleDataType);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	CloseHandleDataPtr cd = (CloseHandleDataPtr)GetShareMemory(h_share, memory_size);
	if (!cd)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	cd->ret.LastError = 0;
	if (!CloseHandle(cd->arg.hObject))
	{
		cd->ret.LastError = GetLastError();
	}

	UnmapViewOfFile(cd);
	CloseHandle(h_share);
	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtResumeThread)
{
	HANDLE h_share = OpenShareMemory(RESUME_THREAD_DATA_TYPE);
	DWORD memory_size = sizeof(ResumeThreadDataType);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	ResumeThreadDataPtr rtd = (ResumeThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!rtd)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	rtd->ret.suspend_count = ResumeThread(rtd->arg.hThread);

	UnmapViewOfFile(rtd);
	CloseHandle(h_share);
	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtSuspendThread)
{
	HANDLE h_share = OpenShareMemory(SUSPEND_THREAD_DATA_STRUCT);
	DWORD memory_size = sizeof(SuspendThreadDataType);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	SuspendThreadDataPtr std = (SuspendThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!std)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	if (GetCurrentThreadId() != GetThreadId(std->arg.hThread))
	{
		std->ret.suspend_count = SuspendThread(std->arg.hThread);
	}

	UnmapViewOfFile(std);
	CloseHandle(h_share);
	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtReadProcessMemory)
{
	DWORD memory_size = sizeof(ReadProcessMemoryDataT);
	HANDLE h_share = OpenShareMemory(READ_PROCESS_MEMORY_DATA_STRUCT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	ReadProcessMemoryDataPtr rpm = (ReadProcessMemoryDataPtr)GetShareMemory(h_share, memory_size);
	if (!rpm)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	HANDLE h_buffer = OpenShareMemory(READ_WRITE_MEMORY_BUFFER_NAME);
	if (!h_buffer)
	{
		UnmapViewOfFile(rpm);
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	PBYTE buffer = (PBYTE)GetShareMemory(h_buffer, rpm->arg.nSize);
	if (!buffer)
	{
		UnmapViewOfFile(rpm);
		CloseHandle(h_buffer);
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	rpm->ret.LastError = 0;
#if 0
	void * test_ptr = GetModuleHandle(nullptr);
	if (!ReadProcessMemory(rpm->arg.hProcess, test_ptr, buffer, rpm->arg.nSize, &rpm->arg.NumberOfBytesRead))
#else
	if (!ReadProcessMemory(rpm->arg.hProcess, rpm->arg.lpBaseAddress, buffer, rpm->arg.nSize, &rpm->arg.NumberOfBytesRead))
#endif
	{
		rpm->ret.LastError = GetLastError();
	}

	UnmapViewOfFile(buffer);
	CloseHandle(h_buffer);

	UnmapViewOfFile(rpm);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtWriteProcessMemory)
{
	DWORD memory_size = sizeof(WriteProcessMemoryDataT);
	HANDLE h_share = OpenShareMemory(WRITE_PROCESS_MEMORY_DATA_STRUCT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	WriteProcessMemoryDataPtr wpm = (WriteProcessMemoryDataPtr)GetShareMemory(h_share, memory_size);
	if (!wpm)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	HANDLE h_buffer = OpenShareMemory(READ_WRITE_MEMORY_BUFFER_NAME);
	if (!h_buffer)
	{
		UnmapViewOfFile(wpm);
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	PBYTE buffer = (PBYTE)GetShareMemory(h_buffer, wpm->arg.nSize);
	if (!buffer)
	{
		UnmapViewOfFile(wpm);
		CloseHandle(h_buffer);
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	wpm->ret.LastError = 0;
	if (!WriteProcessMemory(wpm->arg.hProcess, wpm->arg.lpBaseAddress, buffer, wpm->arg.nSize, &wpm->arg.NumberOfBytesWritten))
	{
		wpm->ret.LastError = GetLastError();
	}

	UnmapViewOfFile(buffer);
	CloseHandle(h_buffer);

	UnmapViewOfFile(wpm);
	CloseHandle(h_share);

	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtSymFromAddr)
{
	DWORD memory_size = sizeof(SymFromAddrDataType);
	HANDLE h_share = OpenShareMemory(SYM_FROM_ADDR_DATA_STRUCT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	SymFromAddrDataPtr sfad = (SymFromAddrDataPtr)GetShareMemory(h_share, memory_size);
	if (!sfad)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}
	sfad->arg.Displacement = 0;

	IMAGEHLP_MODULE64 module_info = { 0, };
	module_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
	if (SymGetModuleInfo64(sfad->arg.hProcess, (DWORD64)sfad->arg.Address, &module_info))
	{
#define MAX_SYMBOL_NAME		sizeof(IMAGEHLP_MODULE64) + (sizeof(SYMBOL_INFO) + 128 * 2)

		char symbol_name[MAX_SYMBOL_NAME] = { 0, };
		StringCbCopyA(symbol_name, MAX_SYMBOL_NAME, module_info.ModuleName);

		CHAR buffer[sizeof(SYMBOL_INFO) + 128 * 2] = { 0, };
		PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)buffer;
		symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol_info->MaxNameLen = 128;

		if (SymFromAddr(sfad->arg.hProcess, sfad->arg.Address, &sfad->arg.Displacement, symbol_info))
		{
			StringCbCatA(symbol_name, MAX_SYMBOL_NAME, "!");
			StringCbCatA(symbol_name, MAX_SYMBOL_NAME, symbol_info->Name);
		}

		memcpy(sfad->arg.Symbol, symbol_name, MAX_SYMBOL_NAME);
	}
	else
	{
		sfad->ret.LastError = GetLastError();
	}

	UnmapViewOfFile(sfad);
	CloseHandle(h_share);

	SetEvent(return_event);
}

//
// simple api
//
DEFINE_REMOTE_API(RmtResumeProcess)
{
	resume();
	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtSuspendProcess)
{
	suspend();
	SetEvent(return_event);
}

DEFINE_REMOTE_API(RmtGetTargetThreadId)
{
	HANDLE h_share = OpenShareMemory(GET_TARGET_THREAD_ID_TABLE);
	DWORD memory_size = sizeof(GetTargetThreadIdTableDataT);
	if (!h_share)
	{
		SetEvent(return_event);
		return;
	}

	GetTargetThreadIdTableDataPtr gttit = (GetTargetThreadIdTableDataPtr)GetShareMemory(h_share, memory_size);
	if (!gttit)
	{
		CloseHandle(h_share);
		SetEvent(return_event);
		return;
	}

	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;
	DWORD s = 0;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (!h_snapshot)
	{
		gttit->ret.LastError = GetLastError();
		SetEvent(return_event);
		return;
	}

	thread_block32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(h_snapshot, &thread_block32))
	{
		gttit->ret.LastError = GetLastError();
		SetEvent(return_event);
		return;
	}

	unsigned long l = 0;
	do
	{
		if (thread_block32.th32OwnerProcessID == GetCurrentProcessId())
		{
			gttit->ret.tid_table[l++] = thread_block32.th32ThreadID;
		}
	} while (Thread32Next(h_snapshot, &thread_block32));

	gttit->ret.table_size = l;
	gttit->ret.LastError = 0;

	UnmapViewOfFile(gttit);
	CloseHandle(h_share);
	CloseHandle(h_snapshot);

	SetEvent(return_event);
}

//
// set dispatch table
//
DEFINE_REMOTE_API(RmtApiTemp)
{
	SetEvent(return_event);
}

void resume()
{
	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;
	DWORD s = 0;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (!h_snapshot)
	{
		return;
	}
	std::shared_ptr<void> snapchot_closer(h_snapshot, CloseHandle);

	thread_block32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(h_snapshot, &thread_block32))
	{
		return;
	}

	do
	{
		if (thread_block32.th32OwnerProcessID == GetCurrentProcessId() && thread_block32.th32ThreadID != GetCurrentThreadId())
		{
			h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, thread_block32.th32ThreadID);
			if (h_thread)
			{
				do
				{
					s = ResumeThread(h_thread);
				} while (s);
			}
			CloseHandle(h_thread);
		}
	} while (Thread32Next(h_snapshot, &thread_block32));
}

void suspend()
{
	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;
	DWORD s = 0;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (!h_snapshot)
	{
		return;
	}
	std::shared_ptr<void> snapchot_closer(h_snapshot, CloseHandle);

	thread_block32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(h_snapshot, &thread_block32))
	{
		return;
	}

	do
	{
		if (thread_block32.th32OwnerProcessID == GetCurrentProcessId() && thread_block32.th32ThreadID != GetCurrentThreadId())
		{
			h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, thread_block32.th32ThreadID);
			if (h_thread)
			{
				do
				{
					s = SuspendThread(h_thread);
				} while (!s);
			}
			CloseHandle(h_thread);
		}
	} while (Thread32Next(h_snapshot, &thread_block32));
}
