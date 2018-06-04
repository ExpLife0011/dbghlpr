#define _CRT_SECURE_NO_WARNINGS

#include <interface.h>

#include <dbgeng_linker.h>
#include <uc_linker.h>
#include <rmtdll_linker.h>

#include <cs_util.h>
#include <dbgeng_util.h>
#include <distorm_util.h>
#include <uc_util.h>

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

//
// interface static funcs
// 
dbg::util::x86_disasm_context_type *dbg::util::create_segment()
{
	dbg::util::x86_disasm_context_type *cs = new dbg::util::x86_disasm_context_type;

	cs->instruction_id = 0;
	cs->instruction_size = 0;
	cs->instruction_group = 0;

	for (int i = 0; i < 8; ++i)
	{
		cs->operands[i].operand_type = 0;
		cs->operands[i].value = 0;
	}

	return cs;
}

dbg::api *dbg::api::create(uuid_type id)
{
	if (check_guid(id, __uuidof(engine_linker)))
	{
		return new engine_linker();
	}
	else if (check_guid(id, __uuidof(uc_linker)))
	{
		return new uc_linker();
	}
	else if (check_guid(id, __uuidof(rmtdll_linker)))
	{
		return new rmtdll_linker();
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
	else if (check_guid(id, __uuidof(distorm_util)))
	{
		return new distorm_util();
	}
	else if (check_guid(id, __uuidof(uc_util)))
	{
		return new uc_util();
	}

	return nullptr;
}

bool dbg::util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *alloc_base, unsigned long long *alloc_end)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return false;
	}

	MEMORY_BASIC_INFORMATION64 mbi = { 0, };
	if (!api->query_virtual_memory(ptr, &mbi))
	{
		return false;
	}

	unsigned long long base_address = mbi.AllocationBase;
	unsigned long long base = base_address;
	unsigned long long analyze_base = 0;
	unsigned long long analyze_size = 0;

	unsigned long long protect = mbi.Protect;

	do
	{
		if (!api->query_virtual_memory(base, &mbi))
		{
			break;
		}

		if (mbi.Protect == protect)
		{
			if (analyze_base == 0)
			{
				analyze_base = mbi.BaseAddress;
			}

			analyze_size += mbi.RegionSize;
		}
		else
		{
			if (analyze_base && analyze_size)
			{
				if (analyze_base <= ptr && (analyze_base + analyze_size) >= ptr)
				{
					*alloc_base = analyze_base;
					*alloc_end = analyze_base + analyze_size;

					return true;
				}

				analyze_base = 0;
				analyze_size = 0;
			}
		}

		base = mbi.BaseAddress + mbi.RegionSize;
	} while (base_address == mbi.AllocationBase);

	return false;
}

bool dbg::util::check(unsigned long long ptr, unsigned long long base, unsigned long long end)
{
	if (base <= ptr && end >= ptr)
	{
		return true;
	}

	return false;
}

//
// linker
//
dbg::api_handle dbg::linker::api::create(unsigned long id)
{
	uuid_t guid_type = { 0, };
	uuid_type iid = { 0, };

	switch (id)
	{
	case CORE_IID::IID_DEFAULT_API_CORE:
		guid_type = __uuidof(engine_linker);
		break;

	case CORE_IID::IID_EMULATOR_API_CORE:
		guid_type = __uuidof(uc_linker);
		break;

	case CORE_IID::IID_REMOTEAPI_CORE:
		guid_type = __uuidof(rmtdll_linker);
		break;

	default:
		return nullptr;
	}

	iid.Data1 = guid_type.Data1;
	iid.Data2 = guid_type.Data2;
	iid.Data3 = guid_type.Data3;
	memcpy(iid.Data4, guid_type.Data4, sizeof(iid.Data4));

	return dbg::api::create(iid);
}

dbg::util_handle dbg::linker::util::create(unsigned long id)
{
	uuid_t guid_type = { 0, };
	uuid_type iid = { 0, };

	switch (id)
	{
	case CORE_IID::IID_DISASSEMBLER_UTIL1:
		guid_type = __uuidof(cs_util);
		break;

	case CORE_IID::IID_DEFAULT_UTIL:
		guid_type = __uuidof(dbgeng_util);
		break;

	case CORE_IID::IID_DISASSEMBLER_UTIL2:
		guid_type = __uuidof(distorm_util);
		break;

	case CORE_IID::IID_EMULATOR_UTIL:
		guid_type = __uuidof(uc_util);
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
// api wrap
//
bool dbg::linker::api::open(api_handle handle, char *path)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;

	return api->open(path);
}

bool dbg::linker::api::open(api_handle handle, unsigned long pid)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->open(pid);
}

