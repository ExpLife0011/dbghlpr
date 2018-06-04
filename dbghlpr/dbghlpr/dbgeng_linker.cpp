#include <windows.h>
#include <memory>
#include <strsafe.h>
#include <dbgeng.h>
#include <stdio.h>

#include <interface.h>
#include <dbgeng_linker.h>

#pragma comment(lib, "dbgeng.lib")

engine_linker::engine_linker()
{
	if (DebugCreate(__uuidof(IDebugClient), (void **)&debug_client_) != S_OK)
	{
		debug_client_ = nullptr;
	}
}

engine_linker::~engine_linker() { }

//
//
//
void engine_linker::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(engine_linker);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;

	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

void * engine_linker::get_object(unsigned long id)
{
	void *i = nullptr;
	if (!debug_client_)
	{
		return false;
	}

	switch (id)
	{
	case dbg::OBJECT_ID::TARGET_CONTROLLER_ID:
		i = debug_client_;
		break;

	case dbg::OBJECT_ID::TARGET_PROCESS_HANDLE_ID:
		if (is_user_mode_debugging())
		{
			IDebugSystemObjects *system_object;
			if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSystemObjects), (void **)&system_object) != S_OK)
			{
				break;
			}

			unsigned long long process_handle = 0;
			if (system_object->GetCurrentProcessHandle(&process_handle) != S_OK)
			{
				break;
			}

			i = (void *)process_handle;
		}
		break;

	case dbg::OBJECT_ID::TARGET_THREAD_HANDLE_ID:
		if (is_user_mode_debugging())
		{
			IDebugSystemObjects *system_object;
			if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSystemObjects), (void **)&system_object) != S_OK)
			{
				break;
			}

			unsigned long long thread_handle = 0;
			if (system_object->GetCurrentThreadHandle(&thread_handle) != S_OK)
			{
				break;
			}

			i = (void *)thread_handle;
		}
		break;

	default:
		break;
	}

	return i;
}

bool engine_linker::is_user_mode_debugging()
{
	if (!debug_client_)
	{
		return false;
	}

	IDebugControl3 *debug_control;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugControl3), (void **)&debug_control) != S_OK)
	{
		return false;
	}

	unsigned long dbg_class = 0;
	unsigned long dbg_qualifier = 0;
	if (debug_control->GetDebuggeeType(&dbg_class, &dbg_qualifier) != S_OK)
	{
		return false;
	}

	return dbg_class == DEBUG_CLASS_USER_WINDOWS && dbg_qualifier == DEBUG_USER_WINDOWS_PROCESS;
}

bool engine_linker::is_kernel_mode_debugging()
{
	if (!debug_client_)
	{
		return false;
	}

	IDebugControl3 *debug_control;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugControl3), (void **)&debug_control) != S_OK)
	{
		return false;
	}

	unsigned long dbg_class = 0;
	unsigned long dbg_qualifier = 0;
	if (debug_control->GetDebuggeeType(&dbg_class, &dbg_qualifier) != S_OK)
	{
		return false;
	}

	return dbg_class == DEBUG_CLASS_KERNEL && dbg_qualifier == DEBUG_USER_WINDOWS_PROCESS;
}

//
//
//
bool engine_linker::open(char *path)
{
	if (!debug_client_)
	{
		return false;
	}

	if (((IDebugClient *)debug_client_)->OpenDumpFile(path) != S_OK)
	{
		return false;
	}

	IDebugControl3 *debug_control;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugControl3), (void **)&debug_control) != S_OK)
	{
		return false;
	}

	if (debug_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK)
	{
		return false;
	}

	return true;
}

bool engine_linker::open(unsigned long pid)
{
	if (!debug_client_)
	{
		return false;
	}

	if (((IDebugClient *)debug_client_)->AttachProcess(0, pid, DEBUG_ATTACH_NONINVASIVE) != S_OK)
	{
		return false;
	}

	IDebugControl3 *debug_control;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugControl3), (void **)&debug_control) != S_OK)
	{
		return false;
	}

	if (debug_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK)
	{
		return false;
	}

	return true;
}

bool engine_linker::close()
{
	if (!debug_client_)
	{
		return false;
	}

	if (((IDebugClient *)debug_client_)->DetachProcesses() != S_OK)
	{
		return false;
	}

	return true;
}

//
//
//
bool engine_linker::get_thread_id_list(std::list<unsigned long> &tid_list)
{
	unsigned long thread_count = 0;
	if (!debug_client_)
	{
		return false;
	}

	IDebugSystemObjects *system_object;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSystemObjects), (void **)&system_object) != S_OK)
	{
		return false;
	}

	if (system_object->GetNumberThreads(&thread_count) != S_OK)
	{
		return false;
	}

	unsigned long *tids = (unsigned long *)malloc(sizeof(unsigned long) * thread_count);
	if (!tids)
	{
		return false;
	}
	memset(tids, 0, sizeof(unsigned long) * thread_count);

	if (system_object->GetThreadIdsByIndex(0, thread_count, nullptr, tids) != S_OK)
	{
		free(tids);
		return false;
	}

	for (unsigned long i = 0; i < thread_count; ++i)
	{
		tid_list.push_back(tids[i]);
	}
	free(tids);

	return true;
}

