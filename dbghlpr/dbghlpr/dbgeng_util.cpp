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

bool dbgeng_util::disasm(unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, x86_disasm_context_type *context)
{
	return false;
}

unsigned long long dbgeng_util::mnemonic_str(void *handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size)
{
	try
	{
		if (!handle)
		{
			return 0;
		}

		dbg::api *api = (dbg::api *)handle;
		IDebugClient *client = (IDebugClient *)api->get_object(dbg::OBJECT_ID::TARGET_CONTROLLER_ID);

		if (!client)
		{
			return 0;
		}

		IDebugControl3 *i;
		if (client->QueryInterface(__uuidof(IDebugControl3), (void **)&i) != S_OK)
		{
			return 0;
		}

		if (type == dbg::PROCESSOR_TYPE::X86_PROCESSOR_BIT_TYPE)
		{
			i->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_I386);
		}
		else
		{
			i->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_AMD64);
		}

		unsigned long disasm_size = 0;
		unsigned long long next_address = 0;
		if (i->Disassemble(address, false, output, output_size, &disasm_size, &next_address) != S_OK)
		{
			return 0;
		}

		return next_address;
	}
	catch (...)
	{
	}

	return 0;
}

bool dbgeng_util::find_reference_value(dbg::api *api_handle, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map)
{
	return false;
}

bool dbgeng_util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end)
{
	return false;
}

unsigned long dbgeng_util::trace(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_range)
{
	return dbg::END_CODE::X86_END_CODE_ERR;
}

unsigned long dbgeng_util::browse(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	return dbg::END_CODE::X86_END_CODE_ERR;
}

unsigned long dbgeng_util::run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set)
{
	return dbg::END_CODE::X86_END_CODE_ERR;
}

//unsigned long dbgeng_util::switch_64_processor(dbg::api_handle api_handle)
//{
//	dbg::api *api = (dbg::api *)api_handle;
//	IDebugClient *client = (IDebugClient *)api->get_object(dbg::OBJECT_ID::TARGET_CONTROLLER_ID);
//
//	if (!client)
//	{
//		return 0;
//	}
//
//	IDebugControl3 *i;
//	if (client->QueryInterface(__uuidof(IDebugControl3), (void **)&i) != S_OK)
//	{
//		return 0;
//	}
//
//	unsigned long type = 0;
//	i->GetEffectiveProcessorType(&type);
//
//	if (type == IMAGE_FILE_MACHINE_I386)
//	{
//		i->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_AMD64);
//	}
//	else
//	{
//		i->SetEffectiveProcessorType(IMAGE_FILE_MACHINE_I386);
//	}
//
//	return 1;
//}
//
//unsigned long dbgeng_util::get_processor_bit(dbg::api_handle api_handle)
//{
//	dbg::api *api = (dbg::api *)api_handle;
//	IDebugClient *client = (IDebugClient *)api->get_object(dbg::OBJECT_ID::TARGET_CONTROLLER_ID);
//
//	if (!client)
//	{
//		return 0;
//	}
//
//	IDebugControl3 *i;
//	if (client->QueryInterface(__uuidof(IDebugControl3), (void **)&i) != S_OK)
//	{
//		return 0;
//	}
//
//	unsigned long type = 0;
//	i->GetEffectiveProcessorType(&type);
//
//	if (type == IMAGE_FILE_MACHINE_I386)
//	{
//		return 32;
//	}
//
//	return 64;
//}