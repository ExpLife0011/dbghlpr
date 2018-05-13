#ifndef __DEFINE_CAPSTION_UTIL__
#define __DEFINE_CAPSTION_UTIL__

#include <interface.h>
#include <capstone.h>

#ifdef _WIN64
#pragma comment(lib, "capstone_static_x64.lib")
#else
#pragma comment(lib, "capstone_static.lib")
#endif

class __declspec(uuid("3BF5CCE6-429E-4A12-AFCB-DE1115B3B31E")) cs_util : public dbg::util
{
private:
	csh cs_handle_;
	cs_insn *insn_;

public:
	cs_util();
	~cs_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool open(unsigned long arch, unsigned long mode);
	virtual bool disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size);
};

#endif