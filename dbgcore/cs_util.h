#ifndef __DEFINE_CAPSTION_UTIL__
#define __DEFINE_CAPSTION_UTIL__

#include <interface.h>
#include <capstone.h>
#include <list>
#include <set>

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
	virtual bool disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *context);
	virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size);

	virtual bool check(unsigned long long ptr, unsigned long long base, unsigned long long end);
	virtual bool calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end);

	virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::util::code &b, bool is_safe);
	virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::util::code &b);
	virtual unsigned long safe_trace(void *handle, unsigned long long ptr, dbg::util::code &b);

	virtual unsigned long browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set, bool is_safe);
	virtual unsigned long browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set);
	virtual unsigned long browse_safe(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set);

	virtual unsigned long analyze(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, std::set<unsigned long long> &entry_point_set);
};

#endif