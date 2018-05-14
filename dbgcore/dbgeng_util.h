#ifndef __DEFINE_DBGENG_UTIL__
#define __DEFINE_DBGENG_UTIL__

#include <interface.h>
#include <DbgEng.h>

class __declspec(uuid("B0E0E3B6-6FB7-47D0-B731-DDDD5D3C12C3")) dbgeng_util : public dbg::util
{
public:
	dbgeng_util();
	~dbgeng_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size);
};

#endif