bool dbg::linker::api::close(api_handle handle)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;

	return api->close();
}

bool dbg::linker::api::get_thread_id_list(api_handle handle, std::list<unsigned long> &tid_list)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->get_thread_id_list(tid_list);
}

bool dbg::linker::api::select_thread(api_handle handle, unsigned long tid)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->select_thread(tid);
}

unsigned long dbg::linker::api::current_thread_id(api_handle handle)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return 0;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->current_thread_id();
}

void * dbg::linker::api::virtual_alloc(api_handle handle, unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return nullptr;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->virtual_alloc(base, size, allocation_type, protect_type);
}

int dbg::linker::api::virtual_free(api_handle handle, void *ptr, unsigned long size, unsigned long free_type)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->virtual_free(ptr, size, free_type);
}

bool dbg::linker::api::query_virtual_memory(api_handle handle, unsigned long long virtual_address, void *out_memory_info)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->query_virtual_memory(virtual_address, out_memory_info);
}

unsigned long __stdcall dbg::linker::api::read_virtual_memory(api_handle handle, unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return 0;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->read_virtual_memory(virtual_address, out_memory, read_size);
}

unsigned long __stdcall dbg::linker::api::write_virtual_memory(api_handle handle, unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return 0;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->write_virtual_memory(virtual_address, input_memory, write_size);
}

bool __stdcall dbg::linker::api::get_thread_context(api_handle handle, cpu_context_type *context)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->get_thread_context(context);
}

bool __stdcall dbg::linker::api::set_thread_context(api_handle handle, cpu_context_type *context)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->set_thread_context(context);
}

bool dbg::linker::api::get_symbol_name(api_handle handle, unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->get_symbol_name(offset, buffer, size_of_buffer, size_of_name, disp);
}

bool dbg::linker::api::stack_trace(api_handle handle, unsigned long tid, dbg::api::stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->stack_trace(tid, stack_frame, size_of_stack_frame, stack_count);
}

bool dbg::linker::api::write_dump(api_handle handle, char *path)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->write_dump(path);
}

bool dbg::linker::api::suspend(api_handle handle, unsigned long tid)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->suspend(tid);
}

bool dbg::linker::api::resume(api_handle handle, unsigned long tid)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(engine_linker)) || check_guid(iid, __uuidof(uc_linker)) || check_guid(iid, __uuidof(rmtdll_linker))))
	{
		return false;
	}

	dbg::api *api = (dbg::api *)handle;
	return api->resume(tid);
}

//
//
//
bool dbg::linker::util::disasm(util_handle handle, unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, dbg::util::x86_disasm_context_type *context)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	return util->disasm(address, table, type, context);
}

unsigned long long dbg::linker::util::mnemonic_str(util_handle handle, void *api_handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	return util->mnemonic_str(api_handle, address, dump, type, output, output_size);
}

bool dbg::linker::util::find_reference_value(util_handle handle, api_handle api, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	dbg::api *api_handle = (dbg::api *)api;
	return util->find_reference_value(api_handle, base, size, type, ref_map);
}

unsigned long dbg::linker::util::trace(util_handle handle, void *api_handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_safe)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	return util->trace(api_handle, ptr, type, b, is_safe);
}

unsigned long dbg::linker::util::browse(util_handle handle, void *api_handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	return util->browse(api_handle, ptr, type, entry_point_set, is_safe);
}

unsigned long dbg::linker::util::run(util_handle handle, void *api_handle, dbg::util::analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set)
{
	dbg::object *obj = (dbg::object *)handle;
	uuid_type iid;
	obj->get_uuid(&iid);

	if (!(check_guid(iid, __uuidof(cs_util)) || check_guid(iid, __uuidof(distorm_util)) || check_guid(iid, __uuidof(dbgeng_util)) || check_guid(iid, __uuidof(uc_util))))
	{
		return false;
	}

	dbg::util *util = (dbg::util *)handle;
	return util->run(api_handle, cb, cb_context, base, end, type, entry_point_set);
}

