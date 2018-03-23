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

//
// segment
//
EXT_CLASS_COMMAND(WindbgEngine, create, "", "{p;ed,o;p;;}")
{
	unsigned long long ptr = GetArgU64("p", FALSE);
	unsigned long long base = 0;
	unsigned long long end = 0;
	analyzer_wrapper::calc_exe_segment(ptr, &base, &end);

	std::map<unsigned long long, segment_descriptor *> segment_descriptor_table = analyzer_wrapper::get_segment_descriptor_table();
	std::map<unsigned long long, segment_descriptor *>::iterator segment_descriptor_table_it = segment_descriptor_table.find(base);

	if (segment_descriptor_table_it != segment_descriptor_table.end())
	{
		dprintf("	[-] found segment descriptor\n\n");
		return;
	}

	HANDLE thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)analyzer_wrapper::create_segment_descriptor, &ptr, 0, nullptr);
	WaitForSingleObject(thread_handle, 500);
}
