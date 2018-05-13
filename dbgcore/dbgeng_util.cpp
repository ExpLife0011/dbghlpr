#include <dbgeng_util.h>

dbgeng_util::dbgeng_util()
{
}

dbgeng_util::~dbgeng_util()
{
}

void dbgeng_util::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(dbgeng_util);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;
	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

bool dbgeng_util::open(unsigned long arch, unsigned long mode)
{
	return true;
}

bool dbgeng_util::disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size)
{
	if (context_size != sizeof(dbgeng_disasm_context))
	{
		return false;
	}

	dbgeng_disasm_context *c = (dbgeng_disasm_context *)context;
	IDebugControl3 *i = (IDebugControl3 *)c->c->get_instance(DBGENG_DEBUGCONTROL_ID);

	if (!i)
	{
		return false;
	}

	if (i->Disassemble(address, false, c->buffer, c->size_of_buffer, c->size_of_name, c->next_address) != S_OK)
	{
		return false;
	}

	return true;
}