#define _CRT_SECURE_NO_WARNINGS

#include <rmtdll_linker.h>
#include <strsafe.h>
#include <memory>

rmtdll_linker::rmtdll_linker()
{
}

rmtdll_linker::~rmtdll_linker()
{
	CloseHandle(rmt_data_handle_);
	CloseHandle(rmt_signal_event_);
	CloseHandle(rmt_return_event_);
}

void rmtdll_linker::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(rmtdll_linker);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;

	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

void * rmtdll_linker::get_object(unsigned long id)
{
	void *i = nullptr;

	switch (id)
	{
	case dbg::OBJECT_ID::TARGET_CONTROLLER_ID:
		break;

	case dbg::OBJECT_ID::TARGET_PROCESS_HANDLE_ID:
		break;

	case dbg::OBJECT_ID::TARGET_THREAD_HANDLE_ID:
		break;

	default:
		break;
	}

	return i;
}

//
//
//
bool rmtdll_linker::open(char *path) // 시스템 파일을 교체하여 인젝션
{
	char cp[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, cp);

	char cmd[1024] = { 0, };
	sprintf(cmd, "%s\\dbghlpr\\dlldr.exe %s rmtdll.dll", cp, path);
	if (WinExec(cmd, SW_SHOW) == 33)
	{
		return true;
	}

	return false;
}

//
// 초기화 성공시 rmt_signal_event_ 및 rmt_return_event_, rmt_control_iid_ 는 제거하지 않는다.
//
bool rmtdll_linker::open(unsigned long pid)
{
	//
	// init rmtdll
	//
	WCHAR path[MAX_PATH];
	ZeroMemory(path, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, path);

	WCHAR wchar_pid[MAX_PATH];
	ZeroMemory(wchar_pid, MAX_PATH);
	_itow(pid, wchar_pid, 10);

	WCHAR remote_handle_name[MAX_PATH];
	ZeroMemory(remote_handle_name, MAX_PATH);
	StringCbCopy(remote_handle_name, MAX_PATH, RMTDLL_MAP_OBJECT);
	StringCbCat(remote_handle_name, MAX_PATH, L"-");
	StringCbCat(remote_handle_name, MAX_PATH, wchar_pid);

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

	rmt_data_handle_ = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, remote_handle_name);
	if (!rmt_data_handle_)
	{
		rmt_data_handle_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(RmtDllDataType), remote_handle_name);
		if (!rmt_data_handle_)
		{
			return false;
		}
	}

	rmt_signal_event_ = OpenEvent(EVENT_ALL_ACCESS, TRUE, signal_event_name);
	if (!rmt_signal_event_)
	{
		rmt_signal_event_ = CreateEvent(0, FALSE, FALSE, signal_event_name);
	}

	rmt_return_event_ = OpenEvent(EVENT_ALL_ACCESS, TRUE, return_event_name);
	if (!rmt_return_event_)
	{
		rmt_return_event_ = CreateEvent(0, FALSE, FALSE, return_event_name);
	}

	if (!rmt_signal_event_ || !rmt_return_event_)
	{
		CloseHandle(rmt_data_handle_);
		return false;
	}

	rmt_control_iid_ = (PDWORD)MapViewOfFile(rmt_data_handle_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DWORD));
	if (!rmt_control_iid_)
	{
		CloseHandle(rmt_data_handle_);
		CloseHandle(rmt_signal_event_);
		CloseHandle(rmt_return_event_);
		return false;
	}

	//
	// install rmt api dispatcher
	//
	module_load_information_type module_load_info;
	DLL_ARGS dll_args;
	if (!install(L"rmtdll.dll", pid, &module_load_info, dll_args))
	{
		UnmapViewOfFile(rmt_control_iid_);

		CloseHandle(rmt_data_handle_);
		CloseHandle(rmt_signal_event_);
		CloseHandle(rmt_return_event_);

		return false;
	}

	//
	// link RmtOpenProcess
	//
	unsigned long memory_size = sizeof(OpenProcessDataT);
	HANDLE h_share = CreateShareMemory(memory_size, OPEN_PROCESS_DATA_TYPE);
	if (!h_share)
	{
		return false;
	}

	OpenProcessDataPtr opdp = (OpenProcessDataPtr)GetShareMemory(h_share, memory_size);
	if (!opdp)
	{
		CloseHandle(h_share);

		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_OPEN_PROCESS;
	opdp->arg.dwDesiredAccess = MAXIMUM_ALLOWED;
	opdp->arg.bInheritHandle = FALSE;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	process_handle_ = opdp->ret.handle;
	if (!process_handle_)
	{
		SetLastError(opdp->ret.LastError);
		UnmapViewOfFile(opdp);
		CloseHandle(h_share);

		return false;
	}

	//
	// Get current thread id
	//
	std::list<unsigned long> tid_list;
	if (get_thread_id_list(tid_list))
	{
		std::list<unsigned long>::iterator it = tid_list.begin();
		current_tid_ = *it;
	}

	return true;
}

