#include <Windows.h>
#include <keystone.h>
#include <stdio.h>

#include <asm.h>

#ifdef _WIN64
#pragma comment(lib, "keystone_dynamic_x64.lib")
#else
#pragma comment(lib, "keystone_dynamic.lib")
#endif

//
assembler::x86::x86()
{
	ks_err err = ks_open(KS_ARCH_X86, KS_MODE_32, &ks_);
	if (err)
	{
		ks_ = nullptr;
	}
}

assembler::x86::~x86()
{
	ks_close(ks_);
}

ks_engine *assembler::x86::get_handle()
{
	return ks_;
}

//
assembler::x64::x64()
{
	ks_err err = ks_open(KS_ARCH_X86, KS_MODE_64, &ks_);
	if (err)
	{
		ks_ = nullptr;
	}
}

assembler::x64::~x64()
{
	ks_close(ks_);
}

ks_engine *assembler::x64::get_handle()
{
	return ks_;
}

bool assembler::assemble(assembler::asm_obj *p, char *mnemonic_str, unsigned char *dump, size_t *size)
{
	ks_engine *ks = p->get_handle();
	if (!ks)
	{
		return false;
	}

	memset(dump, 0, 16);
	*size = 0;

	size_t count = 0;
	unsigned char *encode;
	if (ks_asm(ks, mnemonic_str, 0, &encode, size, &count))
	{
		return false;
	}
	memcpy(dump, encode, *size);

	ks_free(encode);
	return true;
}
