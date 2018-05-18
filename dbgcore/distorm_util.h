#ifndef __DEFINE_DISTORM_UTIL__
#define __DEFINE_DISTORM_UTIL__

#include <interface.h>
#include <distorm/include/distorm.h>
#include <distorm/include/mnemonics.h>
#include <set>

class __declspec(uuid("2F9A74F9-FCEE-4AB2-844D-C1C48C0602D0")) distorm_util : public dbg::util
{
public:
	distorm_util();
	~distorm_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *context);
	virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size);

	virtual bool calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end);
	virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::util::code &b, bool is_range);

	virtual unsigned long browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set, bool is_safe);
	virtual unsigned long analyze(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, std::set<unsigned long long> &entry_point_set);
};

#endif