bool engine_linker::select_thread(unsigned long tid)
{
	if (!debug_client_)
	{
		return false;
	}

	IDebugSystemObjects4 *system_object;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSystemObjects4), (void **)&system_object) != S_OK)
	{
		return false;
	}

	unsigned long id = 0;
	if (system_object->GetThreadIdBySystemId(tid, &id) != S_OK)
	{
		return false;
	}

	if (system_object->SetCurrentThreadId(id) != S_OK)
	{
		return false;
	}

	return true;
}

unsigned long engine_linker::current_thread_id()
{
	if (!debug_client_)
	{
		return 0;
	}

	IDebugSystemObjects4 *system_object;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSystemObjects4), (void **)&system_object) != S_OK)
	{
		return 0;
	}

	unsigned long tid = 0;
	if (system_object->GetCurrentThreadSystemId(&tid) != S_OK)
	{
		return 0;
	}

	return tid;
}


//
//
//
bool engine_linker::query_virtual_memory(unsigned long long virtual_address, void *out_memory_info)
{
	if (!debug_client_)
	{
		return false;
	}

	MEMORY_BASIC_INFORMATION64 *out_mbi = (MEMORY_BASIC_INFORMATION64 *)out_memory_info;

	IDebugDataSpaces2 *debug_data_space;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugDataSpaces2), (void **)&debug_data_space) != S_OK)
	{
		return false;
	}

	if (debug_data_space->QueryVirtual(virtual_address, out_mbi) == S_OK)
	{
		return true;
	}

	return false;
}

void * engine_linker::virtual_alloc(unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type)
{
	if (!debug_client_)
	{
		return nullptr;
	}

	HANDLE process_handle = get_object(dbg::OBJECT_ID::TARGET_PROCESS_HANDLE_ID);
	if (!process_handle)
	{
		return nullptr;
	}

	return VirtualAllocEx(process_handle, nullptr, size, allocation_type, protect_type);
}

int engine_linker::virtual_free(void *ptr, unsigned long size, unsigned long free_type)
{
	if (!debug_client_)
	{
		return 0;
	}

	HANDLE process_handle = get_object(dbg::OBJECT_ID::TARGET_PROCESS_HANDLE_ID);
	if (!process_handle)
	{
		return 0;
	}

	return VirtualFreeEx(process_handle, ptr, size, free_type);
}

unsigned long __stdcall engine_linker::read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	if (!debug_client_)
	{
		return 0;
	}

	unsigned long readn = 0;
	IDebugDataSpaces2 *debug_data_space;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugDataSpaces2), (void **)&debug_data_space) != S_OK)
	{
		return 0;
	}

	if (debug_data_space->ReadVirtual(virtual_address, out_memory, read_size, &readn) != S_OK)
	{
		return 0;
	}

	return readn;
}

unsigned long __stdcall engine_linker::write_virtual_memory(unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size)
{
	if (!debug_client_)
	{
		return false;
	}

	unsigned long written = 0;
	IDebugDataSpaces2 *debug_data_space;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugDataSpaces2), (void **)&debug_data_space) != S_OK)
	{
		return false;
	}

	if (debug_data_space->WriteVirtual(virtual_address, input_memory, write_size, &written) != S_OK)
	{
		return 0;
	}

	return written;
}

bool __stdcall engine_linker::get_thread_context(cpu_context_type *context)
{
	if (!debug_client_)
	{
		return false;
	}

	if (context == nullptr)
	{
		return false;
	}

	unsigned long count = 0;
	IDebugRegisters *debug_register;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugRegisters), (void **)&debug_register) != S_OK)
	{
		return false;
	}

	if (debug_register->GetNumberRegisters(&count) != S_OK)
	{
		return false;
	}

	DEBUG_VALUE debug_value_array[1000];
	memset(debug_value_array, 0, sizeof(debug_value_array));

	if (debug_register->GetValues(count, nullptr, 0, debug_value_array) != S_OK)
	{
		return false;
	}

