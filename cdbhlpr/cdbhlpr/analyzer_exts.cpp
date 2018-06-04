#define _CRT_SECURE_NO_WARNINGS

#include <engextcpp.hpp>
#include <windows.h>

#include <list>
#include <memory>

#include <interface.h>
#include <engine.h>

#include <capstone.h>
#include <engine_linker.h>

#include <analyzer_wrapper.h>

EXT_CLASS_COMMAND(WindbgEngine, test, "", "{p;ed,o;p;;}" "{exe;b,o;exe;;}")
{
	char cmd[500];

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, ".browse !chkdr");
	dprintf(" "), g_Ext->DmlCmdExec("[+]", cmd), dprintf(" check debug register\n");

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, ".browse !chkmem -exe");
	dprintf(" "), g_Ext->DmlCmdExec("[+]", cmd), dprintf(" check exe memory\n");

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "!vmcode -b 0 -l 0x7fffffff");
	dprintf(" "), g_Ext->DmlCmdExec("[+]", cmd), dprintf(" find alloc code\n");
}

//
//
//
EXT_CLASS_COMMAND(WindbgEngine, chkdr, "", "{p;ed,o;p;;}")
{
	engine_linker e;
	unsigned long thread_count = 0;

	if (g_Ext->m_System->GetNumberThreads(&thread_count) == S_OK)
	{
		unsigned long tids[1024] = { 0, };
		if (g_Ext->m_System->GetThreadIdsByIndex(0, thread_count, nullptr, tids) == S_OK)
		{
			for (unsigned long i = 0; i < thread_count; ++i)
			{
				g_Ext->ExecuteSilent("~~[%x]s", tids[i]);
				cpu_context_type context;
				e.get_thread_context(&context);

				if (context.dr0 || context.dr1 || context.dr2 || context.dr3 || context.dr6 || context.dr7)
				{
					dprintf("[tid] %d\n", tids[i]);
					dprintf("dr0=%I64x ", context.dr0);
					dprintf("dr1=%I64x ", context.dr1);
					dprintf("dr2=%I64x ", context.dr2);
					dprintf("dr3=%I64x ", context.dr3);
					dprintf("dr6=%I64x ", context.dr6);
					dprintf("dr7=%I64x\n", context.dr7);
				}
			}
			g_Ext->ExecuteSilent("~~[%x]s", tids[0]);
			dprintf("\n");
		}
	}
}

//
//
//
EXT_CLASS_COMMAND(WindbgEngine, calc, "", "{p;ed,o;p;;}" "{exe;b,o;exe;;}")
{
	unsigned long long ptr = GetArgU64("p", FALSE);
	unsigned long long base = 0;
	unsigned long long end = 0;
	analyzer_wrapper::calc_exe_segment(ptr, &base, &end);

	dprintf("%I64x-%I64x, %I64x\n", base, end, end - base);
}