bool rmtdll_linker::close()
{
	*rmt_control_iid_ = IID_REMOTE_API_DISPATCHER_CLOSE;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	return true;
}

bool rmtdll_linker::get_thread_id_list(std::list<unsigned long> &tid_list)
{
	unsigned long memory_size = sizeof(GetTargetThreadIdTableDataT);
	HANDLE h_share = CreateShareMemory(memory_size, GET_TARGET_THREAD_ID_TABLE);
	if (!h_share)
	{
		return false;
	}

	GetTargetThreadIdTableDataPtr gttit = (GetTargetThreadIdTableDataPtr)GetShareMemory(h_share, memory_size);
	if (!gttit)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_GET_TARGET_THREAD_TABLE;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (gttit->ret.LastError)
	{
		SetLastError(gttit->ret.LastError);
	}

	for (unsigned long i = 0; i < gttit->ret.table_size; ++i)
	{
		tid_list.push_back(gttit->ret.tid_table[i]);
	}

	UnmapViewOfFile(gttit);
	CloseHandle(h_share);

	return true;
}

bool rmtdll_linker::select_thread(unsigned long tid)
{
	current_tid_ = tid;
	return true;
}

unsigned long rmtdll_linker::current_thread_id()
{
	return current_tid_;
}

bool rmtdll_linker::query_virtual_memory(unsigned long long virtual_address, void *out_memory_info)
{
	unsigned long memory_size = sizeof(VirtualQueryDataT);
	HANDLE h_share = CreateShareMemory(memory_size, VIRTUAL_QUERY_DATA_STRUCT);
	if (!h_share)
	{
		return false;
	}

	VirtualQueryDataPtr vqdp = (VirtualQueryDataPtr)GetShareMemory(h_share, memory_size);
	if (!vqdp)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_VIRTUAL_QUERY;
	vqdp->arg.lpAddress = (void *)virtual_address;
	vqdp->arg.dwLength = sizeof(MEMORY_BASIC_INFORMATION);

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	PMEMORY_BASIC_INFORMATION mbi = (PMEMORY_BASIC_INFORMATION)out_memory_info;
	if (vqdp->ret.LastError)
	{
		SetLastError(vqdp->ret.LastError);

		UnmapViewOfFile(vqdp);
		CloseHandle(h_share);
		return false;
	}
	memcpy(mbi, &vqdp->arg.Buffer, vqdp->arg.dwLength);

	UnmapViewOfFile(vqdp);
	CloseHandle(h_share);

	return true;
}

void * rmtdll_linker::virtual_alloc(unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type)
{
	return nullptr;
}

int rmtdll_linker::virtual_free(void *ptr, unsigned long size, unsigned long free_type)
{
	return 0;
}

unsigned long __stdcall rmtdll_linker::read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	DWORD memory_size = sizeof(ReadProcessMemoryDataT);
	HANDLE h_share = CreateShareMemory(memory_size, READ_PROCESS_MEMORY_DATA_STRUCT);
	if (!h_share)
	{
		return 0; 
	}
	std::shared_ptr<void> shared_handle_closer(h_share, CloseHandle);

	ReadProcessMemoryDataPtr rpm = (ReadProcessMemoryDataPtr)GetShareMemory(h_share, memory_size);
	if (!rpm)
	{
		return 0;
	}
	std::shared_ptr<void> rpm_closer(rpm, UnmapViewOfFile);

	HANDLE h_buffer = CreateShareMemory(read_size, READ_WRITE_MEMORY_BUFFER_NAME);
	if (!h_buffer)
	{
		return 0;
	}
	std::shared_ptr<void> buffer_handle_closer(h_buffer, CloseHandle);

	rpm->arg.lpBuffer = (PBYTE)GetShareMemory(h_buffer, read_size);
	if (!rpm->arg.lpBuffer)
	{
		return 0;
	}
	std::shared_ptr<void> buffer_closer(rpm->arg.lpBuffer, UnmapViewOfFile);
	ZeroMemory(rpm->arg.lpBuffer, read_size);

	*rmt_control_iid_ = IID_REMOTE_API_READ_PROCESS;
	rpm->arg.hProcess = process_handle_;
	rpm->arg.lpBaseAddress = (void *)virtual_address;
	rpm->arg.nSize = read_size;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (rpm->ret.LastError)
	{
		SetLastError(rpm->ret.LastError);
		return 0;
	}

	memcpy(out_memory, rpm->arg.lpBuffer, read_size);
	return rpm->arg.NumberOfBytesRead;
}