#ifdef _WIN64
	context->rax = debug_value_array[DBG_X64_REG_IDX_RAX].I64;
	context->rbx = debug_value_array[DBG_X64_REG_IDX_RBX].I64;
	context->rcx = debug_value_array[DBG_X64_REG_IDX_RCX].I64;
	context->rdx = debug_value_array[DBG_X64_REG_IDX_RDX].I64;

	context->rbp = debug_value_array[DBG_X64_REG_IDX_RBP].I64;
	context->rsp = debug_value_array[DBG_X64_REG_IDX_RSP].I64;

	context->rdi = debug_value_array[DBG_X64_REG_IDX_RDI].I64;
	context->rsi = debug_value_array[DBG_X64_REG_IDX_RSI].I64;

	context->r8 = debug_value_array[DBG_X64_REG_IDX_R8].I64;
	context->r9 = debug_value_array[DBG_X64_REG_IDX_R9].I64;
	context->r10 = debug_value_array[DBG_X64_REG_IDX_R10].I64;
	context->r11 = debug_value_array[DBG_X64_REG_IDX_R11].I64;
	context->r12 = debug_value_array[DBG_X64_REG_IDX_R12].I64;
	context->r13 = debug_value_array[DBG_X64_REG_IDX_R13].I64;
	context->r14 = debug_value_array[DBG_X64_REG_IDX_R14].I64;
	context->r15 = debug_value_array[DBG_X64_REG_IDX_R15].I64;

	context->rip = debug_value_array[DBG_X64_REG_IDX_RIP].I64;

	context->efl = (unsigned long)debug_value_array[DBG_X64_REG_IDX_EFL].I64;

	context->cs = (unsigned long)debug_value_array[DBG_X64_REG_IDX_CS].I64;
	context->ds = (unsigned long)debug_value_array[DBG_X64_REG_IDX_DS].I64;
	context->es = (unsigned long)debug_value_array[DBG_X64_REG_IDX_ES].I64;
	context->fs = (unsigned long)debug_value_array[DBG_X64_REG_IDX_FS].I64;
	context->gs = (unsigned long)debug_value_array[DBG_X64_REG_IDX_GS].I64;
	context->ss = (unsigned long)debug_value_array[DBG_X64_REG_IDX_SS].I64;

	context->dr0 = debug_value_array[DBG_X64_REG_IDX_DR0].I64;
	context->dr1 = debug_value_array[DBG_X64_REG_IDX_DR1].I64;
	context->dr2 = debug_value_array[DBG_X64_REG_IDX_DR2].I64;
	context->dr3 = debug_value_array[DBG_X64_REG_IDX_DR3].I64;
	context->dr6 = debug_value_array[DBG_X64_REG_IDX_DR6].I64;
	context->dr7 = debug_value_array[DBG_X64_REG_IDX_DR7].I64;

	context->fpcw = debug_value_array[DBG_X64_REG_IDX_FPCW].I64;
	context->fpsw = debug_value_array[DBG_X64_REG_IDX_FPSW].I64;
	context->fptw = debug_value_array[DBG_X64_REG_IDX_FPTW].I64;

	context->st0 = debug_value_array[DBG_X64_REG_IDX_ST0].I64;
	context->st1 = debug_value_array[DBG_X64_REG_IDX_ST1].I64;
	context->st2 = debug_value_array[DBG_X64_REG_IDX_ST2].I64;
	context->st3 = debug_value_array[DBG_X64_REG_IDX_ST3].I64;
	context->st4 = debug_value_array[DBG_X64_REG_IDX_ST4].I64;
	context->st5 = debug_value_array[DBG_X64_REG_IDX_ST5].I64;
	context->st6 = debug_value_array[DBG_X64_REG_IDX_ST6].I64;
	context->st7 = debug_value_array[DBG_X64_REG_IDX_ST7].I64;

	context->mm0 = debug_value_array[DBG_X64_REG_IDX_MM0].I64;
	context->mm1 = debug_value_array[DBG_X64_REG_IDX_MM1].I64;
	context->mm2 = debug_value_array[DBG_X64_REG_IDX_MM2].I64;
	context->mm3 = debug_value_array[DBG_X64_REG_IDX_MM3].I64;
	context->mm4 = debug_value_array[DBG_X64_REG_IDX_MM4].I64;
	context->mm5 = debug_value_array[DBG_X64_REG_IDX_MM5].I64;
	context->mm6 = debug_value_array[DBG_X64_REG_IDX_MM6].I64;
	context->mm7 = debug_value_array[DBG_X64_REG_IDX_MM7].I64;

	context->mxcsr = debug_value_array[DBG_X64_REG_IDX_MXCSR].I64;

	context->xmm0 = debug_value_array[DBG_X64_REG_IDX_XMM0].I64;
	context->xmm1 = debug_value_array[DBG_X64_REG_IDX_XMM1].I64;
	context->xmm2 = debug_value_array[DBG_X64_REG_IDX_XMM2].I64;
	context->xmm3 = debug_value_array[DBG_X64_REG_IDX_XMM3].I64;
	context->xmm4 = debug_value_array[DBG_X64_REG_IDX_XMM4].I64;
	context->xmm5 = debug_value_array[DBG_X64_REG_IDX_XMM5].I64;
	context->xmm6 = debug_value_array[DBG_X64_REG_IDX_XMM6].I64;
	context->xmm7 = debug_value_array[DBG_X64_REG_IDX_XMM7].I64;
	context->xmm8 = debug_value_array[DBG_X64_REG_IDX_XMM8].I64;
	context->xmm9 = debug_value_array[DBG_X64_REG_IDX_XMM9].I64;
	context->xmm10 = debug_value_array[DBG_X64_REG_IDX_XMM10].I64;
	context->xmm11 = debug_value_array[DBG_X64_REG_IDX_XMM11].I64;
	context->xmm12 = debug_value_array[DBG_X64_REG_IDX_XMM12].I64;
	context->xmm13 = debug_value_array[DBG_X64_REG_IDX_XMM13].I64;
	context->xmm14 = debug_value_array[DBG_X64_REG_IDX_XMM14].I64;
	context->xmm15 = debug_value_array[DBG_X64_REG_IDX_XMM15].I64;

	context->ymm0 = debug_value_array[DBG_X64_REG_IDX_YMM0].I64;
	context->ymm1 = debug_value_array[DBG_X64_REG_IDX_YMM1].I64;
	context->ymm2 = debug_value_array[DBG_X64_REG_IDX_YMM2].I64;
	context->ymm3 = debug_value_array[DBG_X64_REG_IDX_YMM3].I64;
	context->ymm4 = debug_value_array[DBG_X64_REG_IDX_YMM4].I64;
	context->ymm5 = debug_value_array[DBG_X64_REG_IDX_YMM5].I64;
	context->ymm6 = debug_value_array[DBG_X64_REG_IDX_YMM6].I64;
	context->ymm7 = debug_value_array[DBG_X64_REG_IDX_YMM7].I64;
	context->ymm8 = debug_value_array[DBG_X64_REG_IDX_YMM8].I64;
	context->ymm9 = debug_value_array[DBG_X64_REG_IDX_YMM9].I64;
	context->ymm10 = debug_value_array[DBG_X64_REG_IDX_YMM10].I64;
	context->ymm11 = debug_value_array[DBG_X64_REG_IDX_YMM11].I64;
	context->ymm12 = debug_value_array[DBG_X64_REG_IDX_YMM12].I64;
	context->ymm13 = debug_value_array[DBG_X64_REG_IDX_YMM13].I64;
	context->ymm14 = debug_value_array[DBG_X64_REG_IDX_YMM14].I64;
	context->ymm15 = debug_value_array[DBG_X64_REG_IDX_YMM15].I64;

	context->iopl = debug_value_array[DBG_X64_REG_IDX_IOPL].I64;
	context->vip = debug_value_array[DBG_X64_REG_IDX_VIP].I64;
	context->vif = debug_value_array[DBG_X64_REG_IDX_VIF].I64;
