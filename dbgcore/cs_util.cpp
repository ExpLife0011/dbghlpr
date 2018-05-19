#include <cs_util.h>
#include <Windows.h>
#include <stdio.h>

cs_util::cs_util() : cs_handle_(0)
{
}

cs_util::~cs_util()
{
	cs_close(&cs_handle_);
	if (insn_)
	{
		cs_free(insn_, 1);
		insn_ = nullptr;
	}
}

void cs_util::get_uuid(uuid_type *iid)
{
	uuid_t guid_type = __uuidof(cs_util);

	iid->Data1 = guid_type.Data1;
	iid->Data2 = guid_type.Data2;
	iid->Data3 = guid_type.Data3;
	memcpy(iid->Data4, guid_type.Data4, sizeof(iid->Data4));
}

bool cs_util::open(unsigned long arch, unsigned long mode)
{
	if (cs_open((cs_arch)arch, (cs_mode)mode, &cs_handle_) != 0)
	{
		cs_handle_ = 0;
		return false;
	}

	if (cs_option(cs_handle_, CS_OPT_DETAIL, CS_OPT_ON) != 0)
	{
		cs_close(&cs_handle_);
		cs_handle_ = 0;

		return false;
	}

	insn_ = cs_malloc(cs_handle_);

	return true;
}

bool cs_util::disasm(unsigned long long address, unsigned char *table, x86_disasm_context_type *c)
{
	if (cs_handle_ == 0)
	{
		if (c->bit == 32)
		{
			if (!open(CS_ARCH_X86, CS_MODE_32))
			{
				return false;
			}
		}
		else
		{
			if (!open(CS_ARCH_X86, CS_MODE_64))
			{
				return false;
			}
		}
	}

	size_t size = 16;
	if (cs_handle_ == 0 || insn_ == nullptr)
	{
		return false;
	}

	if (!cs_disasm_iter(cs_handle_, (const unsigned char **)&table, &size, &address, insn_))
	{
		return false;
	}

	c->instruction_id = insn_->id;
	c->instruction_size = insn_->size;
	
	cs_x86 *x86 = &(insn_->detail->x86);
	cs_x86_op *op = x86->operands;

	c->operand_count = x86->op_count;
	for (int i = 0; i < x86->op_count; ++i)
	{
		cs_x86_op *op = &(x86->operands[i]);
		switch ((int)op->type)
		{
		case X86_OP_IMM:
			c->operands[i].operand_type = X86_OP_IMM;
			c->operands[i].value = op->imm;
			break;

		case X86_OP_MEM:
			c->operands[i].operand_type = X86_OP_MEM;
			c->operands[i].value = op->mem.disp;
			break;

		case X86_OP_REG:
			c->operands[i].operand_type = X86_OP_REG;
			c->operands[i].value = op->reg;
			break;

		default:
			break;
		}
	}

	c->instruction_group = 0;
	if (cs_insn_group(cs_handle_, insn_, X86_GRP_JUMP))
	{
		c->instruction_group = X86_GRP_JUMP;
	}
	else if (cs_insn_group(cs_handle_, insn_, X86_GRP_RET))
	{
		c->instruction_group = X86_GRP_RET;
	}
	else if (cs_insn_group(cs_handle_, insn_, X86_GRP_INT))
	{
		c->instruction_group = X86_GRP_INT;
	}

	return true;
}

unsigned long long cs_util::mnemonic_str(void *handle, unsigned long long address, unsigned long processor_bit, unsigned char *dump, char *output, size_t output_size)
{
	try
	{
		if (cs_handle_ == 0)
		{
			if (processor_bit == 32)
			{
				if (!open(CS_ARCH_X86, CS_MODE_32))
				{
					return 0;
				}
			}
			else
			{
				if (!open(CS_ARCH_X86, CS_MODE_64))
				{
					return 0;
				}
			}
		}

		if (cs_handle_ == 0 || insn_ == nullptr)
		{
			return 0;
		}

		size_t size = 16;
		if (!cs_disasm_iter(cs_handle_, (const unsigned char **)&dump, &size, &address, insn_))
		{
			return 0;
		}

		if (processor_bit == 32)
		{
			sprintf_s(output, output_size, "%08x	%s	%s\n", (unsigned long)insn_->address, insn_->mnemonic, insn_->op_str);
		}
		else
		{
			sprintf_s(output, output_size, "%I64x	%s	%s\n", insn_->address, insn_->mnemonic, insn_->op_str);
		}

		return address;
	}
	catch (...)
	{
	}

	return 0;
}

bool cs_util::calc_segment(void *handle, unsigned long long ptr, unsigned long long *alloc_base, unsigned long long *alloc_end)
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

bool cs_util::check(unsigned long long ptr, unsigned long long base, unsigned long long end)
{
	if (base <= ptr && end >= ptr)
	{
		return true;
	}

	return false;
}

