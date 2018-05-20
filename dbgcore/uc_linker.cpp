#include <uc_linker.h>

#ifdef _WIN64
#pragma comment(lib, "unicorn_static_x64.lib")
#else
#pragma comment(lib, "unicorn_static.lib")
#endif

uc_linker::uc_linker()
{
	uc_ = nullptr;
}

uc_linker::~uc_linker()
{
	uc_close(uc_);
}

void uc_linker::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(uc_linker);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;

	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

void *uc_linker::get_object(unsigned long id)
{
	void *i = nullptr;
	switch (id)
	{
	case EMULATOR_HANDLE_ID :
		i = (void *)uc_;

		break;
	default:
		break;
	}

	return i;
}

unsigned long long __stdcall uc_linker::alignment(unsigned long long region_size, unsigned long image_aligin)
{
	unsigned long mod = region_size % image_aligin;
	region_size -= mod;

	return region_size + image_aligin;
}

void uc_linker::set_global_descriptor(SegmentDescriptor *desc, uint32_t base, uint32_t limit, uint8_t is_code)
{
	desc->descriptor = 0;
	desc->base_low = base & 0xffff;
	desc->base_mid = (base >> 16) & 0xff;
	desc->base_hi = base >> 24;

	if (limit > 0xfffff)
	{
		limit >>= 12;
		desc->granularity = 1;
	}
	desc->limit_low = limit & 0xffff;
	desc->limit_hi = limit >> 16;

	desc->dpl = 3;
	desc->present = 1;
	desc->db = 1;
	desc->type = is_code ? 0xb : 3;
	desc->system = 1;
}

bool __stdcall uc_linker::create_global_descriptor_table(cpu_context_type context, unsigned long mode)
{
	SegmentDescriptor global_descriptor[31];
	memset(global_descriptor, 0, sizeof(global_descriptor));

	//context.ss = 0x88; // rpl = 0
	//context.gs = 0x63;

	set_global_descriptor(&global_descriptor[0x33 >> 3], 0, 0xfffff000, 1); // 64 code
	set_global_descriptor(&global_descriptor[context.cs >> 3], 0, 0xfffff000, 1);
	set_global_descriptor(&global_descriptor[context.ds >> 3], 0, 0xfffff000, 0);
	set_global_descriptor(&global_descriptor[context.fs >> 3], 0, 0xfff, 0);
	set_global_descriptor(&global_descriptor[context.gs >> 3], 0, 0xfffff000, 0);
	set_global_descriptor(&global_descriptor[context.ss >> 3], 0, 0xfffff000, 0);
	global_descriptor[context.ss >> 3].dpl = 0; // dpl = 0, cpl = 0

	unsigned long long gdt_base = 0xc0000000;
	uc_x86_mmr gdtr;
	gdtr.base = gdt_base;
	gdtr.limit = (sizeof(SegmentDescriptor) * 31) - 1;

	if (uc_reg_write(uc_, UC_X86_REG_GDTR, &gdtr) != 0)
		return false;

	if (uc_mem_map(uc_, gdt_base, (size_t)0x10000, UC_PROT_ALL) == 0)
	{
		if (uc_mem_write(uc_, gdt_base, global_descriptor, sizeof(global_descriptor)) == 0)
		{
			if (mode == UC_MODE_32)
			{
				write_x86_cpu_context(context);
			}
			else
			{
				write_x64_cpu_context(context);
			}
		}
	}

	return true;
}