#else
	context->rax = debug_value_array[DBG_X86_REG_IDX_EAX].I32;
	context->rbx = debug_value_array[DBG_X86_REG_IDX_EBX].I32;
	context->rcx = debug_value_array[DBG_X86_REG_IDX_ECX].I32;
	context->rdx = debug_value_array[DBG_X86_REG_IDX_EDX].I32;

	context->rbp = debug_value_array[DBG_X86_REG_IDX_EBP].I32;
	context->rsp = debug_value_array[DBG_X86_REG_IDX_ESP].I32;

	context->rdi = debug_value_array[DBG_X86_REG_IDX_EDI].I32;
	context->rsi = debug_value_array[DBG_X86_REG_IDX_ESI].I32;

	context->rip = debug_value_array[DBG_X86_REG_IDX_EIP].I32;

	context->efl = (unsigned long)debug_value_array[DBG_X86_REG_IDX_EFL].I32;

	context->cs = (unsigned long)debug_value_array[DBG_X86_REG_IDX_CS].I32;
	context->ds = (unsigned long)debug_value_array[DBG_X86_REG_IDX_DS].I32;
	context->es = (unsigned long)debug_value_array[DBG_X86_REG_IDX_ES].I32;
	context->fs = (unsigned long)debug_value_array[DBG_X86_REG_IDX_FS].I32;
	context->gs = (unsigned long)debug_value_array[DBG_X86_REG_IDX_GS].I32;
	context->ss = (unsigned long)debug_value_array[DBG_X86_REG_IDX_SS].I32;

	context->dr0 = debug_value_array[DBG_X86_REG_IDX_DR0].I32;
	context->dr1 = debug_value_array[DBG_X86_REG_IDX_DR1].I32;
	context->dr2 = debug_value_array[DBG_X86_REG_IDX_DR2].I32;
	context->dr3 = debug_value_array[DBG_X86_REG_IDX_DR3].I32;
	context->dr6 = debug_value_array[DBG_X86_REG_IDX_DR6].I32;
	context->dr7 = debug_value_array[DBG_X86_REG_IDX_DR7].I32;

	context->fpcw = debug_value_array[DBG_X86_REG_IDX_FPCW].I32;
	context->fpsw = debug_value_array[DBG_X86_REG_IDX_FPSW].I32;
	context->fptw = debug_value_array[DBG_X86_REG_IDX_FPTW].I32;

	context->st0 = debug_value_array[DBG_X86_REG_IDX_ST0].I32;
	context->st1 = debug_value_array[DBG_X86_REG_IDX_ST1].I32;
	context->st2 = debug_value_array[DBG_X86_REG_IDX_ST2].I32;
	context->st3 = debug_value_array[DBG_X86_REG_IDX_ST3].I32;
	context->st4 = debug_value_array[DBG_X86_REG_IDX_ST4].I32;
	context->st5 = debug_value_array[DBG_X86_REG_IDX_ST5].I32;
	context->st6 = debug_value_array[DBG_X86_REG_IDX_ST6].I32;
	context->st7 = debug_value_array[DBG_X86_REG_IDX_ST7].I32;

	context->mm0 = debug_value_array[DBG_X86_REG_IDX_MM0].I32;
	context->mm1 = debug_value_array[DBG_X86_REG_IDX_MM1].I32;
	context->mm2 = debug_value_array[DBG_X86_REG_IDX_MM2].I32;
	context->mm3 = debug_value_array[DBG_X86_REG_IDX_MM3].I32;
	context->mm4 = debug_value_array[DBG_X86_REG_IDX_MM4].I32;
	context->mm5 = debug_value_array[DBG_X86_REG_IDX_MM5].I32;
	context->mm6 = debug_value_array[DBG_X86_REG_IDX_MM6].I32;
	context->mm7 = debug_value_array[DBG_X86_REG_IDX_MM7].I32;

	context->mxcsr = debug_value_array[DBG_X86_REG_IDX_MXCSR].I32;

	context->xmm0 = debug_value_array[DBG_X86_REG_IDX_XMM0].I32;
	context->xmm1 = debug_value_array[DBG_X86_REG_IDX_XMM1].I32;
	context->xmm2 = debug_value_array[DBG_X86_REG_IDX_XMM2].I32;
	context->xmm3 = debug_value_array[DBG_X86_REG_IDX_XMM3].I32;
	context->xmm4 = debug_value_array[DBG_X86_REG_IDX_XMM4].I32;
	context->xmm5 = debug_value_array[DBG_X86_REG_IDX_XMM5].I32;
	context->xmm6 = debug_value_array[DBG_X86_REG_IDX_XMM6].I32;
	context->xmm7 = debug_value_array[DBG_X86_REG_IDX_XMM7].I32;

	context->ymm0 = debug_value_array[DBG_X86_REG_IDX_YMM0].I32;
	context->ymm1 = debug_value_array[DBG_X86_REG_IDX_YMM1].I32;
	context->ymm2 = debug_value_array[DBG_X86_REG_IDX_YMM2].I32;
	context->ymm3 = debug_value_array[DBG_X86_REG_IDX_YMM3].I32;
	context->ymm4 = debug_value_array[DBG_X86_REG_IDX_YMM4].I32;
	context->ymm5 = debug_value_array[DBG_X86_REG_IDX_YMM5].I32;
	context->ymm6 = debug_value_array[DBG_X86_REG_IDX_YMM6].I32;
	context->ymm7 = debug_value_array[DBG_X86_REG_IDX_YMM7].I32;

	context->iopl = debug_value_array[DBG_X86_REG_IDX_IOPL].I32;
	context->vip = debug_value_array[DBG_X86_REG_IDX_VIP].I32;
	context->vif = debug_value_array[DBG_X86_REG_IDX_VIF].I32;
