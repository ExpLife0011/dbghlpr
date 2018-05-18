#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#include <interface.h>

#pragma comment(lib, "dbgcore.lib")

void distorm_test(unsigned long long address, char *dump_path)
{
	dbg::api *c = dbg::linker::api::create(IID_DBGENG_CORE);
	if (!c)
	{
		return;
	}

	if (!c->open(dump_path))
	{
		return;
	}

	unsigned char dump[16] = { 0, };
	unsigned long readn = dbg::linker::api::read_virtual_memory(c, address, dump, 16);
	if (readn != 16)
	{
		return;
	}

	dbg::util *u = dbg::linker::util::create(IID_DT_UTIL);
	if (!u)
	{
		return;
	}

	char str[1024] = { 0, };
	if (u->mnemonic_str(nullptr, address, 32, dump, str, sizeof(str)))
	{
		printf("dt:: %s", str);
	}

	dbg::util::x86_disasm_context_type context;
	context.bit = 32;
	if (u->disasm(address, dump, &context))
	{
		printf("dt ins id=%d, ins size=%d, opcount=%d\n", context.instruction_id, context.instruction_size, context.operand_count);
		for (unsigned long i = 0; i < context.operand_count; ++i)
		{
			printf("operand %d %08x\n", context.operands[i].operand_type, (unsigned long)context.operands[i].value);
		}
	}
	printf("\n");

	free(u);
	free(c);
}

void cs_test(unsigned long long address, char *dump_path)
{
	dbg::api *c = dbg::linker::api::create(IID_DBGENG_CORE);
	if (!c)
	{
		return;
	}

	if (!c->open(dump_path))
	{
		return;
	}

	unsigned char dump[16] = { 0, };
	unsigned long readn = dbg::linker::api::read_virtual_memory(c, address, dump, 16);
	if (readn != 16)
	{
		return;
	}

	dbg::util *u = dbg::linker::util::create(IID_CS_UTIL);
	if (!u)
	{
		return;
	}

	dbg::util::x86_disasm_context_type context;
	context.bit = 32;
	if (u->disasm(address, dump, &context))
	{
		printf("cs ins id=%d, ins size=%d, opcount=%d\n", context.instruction_id, context.instruction_size, context.operand_count);
		for (unsigned long i = 0; i < context.operand_count; ++i)
		{
			printf("operand %d %08x\n", context.operands[i].operand_type, (unsigned long)context.operands[i].value);
		}
	}

	dbg::util::code b;
	unsigned long ce = u->trace(c, address, b, false);
	printf("%08x size of code block=%d\n", ce, b.address_map.size());
	std::map<unsigned long long, dbg::util::x86_disasm_context_type *>::iterator address_detail_map_b = b.address_map.begin();
	std::map<unsigned long long, dbg::util::x86_disasm_context_type *>::iterator address_detail_map_e = b.address_map.end();
	for (address_detail_map_b; address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
	{
		unsigned char read_dump[16] = { 0, };
		unsigned long readn = dbg::linker::api::read_virtual_memory(c, address_detail_map_b->first, read_dump, 16);
		if (readn != 16)
		{
			continue;
		}

		char str[1024] = { 0, };
		if (u->mnemonic_str(nullptr, (unsigned long)address_detail_map_b->first, 32, read_dump, str, sizeof(str)))
		{
			printf("cs:: %s", str);
		}
	}

	// free block
	for (address_detail_map_b = b.address_map.begin(); address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
	{
		free(address_detail_map_b->second);
	}
	printf("\n");



	free(u);
	free(c);
}

void dbgeng_test(unsigned long long address, char *dump_path)
{
	dbg::api *c = dbg::linker::api::create(IID_DBGENG_CORE);
	if (!c)
	{
		return;
	}

	if (!c->open(dump_path))
	{
		return;
	}

	unsigned char dump[16] = { 0, };
	unsigned long readn = dbg::linker::api::read_virtual_memory(c, address, dump, 16);
	if (readn != 16)
	{
		return;
	}

	dbg::util *u = dbg::linker::util::create(IID_DBGENG_UTIL);
	if (!u)
	{
		return;
	}

	char str[1024] = { 0, };
	if (u->mnemonic_str(c, address, 32, dump, str, sizeof(str)))
	{
		printf("de:: %s", str);
	}
	printf("\n");

	free(u);
	free(c);
}

void main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return;
	}

	char *end = nullptr;
	unsigned long long address = strtoll(argv[1], &end, 16);

	// dump test
	distorm_test(address, argv[2]);
	cs_test(address, argv[2]);
	dbgeng_test(address, argv[2]);

	_getch();
}