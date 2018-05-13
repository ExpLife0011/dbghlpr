#define _CRT_SECURE_NO_WARNINGS

#include <interface.h>
#include <dbgeng_linker.h>

#include <cs_util.h>
#include <dbgeng_util.h>

#include <windows.h>
#include <stdio.h>

dbg::core *dbg::linker::api::create(unsigned long id)
{
	uuid_t guid_type = { 0, };
	uuid_type iid = { 0, };

	switch (id)
	{
	case IID_DBGENG_CORE:
		guid_type = __uuidof(engine_linker);
		break;

	default:
		return nullptr;
	}

	iid.Data1 = guid_type.Data1;
	iid.Data2 = guid_type.Data2;
	iid.Data3 = guid_type.Data3;
	memcpy(iid.Data4, guid_type.Data4, sizeof(iid.Data4));

	return dbg::core::create(iid);
}

dbg::util *dbg::linker::util::create(unsigned long id)
{
	uuid_t guid_type = { 0, };
	uuid_type iid = { 0, };

	switch (id)
	{
	case IID_CS_UTIL:
		guid_type = __uuidof(cs_util);
		break;

	case IID_DBGENG_UTIL:
		guid_type = __uuidof(dbgeng_util);
		break;

	default:
		return nullptr;
	}

	iid.Data1 = guid_type.Data1;
	iid.Data2 = guid_type.Data2;
	iid.Data3 = guid_type.Data3;

	memcpy(iid.Data4, guid_type.Data4, sizeof(iid.Data4));

	return dbg::util::create(iid);
}

//
//
//
bool dbg::linker::api::open(dbg::core *c, char *path)
{
	return c->open(path);
}

bool dbg::linker::api::open(dbg::core *c, unsigned long pid)
{
	return c->open(pid);
}

bool dbg::linker::api::query_virtual_memory(dbg::core *c, unsigned long long virtual_address, void *out_memory_info)
{
	return c->query_virtual_memory(virtual_address, out_memory_info);
}

unsigned long __stdcall dbg::linker::api::read_virtual_memory(dbg::core *c, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	return c->read_virtual_memory(virtual_address, out_memory, read_size);
}

#if 0
bool dbg::linker::api::disasm(dbg::core *c, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_disasm, unsigned long long *next)
{
	return c->disasm(offset, buffer, size_of_buffer, size_of_disasm, next);
}
#endif

bool __stdcall dbg::linker::api::get_thread_context(dbg::core *c, cpu_context_type *context)
{
	return c->get_thread_context(context);
}

bool dbg::linker::api::get_symbol_name(dbg::core *c, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	return c->get_symbol_name(offset, buffer, size_of_buffer, size_of_name, disp);
}

//
//
//
bool dbg::linker::api::open(unsigned long iid, char *path)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->open(path);
	delete(c);

	return r;
}

bool dbg::linker::api::open(unsigned long iid, unsigned long pid)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->open(pid);
	delete(c);

	return r;
}

bool dbg::linker::api::query_virtual_memory(unsigned long iid, unsigned long long virtual_address, void *out_memory_info)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->query_virtual_memory(virtual_address, out_memory_info);
	delete(c);

	return r;
}

unsigned long __stdcall dbg::linker::api::read_virtual_memory(unsigned long iid, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return 0;
	}

	unsigned long r = c->read_virtual_memory(virtual_address, out_memory, read_size);
	delete(c);

	return r;
}

#if 0
bool dbg::linker::api::disasm(unsigned long iid, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_disasm, unsigned long long *next)
{
	dbg::core *c = dbg::linker::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->disasm(offset, buffer, size_of_buffer, size_of_disasm, next);
	delete(c);

	return r;
}
#endif

bool __stdcall dbg::linker::api::get_thread_context(unsigned long iid, cpu_context_type *context)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->get_thread_context(context);
	delete(c);

	return r;
}

bool dbg::linker::api::get_symbol_name(unsigned long iid, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	dbg::core *c = dbg::linker::api::create(iid);
	if (!c)
	{
		return false;
	}

	bool r = c->get_symbol_name(offset, buffer, size_of_buffer, size_of_name, disp);
	delete(c);

	return r;
}

//
//
//
dbg::core *dbg::core::create(uuid_type id)
{
	char key[128] = { 0, };
	sprintf(key, "%08X-%02X-%02X-%02X%02X-%02X%02X%02X%02X%02X%02X", id.Data1, id.Data2, id.Data3
		, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	if (strstr(key, "60BB225D-A06F-11D1-9138-0020AF715AF0"))
	{
		return new engine_linker();
	}

	return nullptr;
}

dbg::util *dbg::util::create(uuid_type id)
{
	char key[128] = { 0, };
	sprintf(key, "%08X-%02X-%02X-%02X%02X-%02X%02X%02X%02X%02X%02X", id.Data1, id.Data2, id.Data3
		, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	if (strstr(key, "3BF5CCE6-429E-4A12-AFCB-DE1115B3B31E"))
	{
		return new cs_util();
	}
	else if (strstr(key, "B0E0E3B6-6FB7-47D0-B731-DDDD5D3C12C3"))
	{
		return new dbgeng_util();
	}

	return nullptr;
}