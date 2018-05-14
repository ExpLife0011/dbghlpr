#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#include <interface.h>

#pragma comment(lib, "dbgcore.lib")

void test_case_1(unsigned long long address, char *dump_path) // direct disasm dump
{
	dbg::util::dbgeng_disasm_context ctx;
	dbg::util::init_context(&ctx);

	char str[500] = { 0, };
	unsigned long size_of_str = 0;
	unsigned long long next_address = 0;

	ctx.c = nullptr;
	ctx.path = dump_path;
	ctx.pid = 0;

	ctx.buffer = str;
	ctx.size_of_buffer = sizeof(str);
	ctx.size_of_name = &size_of_str;
	ctx.next_address = &next_address;

	printf("test case 1:: %I64x, %s\n", address, dump_path);
	if (dbg::linker::util::disasm(IID_DBGENG_UTIL, address, nullptr, &ctx, sizeof(dbg::util::dbgeng_disasm_context)))
	{
		printf("%s", ctx.buffer);
	}
	else
	{
		printf("f1\n");
	}
}

void test_case_2(unsigned long long address, char *dump_path)
{
	dbg::core *c = dbg::linker::api::create(IID_DBGENG_CORE);
	if (!c)
	{
		return;
	}

	if (!c->open(dump_path))
	{
		return;
	}

	unsigned char dump[16] = { 0, };
	unsigned long readn = c->read_virtual_memory(address, dump, 16);
	if (readn != 16)
	{
		return;
	}

	dbg::util::cs_x86_context cs_context;
	dbg::util::init_context(&cs_context);

	cs_context.bit = 32;
	if (dbg::linker::util::disasm(IID_CS_UTIL, address, dump, &cs_context, sizeof(dbg::util::cs_x86_context)))
	{
		printf("insn id=%d, insn_size=%d\n", cs_context.instruction_id, cs_context.instruction_size);
	}
	else
	{
		printf("f2\n");
	}

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

	test_case_1(address, argv[2]);
	test_case_2(address, argv[2]);

	_getch();
}