#endif

	return true;
}

bool __stdcall engine_linker::set_thread_context(cpu_context_type *context)
{
	if (!debug_client_)
	{
		return false;
	}

	if (context == nullptr)
	{
		return false;
	}

	unsigned long count = 0;

	IDebugRegisters *debug_register;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugRegisters), (void **)&debug_register) != S_OK)
	{
		return false;
	}

	if (debug_register->GetNumberRegisters(&count) != S_OK)
	{
		return false;
	}

	DEBUG_VALUE debug_value_array[1000];
	memset(debug_value_array, 0, sizeof(debug_value_array));

#ifdef _WIN64
	debug_value_array[DBG_X64_REG_IDX_RAX].I64 = context->rax;
	debug_value_array[DBG_X64_REG_IDX_RBX].I64 = context->rbx;
	debug_value_array[DBG_X64_REG_IDX_RCX].I64 = context->rcx;
	debug_value_array[DBG_X64_REG_IDX_RDX].I64 = context->rdx;

	debug_value_array[DBG_X64_REG_IDX_RBP].I64 = context->rbp;
	debug_value_array[DBG_X64_REG_IDX_RSP].I64 = context->rsp;

	debug_value_array[DBG_X64_REG_IDX_RDI].I64 = context->rdi;
	debug_value_array[DBG_X64_REG_IDX_RSI].I64 = context->rsi;

	debug_value_array[DBG_X64_REG_IDX_R8].I64 = context->r8;
	debug_value_array[DBG_X64_REG_IDX_R9].I64 = context->r9;
	debug_value_array[DBG_X64_REG_IDX_R10].I64 = context->r10;
	debug_value_array[DBG_X64_REG_IDX_R11].I64 = context->r11;
	debug_value_array[DBG_X64_REG_IDX_R12].I64 = context->r12;
	debug_value_array[DBG_X64_REG_IDX_R13].I64 = context->r13;
	debug_value_array[DBG_X64_REG_IDX_R14].I64 = context->r14;
	debug_value_array[DBG_X64_REG_IDX_R15].I64 = context->r15;

	debug_value_array[DBG_X64_REG_IDX_RIP].I64 = context->rip;

	debug_value_array[DBG_X64_REG_IDX_EFL].I64 = context->efl;

	debug_value_array[DBG_X64_REG_IDX_CS].I64 = context->cs;
	debug_value_array[DBG_X64_REG_IDX_DS].I64 = context->ds;
	debug_value_array[DBG_X64_REG_IDX_ES].I64 = context->es;
	debug_value_array[DBG_X64_REG_IDX_FS].I64 = context->fs;
	debug_value_array[DBG_X64_REG_IDX_GS].I64 = context->gs;
	debug_value_array[DBG_X64_REG_IDX_SS].I64 = context->ss;

	debug_value_array[DBG_X64_REG_IDX_DR0].I64 = context->dr0;
	debug_value_array[DBG_X64_REG_IDX_DR1].I64 = context->dr1;
	debug_value_array[DBG_X64_REG_IDX_DR2].I64 = context->dr2;
	debug_value_array[DBG_X64_REG_IDX_DR3].I64 = context->dr3;
	debug_value_array[DBG_X64_REG_IDX_DR6].I64 = context->dr6;
	debug_value_array[DBG_X64_REG_IDX_DR7].I64 = context->dr7;

	debug_value_array[DBG_X64_REG_IDX_FPCW].I64 = context->fpcw;
	debug_value_array[DBG_X64_REG_IDX_FPSW].I64 = context->fpsw;
	debug_value_array[DBG_X64_REG_IDX_FPTW].I64 = context->fptw;

	debug_value_array[DBG_X64_REG_IDX_ST0].I64 = context->st0;
	debug_value_array[DBG_X64_REG_IDX_ST1].I64 = context->st1;
	debug_value_array[DBG_X64_REG_IDX_ST2].I64 = context->st2;
	debug_value_array[DBG_X64_REG_IDX_ST3].I64 = context->st3;
	debug_value_array[DBG_X64_REG_IDX_ST4].I64 = context->st4;
	debug_value_array[DBG_X64_REG_IDX_ST5].I64 = context->st5;
	debug_value_array[DBG_X64_REG_IDX_ST6].I64 = context->st6;
	debug_value_array[DBG_X64_REG_IDX_ST7].I64 = context->st7;

	debug_value_array[DBG_X64_REG_IDX_MM0].I64 = context->mm0;
	debug_value_array[DBG_X64_REG_IDX_MM1].I64 = context->mm1;
	debug_value_array[DBG_X64_REG_IDX_MM2].I64 = context->mm2;
	debug_value_array[DBG_X64_REG_IDX_MM3].I64 = context->mm3;
	debug_value_array[DBG_X64_REG_IDX_MM4].I64 = context->mm4;
	debug_value_array[DBG_X64_REG_IDX_MM5].I64 = context->mm5;
	debug_value_array[DBG_X64_REG_IDX_MM6].I64 = context->mm6;
	debug_value_array[DBG_X64_REG_IDX_MM7].I64 = context->mm7;

	debug_value_array[DBG_X64_REG_IDX_MXCSR].I64 = context->mxcsr;

	debug_value_array[DBG_X64_REG_IDX_XMM0].I64 = context->xmm0;
	debug_value_array[DBG_X64_REG_IDX_XMM1].I64 = context->xmm1;
	debug_value_array[DBG_X64_REG_IDX_XMM2].I64 = context->xmm2;
	debug_value_array[DBG_X64_REG_IDX_XMM3].I64 = context->xmm3;
	debug_value_array[DBG_X64_REG_IDX_XMM4].I64 = context->xmm4;
	debug_value_array[DBG_X64_REG_IDX_XMM5].I64 = context->xmm5;
	debug_value_array[DBG_X64_REG_IDX_XMM6].I64 = context->xmm6;
	debug_value_array[DBG_X64_REG_IDX_XMM7].I64 = context->xmm7;
	debug_value_array[DBG_X64_REG_IDX_XMM8].I64 = context->xmm8;
	debug_value_array[DBG_X64_REG_IDX_XMM9].I64 = context->xmm9;
	debug_value_array[DBG_X64_REG_IDX_XMM10].I64 = context->xmm10;
	debug_value_array[DBG_X64_REG_IDX_XMM11].I64 = context->xmm11;
	debug_value_array[DBG_X64_REG_IDX_XMM12].I64 = context->xmm12;
	debug_value_array[DBG_X64_REG_IDX_XMM13].I64 = context->xmm13;
	debug_value_array[DBG_X64_REG_IDX_XMM14].I64 = context->xmm14;
	debug_value_array[DBG_X64_REG_IDX_XMM15].I64 = context->xmm15;

	debug_value_array[DBG_X64_REG_IDX_YMM0].I64 = context->ymm0;
	debug_value_array[DBG_X64_REG_IDX_YMM1].I64 = context->ymm1;
	debug_value_array[DBG_X64_REG_IDX_YMM2].I64 = context->ymm2;
	debug_value_array[DBG_X64_REG_IDX_YMM3].I64 = context->ymm3;
	debug_value_array[DBG_X64_REG_IDX_YMM4].I64 = context->ymm4;
	debug_value_array[DBG_X64_REG_IDX_YMM5].I64 = context->ymm5;
	debug_value_array[DBG_X64_REG_IDX_YMM6].I64 = context->ymm6;
	debug_value_array[DBG_X64_REG_IDX_YMM7].I64 = context->ymm7;
	debug_value_array[DBG_X64_REG_IDX_YMM8].I64 = context->ymm8;
	debug_value_array[DBG_X64_REG_IDX_YMM9].I64 = context->ymm9;
	debug_value_array[DBG_X64_REG_IDX_YMM10].I64 = context->ymm10;
	debug_value_array[DBG_X64_REG_IDX_YMM11].I64 = context->ymm11;
	debug_value_array[DBG_X64_REG_IDX_YMM12].I64 = context->ymm12;
	debug_value_array[DBG_X64_REG_IDX_YMM13].I64 = context->ymm13;
	debug_value_array[DBG_X64_REG_IDX_YMM14].I64 = context->ymm14;
	debug_value_array[DBG_X64_REG_IDX_YMM15].I64 = context->ymm15;

	debug_value_array[DBG_X64_REG_IDX_IOPL].I64 = context->iopl;
	debug_value_array[DBG_X64_REG_IDX_VIP].I64 = context->vip;
	debug_value_array[DBG_X64_REG_IDX_VIF].I64 = context->vif;
