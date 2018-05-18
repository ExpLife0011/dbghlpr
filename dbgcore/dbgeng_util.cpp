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

bool dbgeng_util::disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *context)
{
	return false;
}

unsigned long long dbgeng_util::mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size)
{
	try
	{
		if (!handle)
		{
			return 0;
		}

		dbg::api *api = (dbg::api *)handle;
		IDebugControl3 *i = (IDebugControl3 *)api->get_instance(DBGENG_DEBUGCONTROL_ID);

		if (!i)
		{
			return 0;
		}

		if (processor_bit == 32)
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

bool dbgeng_util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end)
{
	return false;
}

unsigned long dbgeng_util::trace(void *handle, unsigned long long ptr, dbg::util::code &b, bool is_range)
{
	return X86_END_CODE_ERR;
}

unsigned long dbgeng_util::browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	return X86_END_CODE_ERR;
}

unsigned long dbgeng_util::analyze(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, std::set<unsigned long long> &entry_point_set)
{
	return X86_END_CODE_ERR;
}
