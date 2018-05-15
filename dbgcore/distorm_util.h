#ifndef __DEFINE_DISTORM_UTIL__
#define __DEFINE_DISTORM_UTIL__

#include <interface.h>
#include <distorm/include/distorm.h>
#include <distorm/include/mnemonics.h>

class __declspec(uuid("2F9A74F9-FCEE-4AB2-844D-C1C48C0602D0")) distorm_util : public dbg::util
{
public:
	distorm_util();
	~distorm_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *context);
	virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size);
};

#endif