bool __stdcall uc_linker::read_x86_cpu_context(cpu_context_type *context)
{
	int x86_register[] = { UC_X86_REGISTER_SET };
	int size = sizeof(x86_register) / sizeof(int);
	unsigned long *read_register = nullptr;
	void **read_ptr = nullptr;

	read_register = (unsigned long *)malloc(sizeof(unsigned long)*size);
	if (!read_register)
		return false;
	std::shared_ptr<void> read_register_closer(read_register, free);
	memset(read_register, 0, sizeof(unsigned long)*size);

	read_ptr = (void **)malloc(sizeof(void **)*size);
	if (!read_ptr)
		return false;

	std::shared_ptr<void> read_ptr_closer(read_ptr, free);
	memset(read_ptr, 0, sizeof(void **)*size);

	for (int i = 0; i < size; ++i)
		read_ptr[i] = &read_register[i];

	if (uc_reg_read_batch(uc_, x86_register, read_ptr, size) != 0)
		return false;

	context->rax = read_register[PR_RAX];
	context->rbx = read_register[PR_RBX];
	context->rcx = read_register[PR_RCX];
	context->rdx = read_register[PR_RDX];
	context->rsi = read_register[PR_RSI];
	context->rdi = read_register[PR_RDI];
	context->rsp = read_register[PR_RSP];
	context->rbp = read_register[PR_RBP];
	context->rip = read_register[PR_RIP];

	context->xmm0 = read_register[PR_XMM0];
	context->xmm1 = read_register[PR_XMM1];
	context->xmm2 = read_register[PR_XMM2];
	context->xmm3 = read_register[PR_XMM3];
	context->xmm4 = read_register[PR_XMM4];
	context->xmm5 = read_register[PR_XMM5];
	context->xmm6 = read_register[PR_XMM6];
	context->xmm7 = read_register[PR_XMM7];

	context->ymm0 = read_register[PR_YMM0];
	context->ymm1 = read_register[PR_YMM1];
	context->ymm2 = read_register[PR_YMM2];
	context->ymm3 = read_register[PR_YMM3];
	context->ymm4 = read_register[PR_YMM4];
	context->ymm5 = read_register[PR_YMM5];
	context->ymm6 = read_register[PR_YMM6];
	context->ymm7 = read_register[PR_YMM7];

	context->efl = read_register[PR_EFLAGS];
	context->cs = (unsigned short)read_register[PR_REG_CS];
	context->ds = (unsigned short)read_register[PR_REG_DS];
	context->es = (unsigned short)read_register[PR_REG_ES];
	context->fs = (unsigned short)read_register[PR_REG_FS];
	context->gs = (unsigned short)read_register[PR_REG_GS];
	context->ss = (unsigned short)read_register[PR_REG_SS];

	return true;
}

bool __stdcall uc_linker::write_x86_cpu_context(cpu_context_type context)
{
	int x86_register[] = { UC_X86_REGISTER_SET };
	int size = sizeof(x86_register) / sizeof(int);
	unsigned long *write_register = nullptr;
	void **write_ptr = nullptr;

	write_register = (unsigned long *)malloc(sizeof(unsigned long)*size);
	if (!write_register)
		return false;
	std::shared_ptr<void> write_register_closer(write_register, free);
	memset(write_register, 0, sizeof(unsigned long)*size);

	write_ptr = (void **)malloc(sizeof(void **)*size);
	if (!write_ptr)
		return false;
	std::shared_ptr<void> write_ptr_closer(write_ptr, free);
	memset(write_ptr, 0, sizeof(void **)*size);

	for (int i = 0; i < size; ++i)
		write_ptr[i] = &write_register[i];

	write_register[PR_RAX] = (unsigned long)context.rax;
	write_register[PR_RBX] = (unsigned long)context.rbx;
	write_register[PR_RCX] = (unsigned long)context.rcx;
	write_register[PR_RDX] = (unsigned long)context.rdx;
	write_register[PR_RSI] = (unsigned long)context.rsi;
	write_register[PR_RDI] = (unsigned long)context.rdi;
	write_register[PR_RSP] = (unsigned long)context.rsp;
	write_register[PR_RBP] = (unsigned long)context.rbp;
	write_register[PR_RIP] = (unsigned long)context.rip;
	write_register[PR_EFLAGS] = (unsigned long)context.efl;

	write_register[PR_XMM0] = (unsigned long)context.xmm0;
	write_register[PR_XMM1] = (unsigned long)context.xmm1;
	write_register[PR_XMM2] = (unsigned long)context.xmm2;
	write_register[PR_XMM3] = (unsigned long)context.xmm3;
	write_register[PR_XMM4] = (unsigned long)context.xmm4;
	write_register[PR_XMM5] = (unsigned long)context.xmm5;
	write_register[PR_XMM6] = (unsigned long)context.xmm6;
	write_register[PR_XMM7] = (unsigned long)context.xmm7;

	write_register[PR_YMM0] = (unsigned long)context.ymm0;
	write_register[PR_YMM1] = (unsigned long)context.ymm1;
	write_register[PR_YMM2] = (unsigned long)context.ymm2;
	write_register[PR_YMM3] = (unsigned long)context.ymm3;
	write_register[PR_YMM4] = (unsigned long)context.ymm4;
	write_register[PR_YMM5] = (unsigned long)context.ymm5;
	write_register[PR_YMM6] = (unsigned long)context.ymm6;
	write_register[PR_YMM7] = (unsigned long)context.ymm7;

	write_register[PR_REG_CS] = context.cs;
	write_register[PR_REG_DS] = context.ds;
	write_register[PR_REG_ES] = context.es;
	write_register[PR_REG_FS] = context.fs;
	write_register[PR_REG_GS] = context.gs;
	write_register[PR_REG_SS] = context.ss;

	if (uc_reg_write_batch(uc_, x86_register, write_ptr, size) != 0)
		return false;

	return true;
}

