#ifndef __DEFINE_DBGENG_UTIL__
#define __DEFINE_DBGENG_UTIL__

#include <interface.h>
#include <DbgEng.h>
#include <set>

class __declspec(uuid("B0E0E3B6-6FB7-47D0-B731-DDDD5D3C12C3")) dbgeng_util : public dbg::util
{
public:
	dbgeng_util();
	~dbgeng_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *context);
	virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size);
	virtual bool calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end);
	virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::util::code &b, bool is_range);

	virtual unsigned long browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set, bool is_safe);
	virtual unsigned long run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, std::set<unsigned long long> &entry_point_set);
};

#endif