#define _CRT_SECURE_NO_WARNINGS

#include <interface.h>
#include <dbgeng_linker.h>

#include <cs_util.h>
#include <dbgeng_util.h>

#include <windows.h>
#include <stdio.h>

bool check_guid(uuid_type c_iid, uuid_t iid)
{
	if (c_iid.Data1 == iid.Data1 && c_iid.Data2 == iid.Data2 && c_iid.Data3 == iid.Data3)
	{
		if (c_iid.Data4[0] == iid.Data4[0] && c_iid.Data4[1] == iid.Data4[1] && c_iid.Data4[2] == iid.Data4[2]
			&& c_iid.Data4[3] == iid.Data4[3] && c_iid.Data4[4] == iid.Data4[4] && c_iid.Data4[5] == iid.Data4[5]
			&& c_iid.Data4[6] == iid.Data4[6] && c_iid.Data4[7] == iid.Data4[7])
		{
			return true;
		}
	}

	return false;
}

void dbg::util::init_context(dbg::util::dbgeng_disasm_context *ctx)
{
	ctx->buffer = nullptr;
	ctx->c = nullptr;
	ctx->next_address = 0;
	ctx->path = nullptr;
	ctx->pid = 0;
	ctx->size_of_buffer = 0;
	ctx->size_of_name = nullptr;
}

void dbg::util::init_context(dbg::util::cs_x86_context *ctx)
{
	ctx->bit = 0;
	memset(ctx->dump, 0, sizeof(ctx->dump));
	ctx->instruction_group = 0;
	ctx->instruction_id = 0;
	ctx->instruction_size = 0;

	ctx->operand_count = 0;

	for (int i = 0; i < 8; ++i)
	{
		ctx->operands[i].value = 0;
		ctx->operands[i].operand_type = 0;
	}
}

//
//
//
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
bool dbg::linker::util::disasm(dbg::util *u, unsigned long long address, unsigned char *table, void *context, size_t context_size)
{
	return u->disasm(address, table, context, context_size);
}

bool dbg::linker::util::disasm(unsigned long id, unsigned long long address, unsigned char *table, void *context, size_t context_size)
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
		return false;
	}

	iid.Data1 = guid_type.Data1;
	iid.Data2 = guid_type.Data2;
	iid.Data3 = guid_type.Data3;
	memcpy(iid.Data4, guid_type.Data4, sizeof(iid.Data4));

	dbg::util *u = nullptr;
	if (check_guid(iid, __uuidof(cs_util)))
	{
		cs_util cs;
		u = &cs;

		return u->disasm(address, table, context, context_size);
	}
	else if (check_guid(iid, __uuidof(dbgeng_util)))
	{
		dbgeng_util dbgeng;
		u = &dbgeng;

		return u->disasm(address, table, context, context_size);
	}

	return false;
}

//
//
//
dbg::core *dbg::core::create(uuid_type id)
{
	if (check_guid(id, __uuidof(engine_linker)))
	{
		return new engine_linker();
	}

	return nullptr;
}

dbg::util *dbg::util::create(uuid_type id)
{
	if (check_guid(id, __uuidof(cs_util)))
	{
		return new cs_util();
	}
	else if (check_guid(id, __uuidof(dbgeng_util)))
	{
		return new dbgeng_util();
	}

	return nullptr;
}