bool __stdcall uc_linker::write_x64_cpu_context(cpu_context_type context)
{
	int x86_register[] = { UC_X64_REGISTER_SET };
	int size = sizeof(x86_register) / sizeof(int);
	unsigned long long *write_register = nullptr;
	void **write_ptr = nullptr;

	write_register = (unsigned long long *)malloc(sizeof(unsigned long long)*size);
	if (!write_register)
		return false;
	std::shared_ptr<void> write_register_closer(write_register, free);
	memset(write_register, 0, sizeof(unsigned long long)*size);

	write_ptr = (void **)malloc(sizeof(void **)*size);
	if (!write_ptr)
		return false;
	std::shared_ptr<void> write_ptr_closer(write_ptr, free);
	memset(write_ptr, 0, sizeof(void **)*size);

	for (int i = 0; i < size; ++i)
		write_ptr[i] = &write_register[i];

	write_register[PR_RAX] = context.rax;
	write_register[PR_RBX] = context.rbx;
	write_register[PR_RCX] = context.rcx;
	write_register[PR_RDX] = context.rdx;
	write_register[PR_RSI] = context.rsi;
	write_register[PR_RDI] = context.rdi;
	write_register[PR_RSP] = context.rsp;
	write_register[PR_RBP] = context.rbp;
	write_register[PR_RIP] = context.rip;
	write_register[PR_R8] = context.r8;
	write_register[PR_R9] = context.r9;
	write_register[PR_R10] = context.r10;
	write_register[PR_R11] = context.r11;
	write_register[PR_R12] = context.r12;
	write_register[PR_R13] = context.r13;
	write_register[PR_R14] = context.r14;
	write_register[PR_R15] = context.r15;
	write_register[PR_EFLAGS] = (unsigned long)context.efl;

	write_register[PR_XMM0] = context.xmm0;
	write_register[PR_XMM1] = context.xmm1;
	write_register[PR_XMM2] = context.xmm2;
	write_register[PR_XMM3] = context.xmm3;
	write_register[PR_XMM4] = context.xmm4;
	write_register[PR_XMM5] = context.xmm5;
	write_register[PR_XMM6] = context.xmm6;
	write_register[PR_XMM7] = context.xmm7;
	write_register[PR_XMM8] = context.xmm8;
	write_register[PR_XMM9] = context.xmm9;
	write_register[PR_XMM10] = context.xmm10;
	write_register[PR_XMM11] = context.xmm11;
	write_register[PR_XMM12] = context.xmm12;
	write_register[PR_XMM13] = context.xmm13;
	write_register[PR_XMM14] = context.xmm14;
	write_register[PR_XMM15] = context.xmm15;

	write_register[PR_YMM0] = context.ymm0;
	write_register[PR_YMM1] = context.ymm1;
	write_register[PR_YMM2] = context.ymm2;
	write_register[PR_YMM3] = context.ymm3;
	write_register[PR_YMM4] = context.ymm4;
	write_register[PR_YMM5] = context.ymm5;
	write_register[PR_YMM6] = context.ymm6;
	write_register[PR_YMM7] = context.ymm7;
	write_register[PR_YMM8] = context.ymm8;
	write_register[PR_YMM9] = context.ymm9;
	write_register[PR_YMM10] = context.ymm10;
	write_register[PR_YMM11] = context.ymm11;
	write_register[PR_YMM12] = context.ymm12;
	write_register[PR_YMM13] = context.ymm13;
	write_register[PR_YMM14] = context.ymm14;
	write_register[PR_YMM15] = context.ymm15;

	write_register[PR_REG_CS] = context.cs;
	write_register[PR_REG_DS] = context.ds;
	write_register[PR_REG_ES] = context.es;
	write_register[PR_REG_FS] = context.fs;
	write_register[PR_REG_GS] = context.gs;
	write_register[PR_REG_SS] = context.ss;

	if (uc_reg_write_batch(uc_, x86_register, write_ptr, size) != 0)
		return false;

	return true;
}

