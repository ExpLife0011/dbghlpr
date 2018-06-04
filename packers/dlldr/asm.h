#ifndef __DEFINE_ASSEMBLER__
#define __DEFINE_ASSEMBLER__

#include <keystone.h>

//
namespace assembler
{
	class asm_obj
	{
	public:
		virtual ks_engine *get_handle() = 0;
	};

	//
	class x86 : public asm_obj
	{
	private:
		ks_engine *ks_;

	public:
		x86();
		~x86();

		virtual ks_engine *get_handle();
	};

	//
	class x64 : public asm_obj
	{
	private:
		ks_engine *ks_;

	public:
		x64();
		~x64();

		virtual ks_engine *get_handle();
	};

	//
	bool assemble(assembler::asm_obj *p, char *mnemonic_str, unsigned char *dump, size_t *size);
}

#endif