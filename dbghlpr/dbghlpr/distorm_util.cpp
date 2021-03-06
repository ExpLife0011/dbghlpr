#include <distorm_util.h>
#include <Windows.h>
#include <stdio.h>

distorm_util::distorm_util()
{
}

distorm_util::~distorm_util()
{
}

void distorm_util::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(distorm_util);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;
	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

bool distorm_util::disasm(unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, x86_disasm_context_type *context)
{
	unsigned int dc;
	_CodeInfo ci;
	_DInst di;
	_DecodeType dt;

	if (type == dbg::PROCESSOR_TYPE::X86_PROCESSOR_BIT_TYPE)
	{
		dt = Decode32Bits;
	}
	else
	{
		dt = Decode64Bits;
	}

	ci.code = table;
	ci.codeLen = (int)type;
	ci.codeOffset = address;
	ci.dt = (_DecodeType)dt;
	ci.features = DF_NONE;

	if (distorm_decompose(&ci, &di, 1, &dc) == DECRES_INPUTERR)
	{
		return false;
	}

	if (dc < 1)
	{
		return false;
	}

	unsigned long op_count = 0;
	for (op_count = 0; op_count < 4; ++op_count)
	{
		if (di.ops[op_count].type == 0)
		{
			break;
		}
	}

	context->instruction_id = di.opcode;
	context->instruction_size = di.size;
	context->operand_count = op_count;

	for (unsigned long i = 0; i < op_count; ++i)
	{
		context->operands[i].operand_type = di.ops[i].type;
		unsigned long long value = 0;

		switch (di.ops[i].type)
		{
		case O_IMM:
			value = di.imm.qword;
			context->operands[i].value = value;
			break;

		case O_DISP:
			value = di.disp;
			context->operands[i].value = value;
			break;

		case O_MEM:
			value = di.disp;
			break;

		case O_PC:
			value = address + di.size + di.imm.qword;
			context->operands[i].value = value;
			break;

		case O_REG:
			value = di.ops[i].index;
			context->operands[i].value = value;
			break;

		default:
			break;
		}
	}

	return true;
}

unsigned long long distorm_util::mnemonic_str(void *handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size)
{
	_DecodedInst di;
	_DecodeType dt;
	unsigned int dc;

	if (type == dbg::PROCESSOR_TYPE::X86_PROCESSOR_BIT_TYPE)
	{
		dt = Decode32Bits;
	}
	else
	{
		dt = Decode64Bits;
	}

	if (distorm_decode(address, dump, 16, dt, &di, 1, &dc) == DECRES_INPUTERR)
	{
		return 0;
	}

	if (type == dbg::PROCESSOR_TYPE::X86_PROCESSOR_BIT_TYPE)
	{
		sprintf_s(output, output_size, "%08x %s		 %s	 %s\n", (unsigned long)address, di.instructionHex.p, di.mnemonic.p, di.operands.p);
		_strlwr_s(output, strlen(output) + 1);
	}
	else
	{
		sprintf_s(output, output_size, "%I64x %s		 %s	 %s\n", address, di.instructionHex.p, di.mnemonic.p, di.operands.p);
		_strlwr_s(output, strlen(output) + 1);
	}

	address += di.size;
	return address;
}

bool distorm_util::find_reference_value(dbg::api *api_handle, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map)
{
	return false;
}

bool distorm_util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end)
{
	return false;
}

unsigned long distorm_util::trace(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_range)
{
	return false;
}

unsigned long distorm_util::browse(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	return dbg::END_CODE::X86_END_CODE_ERR;
}

unsigned long distorm_util::run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set)
{
	return dbg::END_CODE::X86_END_CODE_ERR;
}