unsigned long __stdcall rmtdll_linker::write_virtual_memory(unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size)
{
	DWORD memory_size = sizeof(WriteProcessMemoryDataT);
	HANDLE h_share = CreateShareMemory(memory_size, READ_PROCESS_MEMORY_DATA_STRUCT);
	if (!h_share)
	{
		return 0;
	}
	std::shared_ptr<void> shared_handle_closer(h_share, CloseHandle);

	WriteProcessMemoryDataPtr wpm = (WriteProcessMemoryDataPtr)GetShareMemory(h_share, memory_size);
	if (!wpm)
	{
		return 0;
	}
	std::shared_ptr<void> rpm_closer(wpm, UnmapViewOfFile);

	HANDLE h_buffer = CreateShareMemory(write_size, READ_WRITE_MEMORY_BUFFER_NAME);
	if (!h_buffer)
	{
		return 0;
	}
	std::shared_ptr<void> buffer_handle_closer(h_buffer, CloseHandle);

	wpm->arg.lpBuffer = (PBYTE)GetShareMemory(h_buffer, write_size);
	if (!wpm->arg.lpBuffer)
	{
		return 0;
	}
	std::shared_ptr<void> buffer_closer(wpm->arg.lpBuffer, UnmapViewOfFile);
	ZeroMemory((void *)wpm->arg.lpBuffer, write_size);
	memcpy((LPVOID)wpm->arg.lpBuffer, input_memory, write_size);

	*rmt_control_iid_ = IID_REMOTE_API_WRITE_PROCESS;
	wpm->arg.hProcess = process_handle_;
	wpm->arg.lpBaseAddress = (void *)virtual_address;
	wpm->arg.nSize = write_size;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (wpm->ret.LastError)
	{
		SetLastError(wpm->ret.LastError);
		return 0;
	}

	return wpm->arg.NumberOfBytesWritten;
}

bool __stdcall rmtdll_linker::get_thread_context(cpu_context_type *context)
{
	/*-- RmtOpenThread --*/
	unsigned long memory_size = sizeof(OpenThreadDataT);
	HANDLE h_share = CreateShareMemory(memory_size, OPEN_THREAD_DATA_TYPR);
	if (!h_share)
	{
		return false;
	}

	OpenThreadDataPtr optd = (OpenThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!optd)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_OPEN_THREAD;
	optd->arg.dwThreadId = current_tid_;
	optd->arg.bInheritHandle = false;
	optd->arg.dwDesiredAccess = MAXIMUM_ALLOWED;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (!optd->ret.handle)
	{
		SetLastError(optd->ret.LastError);

		UnmapViewOfFile(optd);
		CloseHandle(h_share);

		return false;
	}

	HANDLE thread_handle = optd->ret.handle;
	UnmapViewOfFile(optd);
	CloseHandle(h_share);

	/*-- GetThreadContext --*/
	memory_size = sizeof(GetThreadContextDataT);
	h_share = CreateShareMemory(memory_size, GET_THREAD_CONTEXT_DATA_TYPE);
	if (!h_share)
	{
		return false;
	}

	GetThreadContextDataPtr gtcd = (GetThreadContextDataPtr)GetShareMemory(h_share, memory_size);
	if (!gtcd)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_GET_THREAD_CONTEXT;
	gtcd->arg.hThread = thread_handle;
	gtcd->arg.context.ContextFlags = CONTEXT_ALL;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (!gtcd->ret.true_flase)
	{
		SetLastError(gtcd->ret.LastError);
		UnmapViewOfFile(gtcd);
		CloseHandle(h_share);

		return false;
	}

#ifndef _WIN64
	context->rax = gtcd->arg.context.Eax;
	context->rbx = gtcd->arg.context.Ebx;
	context->rcx = gtcd->arg.context.Ecx;
	context->rdx = gtcd->arg.context.Edx;

	context->rdi = gtcd->arg.context.Edi;
	context->rsi = gtcd->arg.context.Esi;

	context->rbp = gtcd->arg.context.Ebp;
	context->rsp = gtcd->arg.context.Esp;

	context->rip = gtcd->arg.context.Eip;
#else
#endif
	context->efl = gtcd->arg.context.EFlags;

	context->dr0 = gtcd->arg.context.Dr0;
	context->dr1 = gtcd->arg.context.Dr1;
	context->dr2 = gtcd->arg.context.Dr2;
	context->dr3 = gtcd->arg.context.Dr3;
	context->dr6 = gtcd->arg.context.Dr6;
	context->dr7 = gtcd->arg.context.Dr7;

	UnmapViewOfFile(gtcd);
	CloseHandle(h_share);

	return true;
}

