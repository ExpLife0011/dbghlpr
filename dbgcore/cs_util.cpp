#include <cs_util.h>
#include <Windows.h>
#include <stdio.h>

cs_util::cs_util() : cs_handle_(0)
{
	X86_INS_AESDEC;
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

bool cs_util::disasm(unsigned long long address, unsigned char *table, void *context, size_t context_size)
{
	if (context_size != sizeof(cs_x86_context))
	{
		return false;
	}

	cs_x86_context *c = (cs_x86_context *)context;
	if (cs_handle_ == 0)
	{
		if (c->bit == 32)
		{
			if (!open(CS_ARCH_X86, CS_MODE_32))
			{
				return false;
			}
		}
		else if(c->bit == 64)
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

	memset(c->dump, 0, 16);
	memcpy(c->dump, insn_->bytes, insn_->size);

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