#else
	debug_value_array[DBG_X86_REG_IDX_EAX].I64 = context->rax;
	debug_value_array[DBG_X86_REG_IDX_EBX].I64 = context->rbx;
	debug_value_array[DBG_X86_REG_IDX_ECX].I64 = context->rcx;
	debug_value_array[DBG_X86_REG_IDX_EDX].I64 = context->rdx;

	debug_value_array[DBG_X86_REG_IDX_EBP].I64 = context->rbp;
	debug_value_array[DBG_X86_REG_IDX_ESP].I64 = context->rsp;

	debug_value_array[DBG_X86_REG_IDX_EDI].I64 = context->rdi;
	debug_value_array[DBG_X86_REG_IDX_ESI].I64 = context->rsi;

	debug_value_array[DBG_X86_REG_IDX_EIP].I64 = context->rip;

	debug_value_array[DBG_X86_REG_IDX_EFL].I32 = context->efl;

	debug_value_array[DBG_X86_REG_IDX_CS].I32 = context->cs;
	debug_value_array[DBG_X86_REG_IDX_DS].I32 = context->ds;
	debug_value_array[DBG_X86_REG_IDX_ES].I32 = context->es;
	debug_value_array[DBG_X86_REG_IDX_FS].I32 = context->fs;
	debug_value_array[DBG_X86_REG_IDX_GS].I32 = context->gs;
	debug_value_array[DBG_X86_REG_IDX_SS].I32 = context->ss;

	debug_value_array[DBG_X86_REG_IDX_DR0].I64 = context->dr0;
	debug_value_array[DBG_X86_REG_IDX_DR1].I64 = context->dr1;
	debug_value_array[DBG_X86_REG_IDX_DR2].I64 = context->dr2;
	debug_value_array[DBG_X86_REG_IDX_DR3].I64 = context->dr3;
	debug_value_array[DBG_X86_REG_IDX_DR6].I64 = context->dr6;
	debug_value_array[DBG_X86_REG_IDX_DR7].I64 = context->dr7;

	debug_value_array[DBG_X86_REG_IDX_FPCW].I64 = context->fpcw;
	debug_value_array[DBG_X86_REG_IDX_FPSW].I64 = context->fpsw;
	debug_value_array[DBG_X86_REG_IDX_FPTW].I64 = context->fptw;

	debug_value_array[DBG_X86_REG_IDX_ST0].I64 = context->st0;
	debug_value_array[DBG_X86_REG_IDX_ST1].I64 = context->st1;
	debug_value_array[DBG_X86_REG_IDX_ST2].I64 = context->st2;
	debug_value_array[DBG_X86_REG_IDX_ST3].I64 = context->st3;
	debug_value_array[DBG_X86_REG_IDX_ST4].I64 = context->st4;
	debug_value_array[DBG_X86_REG_IDX_ST5].I64 = context->st5;
	debug_value_array[DBG_X86_REG_IDX_ST6].I64 = context->st6;
	debug_value_array[DBG_X86_REG_IDX_ST7].I64 = context->st7;

	debug_value_array[DBG_X86_REG_IDX_MM0].I64 = context->mm0;
	debug_value_array[DBG_X86_REG_IDX_MM1].I64 = context->mm1;
	debug_value_array[DBG_X86_REG_IDX_MM2].I64 = context->mm2;
	debug_value_array[DBG_X86_REG_IDX_MM3].I64 = context->mm3;
	debug_value_array[DBG_X86_REG_IDX_MM4].I64 = context->mm4;
	debug_value_array[DBG_X86_REG_IDX_MM5].I64 = context->mm5;
	debug_value_array[DBG_X86_REG_IDX_MM6].I64 = context->mm6;
	debug_value_array[DBG_X86_REG_IDX_MM7].I64 = context->mm7;

	debug_value_array[DBG_X86_REG_IDX_MXCSR].I64 = context->mxcsr;

	debug_value_array[DBG_X86_REG_IDX_XMM0].I64 = context->xmm0;
	debug_value_array[DBG_X86_REG_IDX_XMM1].I64 = context->xmm1;
	debug_value_array[DBG_X86_REG_IDX_XMM2].I64 = context->xmm2;
	debug_value_array[DBG_X86_REG_IDX_XMM3].I64 = context->xmm3;
	debug_value_array[DBG_X86_REG_IDX_XMM4].I64 = context->xmm4;
	debug_value_array[DBG_X86_REG_IDX_XMM5].I64 = context->xmm5;
	debug_value_array[DBG_X86_REG_IDX_XMM6].I64 = context->xmm6;
	debug_value_array[DBG_X86_REG_IDX_XMM7].I64 = context->xmm7;

	debug_value_array[DBG_X86_REG_IDX_YMM0].I64 = context->ymm0;
	debug_value_array[DBG_X86_REG_IDX_YMM1].I64 = context->ymm1;
	debug_value_array[DBG_X86_REG_IDX_YMM2].I64 = context->ymm2;
	debug_value_array[DBG_X86_REG_IDX_YMM3].I64 = context->ymm3;
	debug_value_array[DBG_X86_REG_IDX_YMM4].I64 = context->ymm4;
	debug_value_array[DBG_X86_REG_IDX_YMM5].I64 = context->ymm5;
	debug_value_array[DBG_X86_REG_IDX_YMM6].I64 = context->ymm6;
	debug_value_array[DBG_X86_REG_IDX_YMM7].I64 = context->ymm7;

	debug_value_array[DBG_X86_REG_IDX_IOPL].I64 = context->iopl;
	debug_value_array[DBG_X86_REG_IDX_VIP].I64 = context->vip;
	debug_value_array[DBG_X86_REG_IDX_VIF].I64 = context->vif;
