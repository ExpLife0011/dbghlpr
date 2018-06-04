#include <uc_util.h>

#ifdef _WIN64
#pragma comment(lib, "unicorn_static_x64.lib")
#else
#pragma comment(lib, "unicorn_static.lib")
#endif

uc_util::uc_util()
{
}

uc_util::~uc_util()
{
}

void uc_util::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(uc_util);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;
	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

bool uc_util::disasm(unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, x86_disasm_context_type *context)
{
	return false;
}

unsigned long long uc_util::mnemonic_str(void *handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size)
{
	return 0;
}

bool uc_util::find_reference_value(dbg::api *api_handle, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map)
{
	return 0;
}

bool uc_util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end)
{
	return false;
}

unsigned long uc_util::trace(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_range)
{
	return 0;
}

unsigned long uc_util::browse(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	return 0;
}

//
//
//
void uc_util::code_callback(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
	uc_util::callback_data *cd = (uc_util::callback_data *)user_data;
	if (cd)
	{
		unsigned long err_code = dbg::EMULATOR_ERR::EM_CODE_ERR;
		cd->cb(cd->api, cd->util, &err_code, address, cd->user_context);
	}
}

void uc_util::rw_unmap_callback(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data)
{
	uc_util::callback_data *cd = (uc_util::callback_data *)user_data;
	unsigned long err_code = 0;

	if (type == UC_MEM_WRITE_UNMAPPED)
	{
		err_code = dbg::EMULATOR_ERR::EM_WRITE_ERR;
	}
	else if (type == UC_MEM_READ_UNMAPPED)
	{
		err_code = dbg::EMULATOR_ERR::EM_READ_ERR;
	}

	if (err_code != 0 && cd)
	{
		cd->cb(cd->api, cd->util, &err_code, address, cd->user_context);
	}
}

void uc_util::fetch_unmap_callback(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data)
{
	uc_util::callback_data *cd = (uc_util::callback_data *)user_data;
	if (type == UC_MEM_FETCH_UNMAPPED && cd)
	{
		unsigned long err_code = dbg::EMULATOR_ERR::EM_FETCH_ERR;
		cd->cb(cd->api, cd->util, &err_code, address, cd->user_context);
	}
}

unsigned long uc_util::run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return 0;
	}

	uc_engine *uc = (uc_engine *)api->get_object(dbg::OBJECT_ID::EMULATOR_HANDLE_ID);
	if (!uc)
	{
		return 0;
	}

	callback_data cd;
	cd.api = api;
	cd.util = this;
	cd.cb = cb;
	cd.user_context = cb_context;

	uc_hook code_hook;
	uc_hook_add(uc, &code_hook, UC_HOOK_CODE, code_callback, &cd, (uint64_t)1, (uint64_t)0);

	uc_hook write_unmap_hook;
	uc_hook_add(uc, &write_unmap_hook, UC_HOOK_MEM_WRITE_UNMAPPED, rw_unmap_callback, &cd, (uint64_t)1, (uint64_t)0);

	uc_hook read_unmap_hook;
	uc_hook_add(uc, &read_unmap_hook, UC_HOOK_MEM_READ_UNMAPPED, rw_unmap_callback, &cd, (uint64_t)1, (uint64_t)0);

	uc_hook fetch_hook;
	uc_hook_add(uc, &fetch_hook, UC_HOOK_MEM_FETCH_UNMAPPED, fetch_unmap_callback, &cd, (uint64_t)1, (uint64_t)0);

	uc_err err = uc_emu_start(uc, base, end-base, 0, 1);
	if (err != UC_ERR_OK)
	{
		return 0;
	}

	entry_point_set.insert(base);

	return 1;
}