bool __stdcall rmtdll_linker::set_thread_context(cpu_context_type *context)
{
	return false;
}

bool rmtdll_linker::get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	unsigned long memory_size = sizeof(SymFromAddrDataType);
	HANDLE h_share = CreateShareMemory(memory_size, SYM_FROM_ADDR_DATA_STRUCT);
	if (!h_share)
	{
		return false;
	}
	std::shared_ptr<void> share_handle_closer(h_share, CloseHandle);

	SymFromAddrDataPtr sfad = (SymFromAddrDataPtr)GetShareMemory(h_share, memory_size);
	if (!sfad)
	{
		CloseHandle(h_share);
		return false;
	}
	std::shared_ptr<void> share_memory_closer(sfad, UnmapViewOfFile);

	*rmt_control_iid_ = IID_REMOTE_API_SYM_FROM_ADDR;
	sfad->arg.hProcess = process_handle_;
	sfad->arg.Address = offset;
	PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)sfad->arg.Symbol;
	symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol_info->MaxNameLen = 128;

	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (sfad->ret.LastError)
	{
		SetLastError(sfad->ret.LastError);
		return false;
	}

	memcpy(buffer, symbol_info, size_of_buffer);
	*disp = sfad->arg.Displacement;

	return true;
}

bool rmtdll_linker::stack_trace(unsigned long tid, stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count)
{
	return false;
}

bool rmtdll_linker::write_dump(char *path)
{
	return false;
}

//
//
//
bool rmtdll_linker::close_handle(void *handle)
{
	unsigned long memory_size = sizeof(CloseHandleDataType);
	HANDLE h_share = CreateShareMemory(memory_size, CLOSE_HANDLE_DATA_TYPE);
	if (!h_share)
	{
		return false;
	}

	CloseHandleDataPtr cd = (CloseHandleDataPtr)GetShareMemory(h_share, memory_size);
	if (!cd)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_CLOSE_HANDLE;
	cd->arg.hObject = handle;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (cd->ret.LastError)
	{
		SetLastError(cd->ret.LastError);
		UnmapViewOfFile(cd);
		CloseHandle(h_share);

		return false;
	}

	UnmapViewOfFile(cd);
	CloseHandle(h_share);

	return true;
}

bool rmtdll_linker::resume(unsigned long tid)
{
#if 0
	/*-- RmtOpenThread --*/
	unsigned long memory_size = sizeof(OpenThreadDataT);
	HANDLE h_share = CreateShareMemory(memory_size, OPEN_THREAD_DATA_TYPR);
	if (!h_share)
	{
		return false;
	}

	OpenThreadDataPtr optd = (OpenThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!optd)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_OPEN_THREAD;
	optd->arg.dwThreadId = tid;
	optd->arg.bInheritHandle = false;
	optd->arg.dwDesiredAccess = MAXIMUM_ALLOWED;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (!optd->ret.handle)
	{
		SetLastError(optd->ret.LastError);

		UnmapViewOfFile(optd);
		CloseHandle(h_share);

		return false;
	}

	HANDLE thread_handle = optd->ret.handle;
	UnmapViewOfFile(optd);
	CloseHandle(h_share);
#endif
	/*-- ResumeThread --*/
	*rmt_control_iid_ = IID_REMOTE_API_RESUME_PROCESS;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	return true;
}

bool rmtdll_linker::suspend(unsigned long tid)
{
#if 0
	/*-- RmtOpenThread --*/
	unsigned long memory_size = sizeof(OpenThreadDataT);
	HANDLE h_share = CreateShareMemory(memory_size, OPEN_THREAD_DATA_TYPR);
	if (!h_share)
	{
		return false;
	}

	OpenThreadDataPtr optd = (OpenThreadDataPtr)GetShareMemory(h_share, memory_size);
	if (!optd)
	{
		CloseHandle(h_share);
		return false;
	}

	*rmt_control_iid_ = IID_REMOTE_API_OPEN_THREAD;
	optd->arg.dwThreadId = tid;
	optd->arg.bInheritHandle = false;
	optd->arg.dwDesiredAccess = MAXIMUM_ALLOWED;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	if (!optd->ret.handle)
	{
		SetLastError(optd->ret.LastError);

		UnmapViewOfFile(optd);
		CloseHandle(h_share);

		return false;
	}

	HANDLE thread_handle = optd->ret.handle;
	UnmapViewOfFile(optd);
	CloseHandle(h_share);
#endif
	/*-- SuspendThread --*/
	*rmt_control_iid_ = IID_REMOTE_API_SUSPEND_PROCESS;
	SetEvent(rmt_signal_event_);
	WaitForSingleObject(rmt_return_event_, INFINITE);

	return true;
}