//
//
//
EXT_CLASS_COMMAND(WindbgEngine, fe, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{p;ed,o;p;;}") // find entry
{
	unsigned long long ptr = 0;
	unsigned long long base = 0;
	unsigned long long end = 0;

	if(HasArg("b") && HasArg("l") && HasArg("p"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
		ptr = GetArgU64("p", FALSE);
	}
	else if(HasArg("p"))
	{
		 ptr = GetArgU64("p", FALSE);
		 base = 0;
		 end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			dprintf("%I64x is not exe memory\n", ptr);
			dprintf("0:000> !fe -b xxxx -l xxxx -p xxxx\n\n");
			return;
		}

		if (analyzer_wrapper::check(base, end, ptr - 0x1000))
		{
			base = ptr - 0x1000;
		}
	}

	dprintf(" [-] %I64x-%I64x\n", base, end);
	ptr = analyzer_wrapper::find_entry(ptr, base, end - base);
	if (ptr)
	{
		dprintf("	[-] %I64x\n", ptr);
	}
	else
	{
		dprintf("	[-] nop\n");
	}
}

EXT_CLASS_COMMAND(WindbgEngine, fae, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{p;ed,o;p;;}") // find all entry
{
	unsigned long long ptr = 0;
	unsigned long long base = 0;
	unsigned long long end = 0;

	if (HasArg("b") && HasArg("l"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
	}
	else if (HasArg("p"))
	{
		ptr = GetArgU64("p", FALSE);
		base = 0;
		end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			dprintf("%I64x is not exe memory\n", ptr);
			dprintf("0:000> !fae -b xxxx -l xxxx -p xxxx\n\n");
			return;
		}
	}

	dprintf(" [-] %I64x-%I64x\n", base, end);

	std::set<unsigned long long> entry_point_set;
	if (analyzer_wrapper::find_all_entry(base, end - base, entry_point_set))
	{
		int i = 0;
		std::set<unsigned long long>::iterator entry_point_set_it = entry_point_set.begin();
		for (entry_point_set_it; entry_point_set_it != entry_point_set.end(); ++entry_point_set_it, ++i)
		{
			if (i % 8 == 0)
			{
				dprintf("\n");
			}

#ifdef _WIN64
			dprintf("%I64x ", *entry_point_set_it);
#else
			dprintf("%08x ", *entry_point_set_it);
#endif
		}
	}
}

//
//
//
EXT_CLASS_COMMAND(WindbgEngine, caller, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{p;ed,o;p;;}") // find caller
{
	unsigned long long ptr = 0;
	unsigned long long base = 0;
	unsigned long long end = 0;
	unsigned long long entry_base = 0;
	std::list<unsigned long long> l;

	if (HasArg("b") && HasArg("l") && HasArg("p"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
		ptr = GetArgU64("p", FALSE);

		ptr = analyzer_wrapper::find_entry(ptr, base, end - base);
	}
	else if (HasArg("p"))
	{
		ptr = GetArgU64("p", FALSE);
		base = 0;
		end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			dprintf("%I64x is not exe memory\n", ptr);
			dprintf("0:000> !caller -b xxxx -l xxxx -p xxxx\n\n");
			return;
		}

		if (analyzer_wrapper::check(base, end, ptr - 0x1000))
		{
			entry_base = ptr - 0x1000;
		}
		else
		{
			entry_base = base;
		}

		ptr = analyzer_wrapper::find_entry(ptr, entry_base, end - entry_base);
	}

	if (ptr)
	{
		analyzer_wrapper::find_caller(ptr, base, end - base, l);
		std::list<unsigned long long>::iterator it = l.begin();
		if (l.size())
		{
			for (it; it != l.end(); ++it)
			{
				char cmd[500] = { 0, };
				memset(cmd, 0, sizeof(cmd));
				sprintf(cmd, "!caller -p %I64x", *it);

				char addr[16];
				memset(addr, 0, sizeof(addr));
				sprintf(addr, "%I64x", *it);

				g_Ext->DmlCmdLink(addr, cmd), dprintf(" ");
			}
			dprintf("\n");
		}
		else
		{
			dprintf("	[-] nop\n");
		}
	}
	else
	{
		dprintf("	[-] zero\n");
	}

	dprintf("\n");
}

//
//
//
#include <helper.h>
#include <strsafe.h>

void print_ascii(unsigned char *p, size_t l)
{
	for (size_t i = 0; i < l; ++i)
	{
		if (isprint(p[i]))
			dprintf("%c", p[i]);
	}
}

void print_unicode(unsigned char *p, size_t l)
{
	for (size_t i = 0; i < l * 2; ++i)
	{
		if (isprint(p[i]))
			dprintf("%c", p[i]);
	}
}

EXT_CLASS_COMMAND(WindbgEngine, refstr, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{p;ed,o;p;;}") // ref string
{
	std::shared_ptr<engine::linker> engine;
	if (!engine::create<engine_linker>(engine))
	{
		return;
	}

	unsigned long long ptr = 0;
	unsigned long long base = 0;
	unsigned long long end = 0;
	std::multimap<unsigned long long, unsigned long long> ref_map;

	if (HasArg("b") && HasArg("l"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
	}
	else if (HasArg("p"))
	{
		ptr = GetArgU64("p", FALSE);
		base = 0;
		end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			dprintf("%I64x is not exe memory\n", ptr);
			dprintf("0:000> !refstr -b xxxx -l xxxx -p xxxx\n\n");
			return;
		}
	}

	analyzer_wrapper::find_reference_one_value(base, end - base, ref_map, nullptr, false);
	std::multimap<unsigned long long, unsigned long long>::iterator ref_map_it = ref_map.begin();
	unsigned char str_dump[1024];
	unsigned long long address;
	for (ref_map_it; ref_map_it != ref_map.end(); ++ref_map_it)
	{
		memset(str_dump, 0, sizeof(str_dump));
		unsigned long long readn = engine->read_virtual_memory(ref_map_it->first, (unsigned char*)str_dump, 1024);
		if (readn == 0)
		{
			continue;
		}

		if (helper::is_ascii(str_dump, 512))
		{
			size_t l = strlen((char *)str_dump);

			address = ref_map_it->second;
			char str[1024] = { 0, };
			Disasm(&address, str, false);

			char comment_str[1024];
			StringCbCopyA(comment_str, strlen(str), str);

			dprintf("%s	; \"", comment_str), print_ascii(str_dump, l), dprintf("\"\n");
		}

		if (helper::is_unicode(str_dump, 512))
		{
			size_t l = wcslen((wchar_t *)str_dump);

			address = ref_map_it->second;
			char str[1024] = { 0, };
			Disasm(&address, str, false);

			char comment_str[1024];
			StringCbCopyA(comment_str, strlen(str), str);

			dprintf("%s	; \"", comment_str), print_unicode(str_dump, l), dprintf("\"\n");
		}
	}
}

EXT_CLASS_COMMAND(WindbgEngine, refexe, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{p;ed,o;p;;}") // ref exe memory
{
	std::shared_ptr<engine::linker> engine;
	if (!engine::create<engine_linker>(engine))
	{
		return;
	}

	unsigned long long ptr = 0;
	unsigned long long base = 0;
	unsigned long long end = 0;
	std::multimap<unsigned long long, unsigned long long> ref_map;

	if (HasArg("b") && HasArg("l"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
	}
	else if (HasArg("p"))
	{
		ptr = GetArgU64("p", FALSE);
		base = 0;
		end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			dprintf("%I64x is not exe memory\n", ptr);
			dprintf("0:000> !refexe -b xxxx -l xxxx -p xxxx\n\n");
			return;
		}
	}

	analyzer_wrapper::find_reference_one_value(base, end - base, ref_map, nullptr, false);
	std::multimap<unsigned long long, unsigned long long>::iterator ref_map_it = ref_map.begin();
	for (ref_map_it; ref_map_it != ref_map.end(); ++ref_map_it)
	{
		MEMORY_BASIC_INFORMATION64 mbi = { 0, };
		if (!engine->query_virtual(ref_map_it->first, &mbi))
		{
			continue;
		}

		if (!HasArg("d"))
		{
			if (mbi.State == MEM_COMMIT && (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY))
			{
				if (analyzer_wrapper::check(base, end, ref_map_it->first))
				{
					continue;
				}

				char symbol[256] = { 0, };
				unsigned long long disp = 0;
				GetSymbol(ref_map_it->first, symbol, &disp);

				char str[1024] = { 0, };
				unsigned long long address = ref_map_it->second;
				Disasm(&address, str, false);

				char comment_str[1024];
				StringCbCopyA(comment_str, strlen(str), str);

				if (disp)
				{
					dprintf("%s	; %s+0x%x\n", comment_str, symbol, disp);
				}
				else
				{
					dprintf("%s	; %s\n", comment_str, symbol);
				}
			}
		}
		else
		{
			if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE && (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY))
			{
				char str[1024] = { 0, };
				unsigned long long address = ref_map_it->second;
				Disasm(&address, str, false);

				char comment_str[1024];
				StringCbCopyA(comment_str, strlen(str), str);

				dprintf("%s\n", comment_str);
			}
		}
	}
}

//
// find call & jmp code
// 
bool is_code(unsigned long long base, unsigned long long size, std::multimap<unsigned long long, unsigned long long> ref_map)
{
	unsigned long count = 0;
	unsigned long long end = base + size;
	std::multimap<unsigned long long, unsigned long long>::iterator ref_map_it = ref_map.begin();

	for (ref_map_it; ref_map_it != ref_map.end(); ++ref_map_it)
	{
		if (!analyzer_wrapper::check(base, end, ref_map_it->first))
		{
			continue;
		}

		count = (unsigned long)ref_map.count((unsigned long)ref_map_it->first);
		if (count >= 3)
		{
			return true;
		}
	}

	return false;
}

EXT_CLASS_COMMAND(WindbgEngine, vmcode, "", "{b;ed,o;b;;}" "{l;ed,o;l;;}" "{ref;b,o;ref;;}")
{
	unsigned long long ptr = GetArgU64("p", FALSE);
	unsigned long long base = 0;
	unsigned long long end = 0;

	if (HasArg("b") && HasArg("l"))
	{
		base = GetArgU64("b", FALSE);
		end = GetArgU64("l", FALSE) + base;
	}
	else
	{
		return;
	}

	bool is_ref_check = HasArg("ref");

	dprintf("base=>%I64x, end=%I64x\n", base, base+end);
	std::map<unsigned long long, unsigned long long> page_map;
	do
	{
		MEMORY_BASIC_INFORMATION64 mbi = { 0, };
		if (g_Ext->m_Data2->QueryVirtual(base, &mbi) == S_OK)
		{
			dprintf("page %I64x\r", base);

			base = mbi.BaseAddress + mbi.RegionSize;
			if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE)
			{
				unsigned long long offset = mbi.BaseAddress - mbi.AllocationBase;

				page_map[mbi.BaseAddress] = mbi.RegionSize;
			}
		}
		else
		{
			break;
		}
	} while (base < end);
	dprintf("\r");

	//
	//
	//
	std::shared_ptr<engine::linker> engine;
	if (!engine::create<engine_linker>(engine))
	{
		return;
	}

	std::map<unsigned long long, unsigned long long>::iterator page_map_it = page_map.begin();
	for (page_map_it; page_map_it != page_map.end(); ++page_map_it)
	{
		dprintf(" [*] base=%0*I64x size=%0*I64x\r", 16, page_map_it->first, 16, page_map_it->second);
		if (page_map_it->second > 0x3fffff) // 동작 속도 최적화
		{
			continue;
		}

		bool is_pe = false;
		std::multimap<unsigned long long, unsigned long long> call_code_map;
		analyzer_wrapper::find_reference_one_value(page_map_it->first, page_map_it->second, call_code_map, &is_pe, true);

		if (is_pe)
		{
			dprintf(" [-] base=%0*I64x size=%0*I64x	[pe]", 16, page_map_it->first, 16, page_map_it->second);
		}

		if (call_code_map.size() == 0)
		{
			if (is_pe)
			{
				dprintf("\n");
			}
			continue;
		}

		if (is_code(page_map_it->first, page_map_it->second, call_code_map))
		{
			dprintf("\r");
			char cmd[500] = { 0, };
			sprintf(cmd, ".browse !refexe -b %I64x -l %I64x", page_map_it->first, page_map_it->second);
			dprintf(" "), g_Ext->DmlCmdExec("[1]", cmd);

			memset(cmd, 0, 500);
			sprintf(cmd, ".browse !refstr -b %I64x -l %I64x", page_map_it->first, page_map_it->second);
			g_Ext->DmlCmdExec("[2]", cmd);

			dprintf(" base=%0*I64x size=%0*I64x	", 16, page_map_it->first, 16, page_map_it->second);

			if (is_pe)
			{
				dprintf("[pe] ");
			}

			MEMORY_BASIC_INFORMATION64 mbi;
			if (engine->virtual_query(page_map_it->first, &mbi))
			{
				bool p = (mbi.Protect == PAGE_EXECUTE
					|| mbi.Protect == PAGE_EXECUTE_READ
					|| mbi.Protect == PAGE_EXECUTE_READWRITE
					|| mbi.Protect == PAGE_EXECUTE_WRITECOPY);
				if (!p)
				{
					dprintf("[rw] ");
				}
				else
				{
					dprintf("[exe] ");
				}
			}

			dprintf("[code]\n");
		}
		else if (is_pe)
		{
			dprintf("\n");
		}
	}

	dprintf("%*c\r", 50, ' ');
	dprintf("=====================================================\n");
}

//
// 루틴 추적기
//
EXT_CLASS_COMMAND(WindbgEngine, ut, "", "{;ed,o;ptr;;}" "{entry;b,o;entry;;}")
{
	unsigned long n = GetNumUnnamedArgs();
	if (n == 0)
	{
		return;
	}

	unsigned long long ptr = GetUnnamedArgU64(0);
	std::shared_ptr<engine::linker> engine;
	if (!engine::create<engine_linker>(engine))
	{
		return;
	}

	unsigned long long base = 0;
	unsigned long long end = 0;

	if (HasArg("entry"))
	{
		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (analyzer_wrapper::check(base, end, ptr - 0x500)) //
		{
			base = ptr - 0x500;
		}

		//
		// jmp to current ptr 
		//
		dprintf("\n");
		Dml("<link name=\"TOP\"> ; go to </link>");
		Dml("<link section=\"%I64x\">%I64x\n</link>", ptr, ptr);

		ptr = analyzer_wrapper::find_entry(ptr, base, end - base);
		if (ptr == 0)
		{
			return;
		}

		//
		// xref to
		//
		dprintf(" ; xref ");
		std::list<unsigned long long> l;
		analyzer_wrapper::find_caller(ptr, base, end - base, l);
		std::list<unsigned long long>::iterator it = l.begin();
		if (l.size())
		{
			int i = 0;
			for (it; it != l.end(); ++it, ++i)
			{
				if (i % 16 == 0)
				{
					dprintf("\n   ");
				}

				Dml("<link cmd=\"!ut %I64x -entry\">%I64x</link>", *it, *it), dprintf(" ");
			}
			dprintf("▲\n");
		}
		else
		{
			dprintf(" nop\n");
		}

		dprintf("\n");
	}
	else
	{
		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);

		//
		//
		//
		dprintf("\n");
		Dml("<link name=\"TOP\"> ; go to </link>");
		Dml("<link section=\"%I64x\">%I64x\n</link>", ptr, ptr);

		//
		// xref to
		//
		dprintf(" ; xref ");
		std::list<unsigned long long> l;
		analyzer_wrapper::find_caller(ptr, base, end - base, l);
		std::list<unsigned long long>::iterator it = l.begin();
		if (l.size())
		{
			int i = 0;
			for (it; it != l.end(); ++it, ++i)
			{
				if (i % 16 == 0)
				{
					dprintf("\n   ");
				}

				Dml("<link cmd=\"!ut %I64x -entry\">%I64x</link>", *it, *it), dprintf(" ");
			}
			dprintf("▲\n");
		}
		else
		{
			dprintf(" nop\n");
		}

		dprintf("\n");
	}

	analyzer an(base, end - base);
	unsigned char *memory_dump = an.alloc(engine);
	if (!memory_dump)
	{
		return;
	}
	std::shared_ptr<void> memory_dump_closer(memory_dump, free);

	std::set<unsigned long long> visited;
	analyzer::block *b = new analyzer::block;
	memset(b->tag, 0, sizeof(b->tag));

	an.trace(engine, ptr, memory_dump, visited, *b);

	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_b = b->address_map.begin();
	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_e = b->address_map.end();
	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_t = b->address_map.end();

	end = (--address_detail_map_t)->first + 0x16;

	std::multimap<unsigned long long, unsigned long long> ref_map;
	analyzer_wrapper::find_reference_value(ptr, end - ptr, ref_map);

	for (address_detail_map_b; address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
	{
		std::multimap<unsigned long long, unsigned long long>::iterator fi = ref_map.find(address_detail_map_b->first);
		if (fi != ref_map.end())
		{
			dprintf("\n");
			std::pair<std::multimap<unsigned long long, unsigned long long>::iterator, std::multimap<unsigned long long, unsigned long long>::iterator> p;
			p = ref_map.equal_range(address_detail_map_b->first);

			dprintf(" ; ");
			std::multimap<unsigned long long, unsigned long long>::iterator range_it = p.first;
			int i = 0;
			for (range_it; range_it != p.second; ++range_it, ++i)
			{
				if (i == 16)
				{
					dprintf("\n   ");
					i = 0;
				}

				Dml("<link section=\"%I64x\">%I64x</link>", range_it->second, range_it->second);
				dprintf(" ");
			}
			dprintf("▲\n");
		}

		//
		//
		//
		char d[1024] = { 0, };
		unsigned long size_of_disasm = 0;
		unsigned long long next_address = 0;
		engine->disasm(address_detail_map_b->first, d, sizeof(d), &size_of_disasm, &next_address);

		if (address_detail_map_b->second->instruction_id == X86_INS_CALL && address_detail_map_b->second->operand_count == 1)
		{
			char comment_str[1024];
			StringCbCopyA(comment_str, strlen(d), d);

			//
			// link next func
			// 
			if (address_detail_map_b->second->operands[0].operand_type == X86_OP_IMM)
			{
				char cmd[500] = { 0, };
				sprintf(cmd, "!ut %I64x", address_detail_map_b->second->operands[0].value);

				Dml("<link name=\"%I64x\"> %s ; </link>", address_detail_map_b->first, comment_str), g_Ext->DmlCmdLink("call branch", cmd), dprintf("\n");
			}

			//
			// print symbol
			// 
			else if (address_detail_map_b->second->operands[0].operand_type == X86_OP_MEM)
			{
				unsigned long long value = 0;
				unsigned long r = engine->read_virtual_memory(address_detail_map_b->second->operands[0].value, (unsigned char *)&value, sizeof(unsigned long));

				if (r == sizeof(unsigned long))
				{
					char symbol[256] = { 0, };
					unsigned long long disp = 0;
					GetSymbol(value, symbol, &disp);

					char comment_str[1024];
					StringCbCopyA(comment_str, strlen(d), d);

					if (strlen(symbol))
					{
						if (disp)
						{
							Dml("<link name=\"%I64x\"> %s ; %s+0x%x\n</link>", address_detail_map_b->first, comment_str, symbol, disp);
						}
						else
						{
							Dml("<link name=\"%I64x\"> %s ; %s\n</link>", address_detail_map_b->first, comment_str, symbol);
						}
					}
					else
					{
						Dml("<link name=\"%I64x\"> %s ; unknown\n</link>", address_detail_map_b->first, comment_str);
					}
				}
				else
				{
					Dml("<link name=\"%I64x\"> %s</link>", address_detail_map_b->first, d);
				}
			}
			else
			{
				Dml("<link name=\"%I64x\"> %s</link>", address_detail_map_b->first, d);
			}
		}
		else
		{
			if (address_detail_map_b->second->is_jmp_code 
				&& address_detail_map_b->second->operand_count == 1 
				&& address_detail_map_b->second->operands[0].operand_type == X86_OP_IMM)
			{
				char comment_str[1024];
				StringCbCopyA(comment_str, strlen(d), d);

				//
				// 난독화 분석을 위해..
				//
				if (!analyzer_wrapper::check(base, end, address_detail_map_b->second->operands[0].value))
				{
					char cmd[500] = { 0, };
					sprintf(cmd, "!ut %I64x -entry", address_detail_map_b->second->operands[0].value);
					
					Dml("<link name=\"%I64x\"> %s ; </link>", address_detail_map_b->first, comment_str), g_Ext->DmlCmdLink("jump branch", cmd), dprintf("\n");
				}
				else
				{
					Dml("<link name=\"%I64x\"> %s ; </link>", address_detail_map_b->first, comment_str);
					Dml("<link section=\"%I64x\">%s</link>", address_detail_map_b->second->operands[0].value, "jump branch\n");
				}
			}
			else if (address_detail_map_b->second->instruction_id == X86_INS_INT3)
			{
				char comment_str[1024];
				StringCbCopyA(comment_str, strlen(d), d);

				Dml("<link name=\"%I64x\"> %s</link>", address_detail_map_b->first, comment_str);
				dprintf(" ; debug trap, ");
				Dml("<link cmd=\"!ut %I64x\">next\n</link>", next_address);
			}
			else
			{
				Dml("<link name=\"%I64x\"> %s</link>", address_detail_map_b->first, d);
			}
		}

		//
		//
		//
		if (address_detail_map_b->second->is_jmp_code)
		{
			address_detail_map_t = address_detail_map_b;
			++address_detail_map_t;

			fi = ref_map.find(address_detail_map_t->first);
			if (fi == ref_map.end())
			{
				dprintf("\n");
			}
		}
	}
	dprintf("\n");

	dprintf(" "), Dml("<link cmd=\"!graph -p %I64x\">[graph]</link>", ptr);
	dprintf(" "), Dml("<link cmd=\"!refstr -b %I64x -l %I64x\">[ref str]</link>", ptr, end - ptr);
	dprintf(" "), Dml("<link cmd=\"!refexe -b %I64x -l %I64x\">[ref str]</link>", ptr, end - ptr);
	dprintf(" "), Dml("<link section=\"TOP\">%s</link>", "[top]\n");
	dprintf("\n");

	analyzer::free(b);
	::free(b);
}
