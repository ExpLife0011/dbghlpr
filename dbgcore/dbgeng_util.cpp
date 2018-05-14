#include <dbgeng_util.h>
#include <stdio.h>

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

bool dbgeng_util::disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size)
{
	if (context_size != sizeof(dbgeng_disasm_context))
	{
		return false;
	}

	IDebugControl3 *i = nullptr;
	dbgeng_disasm_context *c = (dbgeng_disasm_context *)context;

	try
	{
		if (c->c)
		{
			i = (IDebugControl3 *)c->c->get_instance(DBGENG_DEBUGCONTROL_ID);
		}
		else if (strlen(c->path))
		{
			IDebugClient *dc = nullptr;
			if (DebugCreate(__uuidof(IDebugClient), (void **)&dc) != S_OK)
			{
				return false;
			}

			if (dc->OpenDumpFile(c->path) != S_OK)
			{
				return false;
			}

			if ((dc->QueryInterface(__uuidof(IDebugControl3), (void **)&i) != S_OK))
			{
				return false;
			}

			if (i->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE) != S_OK)
			{
				return false;
			}
		}

		if (!i)
		{
			return false;
		}

		if (i->Disassemble(address, false, c->buffer, c->size_of_buffer, c->size_of_name, c->next_address) != S_OK)
		{
			printf("f");

			return false;
		}

		return true;
	}
	catch (...)
	{
	}

	return false;
}