#endif

	if (debug_register->SetValues(count, nullptr, 0, debug_value_array) != S_OK)
	{
		return false;
	}

	return true;
}

bool engine_linker::get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	if (!debug_client_)
	{
		return false;
	}

	IDebugSymbols *debug_symbol;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugSymbols), (void **)&debug_symbol) != S_OK)
	{
		return false;
	}

	if (debug_symbol->GetNameByOffset(offset, buffer, size_of_buffer, size_of_name, disp) != S_OK)
	{
		return false;
	}

	return true;
}

bool engine_linker::stack_trace(unsigned long tid, stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count)
{
	if (!debug_client_)
	{
		return false;
	}

	if (!select_thread(tid))
	{
		return false;
	}

	IDebugControl3 *debug_control;
	if (((IDebugClient *)debug_client_)->QueryInterface(__uuidof(IDebugControl3), (void **)&debug_control) != S_OK)
	{
		return false;
	}

	unsigned long type = 0;
	if (debug_control->GetEffectiveProcessorType(&type) != S_OK)
	{
		return false;
	}

	if (stack_frame->type == dbg::PROCESSOR_TYPE::X86_PROCESSOR_BIT_TYPE)
	{
		debug_control->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_I386);
	}
	else if (stack_frame->type == dbg::PROCESSOR_TYPE::X64_PROCESSOR_BIT_TYPE)
	{
		debug_control->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_AMD64);
	}
	else
	{
		return false;
	}

	unsigned long cnt = size_of_stack_frame / sizeof(stack_frame_type);
	PDEBUG_STACK_FRAME debug_stack_frame = (PDEBUG_STACK_FRAME)malloc(sizeof(DEBUG_STACK_FRAME) * cnt);
	if (!debug_stack_frame)
	{
		return false;
	}
	memset(debug_stack_frame, 0, sizeof(DEBUG_STACK_FRAME) * cnt);

	if (debug_control->GetStackTrace(0, 0, 0, debug_stack_frame, sizeof(DEBUG_STACK_FRAME) * cnt, stack_count) != S_OK)
	{
		return false;
	}

	for (unsigned long i = 0; i < *stack_count; ++i)
	{
		stack_frame[i].frame_number = debug_stack_frame[i].FrameNumber;
		stack_frame[i].frame_offset = debug_stack_frame[i].FrameOffset;
		stack_frame[i].func_table_entry = debug_stack_frame[i].FuncTableEntry;
		stack_frame[i].instruction_offset = debug_stack_frame[i].InstructionOffset;
		stack_frame[i].return_offset = debug_stack_frame[i].ReturnOffset;
		stack_frame[i].stack_offset = debug_stack_frame[i].StackOffset;
		stack_frame[i].bool_virtual = debug_stack_frame[i].Virtual;

		for (int j = 0; j < 4; ++j)
		{
			stack_frame[i].params[j] = debug_stack_frame[i].Params[j];
		}

		for (int j = 0; j < 6; ++j)
		{
			stack_frame[i].reserved[j] = debug_stack_frame[i].Reserved[j];
		}
	}

	free(debug_stack_frame);

	return true;
}