//
//
//
unsigned long cs_util::trace(void *handle, unsigned long long ptr, dbg::util::code &b)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return X86_END_CODE_ERR;
	}

	unsigned char dump[16] = { 0, };
	unsigned long ret = 0;
	std::set<unsigned long long> visit;
	std::list<unsigned long long> stack;
	std::list<unsigned long long>::iterator bit;
	while (ptr)
	{
		if (!api->read_virtual_memory(ptr, dump, 16))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		dbg::util::x86_disasm_context_type *context = dbg::util::create_segment();
		if (!disasm(ptr, dump, context))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		bool is_jmp = false;
		if (context->instruction_group == X86_GRP_JUMP)
		{
			is_jmp = true;
		}

		//
		// 방문 확인
		// 
		std::set<unsigned long long>::iterator vit = visit.find(ptr);
		if (vit != visit.end())
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();

				continue;
			}
			else
			{
				ret = X86_END_CODE_ERR;
				break;
			}
		}

		//
		// 방문하지 않았다면, 분석
		//
		b.address_map.insert(std::map<unsigned long long, dbg::util::x86_disasm_context_type *>::value_type(ptr, context));
		visit.insert(std::set<unsigned long long>::value_type(ptr));

		//
		// 조건문
		// 
		if (is_jmp && context->instruction_id != X86_INS_JMP)
		{
			stack.push_back(ptr + context->instruction_size);
			if (context->operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context->operands[0].value;

			continue;
		}

		//
		// 나머지
		//
		if (context->instruction_id == X86_INS_JMP)
		{
			if (context->operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context->operands[0].value;
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
		}
		else if (context->instruction_group == X86_GRP_RET)
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_RET;
				break;
			}
		}
		else if (context->instruction_group != X86_GRP_INT && !(dump[0] == 0 && dump[1] == 0))
		{
			ptr += context->instruction_size;
		}
		else
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_NUL;
				break;
			}
		}
	}

	return ret;
}

unsigned long cs_util::safe_trace(void *handle, unsigned long long ptr, dbg::util::code &b)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return X86_END_CODE_ERR;
	}

	unsigned char dump[16] = { 0, };
	unsigned long long base = 0;
	unsigned long long end = 0;
	if (!calc_segment(handle, ptr, &base, &end))
	{
		return X86_END_CODE_ERR;
	}

	unsigned long ret = 0;
	std::set<unsigned long long> visit;
	std::list<unsigned long long> stack;
	std::list<unsigned long long>::iterator bit;
	while (ptr && check(ptr, base, end))
	{
		if (!api->read_virtual_memory(ptr, dump, 16))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		dbg::util::x86_disasm_context_type *context = dbg::util::create_segment();
		if (!disasm(ptr, dump, context))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		bool is_jmp = false;
		if (context->instruction_group == X86_GRP_JUMP)
		{
			is_jmp = true;
		}

		std::set<unsigned long long>::iterator vit = visit.find(ptr);
		if (vit != visit.end())
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();

				continue;
			}
			else
			{
				ret = X86_END_CODE_ERR;
				break;
			}
		}

		b.address_map.insert(std::map<unsigned long long, dbg::util::x86_disasm_context_type *>::value_type(ptr, context));
		visit.insert(std::set<unsigned long long>::value_type(ptr));

		if (is_jmp && context->instruction_id != X86_INS_JMP) // switch case
		{
			stack.push_back(ptr + context->instruction_size);
			if (context->operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context->operands[0].value;

			continue;
		}

		if (context->instruction_id == X86_INS_JMP)
		{
			if (context->operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context->operands[0].value;

			if (!check(ptr, base, end) && stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
		}
		else if (context->instruction_group == X86_GRP_RET)
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_RET;
				break;
			}
		}
		else if (context->instruction_group != X86_GRP_INT && !(dump[0] == 0 && dump[1] == 0))
		{
			ptr += context->instruction_size;
		}
		else
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				break;
			}
		}
	}

	return ret;
}

unsigned long cs_util::trace(void *handle, unsigned long long ptr, dbg::util::code &b, bool is_safe)
{
	if (is_safe)
	{
		return safe_trace(handle, ptr, b);
	}
	else
	{
		return trace(handle, ptr, b);
	}
}

//
//
//
unsigned long cs_util::browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return X86_END_CODE_ERR;
	}

	unsigned char dump[16] = { 0, };
	unsigned long ret = 0;
	std::list<unsigned long long> stack;
	std::list<unsigned long long>::iterator bit;
	while (ptr)
	{
		if (!api->read_virtual_memory(ptr, dump, 16))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		dbg::util::x86_disasm_context_type context;
		if (!disasm(ptr, dump, &context))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		bool is_jmp = false;
		if (context.instruction_group == X86_GRP_JUMP)
		{
			is_jmp = true;
		}

		std::set<unsigned long long>::iterator vit = entry_point_set.find(ptr);
		if (vit != entry_point_set.end())
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();

				continue;
			}
			else
			{
				ret = X86_END_CODE_ERR;
				break;
			}
		}

		entry_point_set.insert(std::set<unsigned long long>::value_type(ptr));

		if (is_jmp && context.instruction_id != X86_INS_JMP)
		{
			stack.push_back(ptr + context.instruction_size);
			if (context.operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context.operands[0].value;

			continue;
		}

		if (context.instruction_id == X86_INS_JMP)
		{
			if (context.operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context.operands[0].value;
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
		}
		else if (context.instruction_group == X86_GRP_RET)
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_RET;
				break;
			}
		}
		else if (context.instruction_group != X86_GRP_INT && !(dump[0] == 0 && dump[1] == 0))
		{
			ptr += context.instruction_size;
		}
		else
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_NUL;
				break;
			}
		}
	}

	return ret;
}