bool __stdcall uc_linker::read_x64_cpu_context(cpu_context_type *context)
{
	int x86_register[] = { UC_X64_REGISTER_SET };
	int size = sizeof(x86_register) / sizeof(int);
	unsigned long long *read_register = nullptr;
	void **read_ptr = nullptr;

	read_register = (unsigned long long *)malloc(sizeof(unsigned long long)*size);
	if (!read_register)
		return false;
	std::shared_ptr<void> read_register_closer(read_register, free);
	memset(read_register, 0, sizeof(unsigned long long)*size);

	read_ptr = (void **)malloc(sizeof(void **)*size);
	if (!read_ptr)
		return false;
	std::shared_ptr<void> read_ptr_closer(read_ptr, free);
	memset(read_ptr, 0, sizeof(void **)*size);

	for (int i = 0; i < size; ++i)
		read_ptr[i] = &read_register[i];

	if (uc_reg_read_batch(uc_, x86_register, read_ptr, size) != 0)
		return false;

	context->rax = read_register[PR_RAX];
	context->rbx = read_register[PR_RBX];
	context->rcx = read_register[PR_RCX];
	context->rdx = read_register[PR_RDX];
	context->rsi = read_register[PR_RSI];
	context->rdi = read_register[PR_RDI];
	context->rsp = read_register[PR_RSP];
	context->rbp = read_register[PR_RBP];
	context->rip = read_register[PR_RIP];
	context->r8 = read_register[PR_R8];
	context->r9 = read_register[PR_R9];
	context->r10 = read_register[PR_R10];
	context->r11 = read_register[PR_R11];
	context->r12 = read_register[PR_R12];
	context->r13 = read_register[PR_R13];
	context->r14 = read_register[PR_R14];
	context->r15 = read_register[PR_R15];
	context->efl = (unsigned long)read_register[PR_EFLAGS];

	context->xmm0 = read_register[PR_XMM0];
	context->xmm1 = read_register[PR_XMM1];
	context->xmm2 = read_register[PR_XMM2];
	context->xmm3 = read_register[PR_XMM3];
	context->xmm4 = read_register[PR_XMM4];
	context->xmm5 = read_register[PR_XMM5];
	context->xmm6 = read_register[PR_XMM6];
	context->xmm7 = read_register[PR_XMM7];
	context->xmm8 = read_register[PR_XMM8];
	context->xmm9 = read_register[PR_XMM9];
	context->xmm10 = read_register[PR_XMM10];
	context->xmm11 = read_register[PR_XMM11];
	context->xmm12 = read_register[PR_XMM12];
	context->xmm13 = read_register[PR_XMM13];
	context->xmm14 = read_register[PR_XMM14];
	context->xmm15 = read_register[PR_XMM15];

	context->ymm0 = read_register[PR_YMM0];
	context->ymm1 = read_register[PR_YMM1];
	context->ymm2 = read_register[PR_YMM2];
	context->ymm3 = read_register[PR_YMM3];
	context->ymm4 = read_register[PR_YMM4];
	context->ymm5 = read_register[PR_YMM5];
	context->ymm6 = read_register[PR_YMM6];
	context->ymm7 = read_register[PR_YMM7];
	context->ymm8 = read_register[PR_YMM8];
	context->ymm9 = read_register[PR_YMM9];
	context->ymm10 = read_register[PR_YMM10];
	context->ymm11 = read_register[PR_YMM11];
	context->ymm12 = read_register[PR_YMM12];
	context->ymm13 = read_register[PR_YMM13];
	context->ymm14 = read_register[PR_YMM14];
	context->ymm15 = read_register[PR_YMM15];

	context->cs = (unsigned short)read_register[PR_REG_CS];
	context->ds = (unsigned short)read_register[PR_REG_DS];
	context->es = (unsigned short)read_register[PR_REG_ES];
	context->fs = (unsigned short)read_register[PR_REG_FS];
	context->gs = (unsigned short)read_register[PR_REG_GS];
	context->ss = (unsigned short)read_register[PR_REG_SS];

	return true;
}