bool engine_linker::write_dump(char *path)
{
	if (!debug_client_)
	{
		return false;
	}

	unsigned long flag = DEBUG_DUMP_DEFAULT;
	if (is_kernel_mode_debugging())
	{
		flag = DEBUG_DUMP_FULL;
	}

	if (((IDebugClient *)debug_client_)->WriteDumpFile(path, flag) != S_OK)
	{
		return false;
	}

	return true;
}

bool engine_linker::suspend(unsigned long tid)
{
	if (tid == 0)
	{
		std::list<unsigned long> tid_list;
		if (!get_thread_id_list(tid_list))
		{
			return false;
		}
		std::list<unsigned long>::iterator tid_list_it = tid_list.begin();
		for (tid_list_it; tid_list_it != tid_list.end(); ++tid_list_it)
		{
			HANDLE thread_handle = OpenThread(MAXIMUM_ALLOWED, FALSE, *tid_list_it);
			if (!thread_handle)
			{
				continue;
			}
			std::shared_ptr<void> handle_closer(thread_handle, CloseHandle);
			unsigned long cnt = SuspendThread(thread_handle);
			if (cnt == 0)
			{
				return false;
			}
		}
	}
	else
	{
		HANDLE thread_handle = OpenThread(MAXIMUM_ALLOWED, FALSE, tid);
		if (!thread_handle)
		{
			return false;
		}
		std::shared_ptr<void> handle_closer(thread_handle, CloseHandle);
		unsigned long cnt = SuspendThread(thread_handle);
		if (cnt == 0)
		{
			return false;
		}
	}

	return true;
}

bool engine_linker::resume(unsigned long tid)
{
	if (tid == 0)
	{
		std::list<unsigned long> tid_list;
		if (!get_thread_id_list(tid_list))
		{
			return false;
		}
		std::list<unsigned long>::iterator tid_list_it = tid_list.begin();
		for (tid_list_it; tid_list_it != tid_list.end(); ++tid_list_it)
		{
			HANDLE thread_handle = OpenThread(MAXIMUM_ALLOWED, FALSE, *tid_list_it);
			if (!thread_handle)
			{
				continue;
			}
			std::shared_ptr<void> handle_closer(thread_handle, CloseHandle);

			unsigned long cnt = 0;
			do
			{
				cnt = ResumeThread(thread_handle);
			} while (cnt);
		}
	}
	else
	{
		HANDLE thread_handle = OpenThread(MAXIMUM_ALLOWED, FALSE, tid);
		if (!thread_handle)
		{
			return false;
		}
		std::shared_ptr<void> handle_closer(thread_handle, CloseHandle);

		unsigned long cnt = 0;
		do
		{
			cnt = ResumeThread(thread_handle);
		} while (cnt);
	}

	return true;
}