unsigned long cs_util::browse_safe(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return X86_END_CODE_ERR;
	}

	unsigned char dump[16] = { 0, };
	unsigned long long base = 0;
	unsigned long long end = 0;
	if (!calc_segment(handle, ptr, &base, &end))
	{
		return X86_END_CODE_ERR;
	}

	unsigned long ret = 0;
	std::list<unsigned long long> stack;
	std::list<unsigned long long>::iterator bit;
	while (ptr && check(ptr, base, end))
	{
		if (!api->read_virtual_memory(ptr, dump, 16))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		dbg::util::x86_disasm_context_type context;
		if (!disasm(ptr, dump, &context))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		bool is_jmp = false;
		if (context.instruction_group == X86_GRP_JUMP)
		{
			is_jmp = true;
		}

		std::set<unsigned long long>::iterator vit = entry_point_set.find(ptr);
		if (vit != entry_point_set.end())
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();

				continue;
			}
			else
			{
				ret = X86_END_CODE_ERR;
				break;
			}
		}

		entry_point_set.insert(std::set<unsigned long long>::value_type(ptr));

		if (is_jmp && context.instruction_id != X86_INS_JMP) // switch case
		{
			stack.push_back(ptr + context.instruction_size);
			if (context.operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context.operands[0].value;

			continue;
		}

		if (context.instruction_id == X86_INS_JMP)
		{
			if (context.operands[0].operand_type != X86_OP_IMM)
			{
				ret = X86_END_CODE_JMP;
				break;
			}

			ptr = context.operands[0].value;

			if (!check(ptr, base, end) && stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
		}
		else if (context.instruction_group == X86_GRP_RET)
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				ret = X86_END_CODE_RET;
				break;
			}
		}
		else if (context.instruction_group != X86_GRP_INT && !(dump[0] == 0 && dump[1] == 0))
		{
			ptr += context.instruction_size;
		}
		else
		{
			if (stack.size())
			{
				bit = stack.end();
				ptr = *(--bit);
				stack.pop_back();
			}
			else
			{
				break;
			}
		}
	}

	return ret;
}

unsigned long cs_util::browse(void *handle, unsigned long long ptr, std::set<unsigned long long> &entry_point_set, bool is_safe)
{
	if (is_safe)
	{
		return browse_safe(handle, ptr, entry_point_set);
	}
	else
	{
		return browse(handle, ptr, entry_point_set);
	}
}

//
//
//
unsigned long cs_util::analyze(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, std::set<unsigned long long> &entry_point_set)
{
	dbg::api *api = (dbg::api *)handle;
	if (!api)
	{
		return X86_END_CODE_ERR;
	}

	unsigned long long ptr = base;
	unsigned char dump[16] = { 0, };
	unsigned long ret = 0;
	std::set<unsigned long long> visit;

	do
	{
		if (!api->read_virtual_memory(ptr, dump, 16))
		{
			ret = X86_END_CODE_ERR;
			break;
		}

		dbg::util::x86_disasm_context_type context;
		if (!disasm(ptr, dump, &context))
		{
			++ptr;
			//offset = (unsigned long)(address - base_address_);
			if (ptr > end)
			{
				ret = X86_END_CODE_NUL;
				break;
			}

			continue;
		}

		bool is_nop = (context.instruction_id == X86_INS_NOP);
		bool is_int = (context.instruction_id == X86_INS_INT3);
		bool is_add = (context.instruction_id == X86_INS_ADD);

		if ((is_nop || is_int || is_add))
		{
			ptr += context.instruction_size;
			//offset = (unsigned long)(address - base_address_);
			if (ptr > end)
			{
				ret = X86_END_CODE_NUL;
				break;
			}
			continue;
		}

		std::set<unsigned long long>::iterator mit = visit.find(ptr);
		if (mit != visit.end())
		{
			ptr += context.instruction_size;
			//offset = (unsigned long)(address - base_address_);
			if (ptr > end)
			{
				ret = X86_END_CODE_NUL;
				break;
			}
			continue;
		}

		size_t insn_size = context.instruction_size;
		browse_safe(handle, ptr, visit);
		entry_point_set.insert(std::set<unsigned long long>::value_type(ptr));

		if (cb)
		{
			if (!cb(api, this, dump, ptr, cb_context))
			{
				break;
			}
		}

		ptr += insn_size;
	} while (ptr <= end);

	return 0;
}