//
//
//
bool uc_linker::open(char *path)
{
	return false;
}

bool uc_linker::open(unsigned long bit)
{
	if (bit == 32)
	{
		mode_ = UC_MODE_32;
	}
	else
	{
		mode_ = UC_MODE_64;
	}

	if (uc_)
	{
		uc_close(uc_);
	}

	if (uc_open(UC_ARCH_X86, mode_, &uc_) == 0)
	{
		cpu_context_type segment_context = { 0, };
		segment_context.cs = 0x23;
		segment_context.ds = 0x2b;
		segment_context.es = 0x2b;
		segment_context.fs = 0x53;
		segment_context.ss = 0x88;
		segment_context.gs = 0x63;

		if (create_global_descriptor_table(segment_context, mode_))
		{
			return true;
		}
	}

	return false;
}

bool uc_linker::close()
{
	uc_close(uc_);
	return true;
}

bool uc_linker::get_thread_id_list(std::list<unsigned long> &tid_list)
{
	return false;
}

bool uc_linker::select_thread(unsigned long tid)
{
	return false;
}

bool uc_linker::query_virtual_memory(unsigned long long virtual_address, void *out_memory_info)
{
	uc_memory_t *uc_mem = (uc_memory_t*)out_memory_info;
	uc_mem_region *um = nullptr;
	uint32_t count = 0;

	if (uc_mem_regions(uc_, &um, &count) != 0)
		return false;
	std::shared_ptr<void> uc_memory_closer(um, free);

	for (unsigned int i = 0; i < count; ++i)
	{
		if (virtual_address >= um[i].begin && virtual_address <= um[i].end)
		{
			uc_mem->base_address = um[i].begin;
			uc_mem->end_address = um[i].end - um[i].begin;

			return true;
		}
	}

	return false;
}

void * uc_linker::virtual_alloc(unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type)
{
	base = alignment(base, 0x1000) - 0x1000;
	unsigned long long base_size = alignment(size, 0x1000);

	if (uc_mem_map(uc_, base, (size_t)base_size, UC_PROT_ALL) == 0)
	{
		return (void *)base;
	}

	return nullptr;
}

int uc_linker::virtual_free(void *ptr, unsigned long size, unsigned long free_type)
{
	return 0;
}

unsigned long __stdcall uc_linker::read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	if (uc_mem_read(uc_, virtual_address, out_memory, (size_t)read_size) == 0)
	{
		return read_size;
	}

	return 0;
}

unsigned long __stdcall uc_linker::write_virtual_memory(unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size)
{
	if (uc_mem_write(uc_, virtual_address, input_memory, write_size) == 0)
	{
		return write_size;
	}

	return 0;
}

bool __stdcall uc_linker::get_thread_context(cpu_context_type *context)
{
	if (mode_ == UC_MODE_32)
	{
		return read_x86_cpu_context(context);
	}
	
	return read_x64_cpu_context(context);
}

bool __stdcall uc_linker::set_thread_context(cpu_context_type *context)
{
	if (!context)
	{
		return false;
	}

	context->cs = 0x23;
	context->ds = 0x2b;
	context->es = 0x2b;
	context->fs = 0x53;
	context->ss = 0x88;
	context->gs = 0x63;

	if (mode_ == UC_MODE_32)
	{
		return write_x86_cpu_context(*context);
	}

	return  write_x64_cpu_context(*context);
}

bool uc_linker::get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	return false;
}

bool uc_linker::stack_trace(unsigned long tid, stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count)
{
	return false;
}

bool uc_linker::write_dump(char *path)
{
	return false;
}