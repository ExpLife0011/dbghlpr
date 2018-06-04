#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4819)

#include <Windows.h>
#include <TlHelp32.h>
#include <winternl.h>

#include <stdio.h>
#include <conio.h>
#include <memory>

unsigned long __stdcall alignment(unsigned long region_size, unsigned long image_aligin);

int static_patch(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("argument[1] = target dll\n");
		printf("argument[2] = attack dll\n");
		printf("\n");
		printf("example> patch_dll target.dll dummy.dll");

		return 0;
	}

	printf(" - target dll=%s\n\n", argv[1]);
	HANDLE module_file_handle = CreateFileA(argv[1], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (module_file_handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	std::shared_ptr<void> module_file_handle_closer(module_file_handle, CloseHandle);
	DWORD file_size = GetFileSize(module_file_handle, NULL) + 0x400;

	void *buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!buffer)
	{
		return FALSE;
	}

	DWORD readn = 0;
	if (!ReadFile(module_file_handle, buffer, file_size, &readn, NULL))
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		return FALSE;
	}

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)buffer;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		VirtualFree(buffer, 0, MEM_RELEASE);

		return FALSE;
	}

	PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)((LPBYTE)buffer + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		return FALSE;
	}

	PIMAGE_SECTION_HEADER section_headers = (PIMAGE_SECTION_HEADER)((unsigned long long)nt_header + sizeof(IMAGE_NT_HEADERS));
	int add_idx = nt_header->FileHeader.NumberOfSections;

	IMAGE_SECTION_HEADER add_section;
	sprintf((char *)add_section.Name, ".loader");
	add_section.Misc.VirtualSize = nt_header->OptionalHeader.SectionAlignment;
	add_section.SizeOfRawData = nt_header->OptionalHeader.FileAlignment;
	add_section.VirtualAddress = section_headers[add_idx - 1].VirtualAddress + alignment(section_headers[add_idx - 1].Misc.VirtualSize, nt_header->OptionalHeader.SectionAlignment);
	add_section.PointerToRawData = section_headers[add_idx - 1].PointerToRawData + section_headers[add_idx - 1].SizeOfRawData;
	add_section.Characteristics = 0x20 | 0x20000000 | 0x40000000 | 0x80000000;

	memcpy(&section_headers[add_idx], &add_section, sizeof(IMAGE_SECTION_HEADER));
	nt_header->FileHeader.NumberOfSections += 1;
	nt_header->OptionalHeader.SizeOfImage += add_section.Misc.VirtualSize;

	//
	// get import name table and find LoadLibraryA
	//
	unsigned long loadliblary = 0;
	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i)
	{
		unsigned long va = section_headers[i].VirtualAddress;
		unsigned long ve = va + section_headers[i].Misc.VirtualSize;

		if (va <= nt_header->OptionalHeader.DataDirectory[1].VirtualAddress && ve >= nt_header->OptionalHeader.DataDirectory[1].VirtualAddress)
		{
			printf(" + import section => %s\n", section_headers[i].Name);

			unsigned long import_offset = nt_header->OptionalHeader.DataDirectory[1].VirtualAddress - va;
			unsigned long import_idx = nt_header->OptionalHeader.DataDirectory[1].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR);
			unsigned char *dump = (unsigned char *)buffer;
			unsigned long fo = import_offset + section_headers[i].PointerToRawData;

			PIMAGE_IMPORT_DESCRIPTOR id = (PIMAGE_IMPORT_DESCRIPTOR)&dump[fo];
			int kernel32_idx = 0;
			for (unsigned int j = 0; j < import_idx - 1; ++j)
			{
				unsigned long name_offset = id[j].Name - va;
				unsigned long fn = name_offset + section_headers[i].PointerToRawData;

				if (strstr((char*)&dump[fn], "KERNEL32") || strstr((char*)&dump[fn], "kernel32") || strstr((char*)&dump[fn], "Kernel32"))
				{
					kernel32_idx = j;
					break;
				}
			}

			if (id[kernel32_idx].OriginalFirstThunk == 0) // int가 존재해야 아래 패치를 진행할 수 있다.
			{
				return 0;
			}

			unsigned long thunk_offset = id[kernel32_idx].FirstThunk - va;
			unsigned long ft = thunk_offset + section_headers[i].PointerToRawData;

			do
			{
				unsigned long *data = (unsigned long *)&dump[ft];
				if (*data == 0)
				{
					break;
				}

				unsigned long name_offset = *data - va;
				unsigned long fn = name_offset + section_headers[i].PointerToRawData;

				PIMAGE_IMPORT_BY_NAME import_name_table = (PIMAGE_IMPORT_BY_NAME)&dump[fn];
				printf("	- import name %08x, %s\n", *data, import_name_table->Name);

				if (strstr(import_name_table->Name, "LoadLibraryA"))
				{
					loadliblary = ft - section_headers[i].PointerToRawData + section_headers[i].VirtualAddress;

					printf("		- found loadlibary, iat=%08x\n", loadliblary);
					break;
				}

				ft += sizeof(ft);
			} while (1);

			break;
		}
	}
	printf("\n");

	if (loadliblary == 0)
	{
		printf(" loadliblary not found\n");
		return 0;
	}

	//
	// get entry point and patch dll entry
	//
	unsigned long oep = nt_header->OptionalHeader.AddressOfEntryPoint;
	printf(" - dll oep=>%08x\n", oep);

	char *dump = (char *)buffer;
	add_idx = nt_header->FileHeader.NumberOfSections - 1;
	int path_idx = section_headers[add_idx].PointerToRawData;

	char temp_path[MAX_PATH] = { 0, };
	GetTempPathA(MAX_PATH, temp_path);
	sprintf(&dump[path_idx], "%s\\%s", temp_path, argv[2]);

	unsigned long new_ep = section_headers[add_idx].VirtualAddress + (unsigned long)strlen(&dump[path_idx]) + 1;
	printf(" - new entry point=%08x\n", new_ep);
	nt_header->OptionalHeader.AddressOfEntryPoint = new_ep;

	//
	// 64bit code
	//
	size_t code_idx = 0;
	unsigned char code_64_dump[128] = { 0, };
	char *code_tmp = (char *)code_64_dump;

	/*
	push rax
	push rbx
	push rcx
	push rdx
	; push rbp => nop
	; push rsp => nop
	push rsi
	push rdi
	nop
	mov rax,rcx
	nop
	mov rdx,rax
	add rdx,????
	*/
	sprintf(code_tmp, "\x50\x53\x51\x52\x56\x57\x90\x48\x83\xec\x28\x48\x89\xc8\x48\x89\xc2\x48\x81\xc2");
	code_idx = strlen(code_tmp);

	unsigned long *xxxx = (unsigned long *)&code_64_dump[code_idx];
	*xxxx = loadliblary;
	code_idx += sizeof(loadliblary);

	/*
	nop
	mov rcx,rax
	add rcx,????
	*/
	code_tmp = (char *)&code_64_dump[code_idx];
	sprintf(code_tmp, "\x90\x48\x81\xc1");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_64_dump[code_idx];
	*xxxx = section_headers[add_idx].VirtualAddress;
	code_idx += sizeof(section_headers[add_idx].VirtualAddress);

	/*
	nop
	call qword ptr ds:[rdx]
	nop
	pop rax
	pop rbx
	pop rcx
	pop rdx
	; pop rbp => nop
	; pop rsp => nop
	pop rsi
	pop rdi
	nop
	mov rax, rcx
	add rax, ????
	*/
	code_tmp = (char *)&code_64_dump[code_idx];
	sprintf(code_tmp, "\x90\xff\x12\x90\x48\x83\xc4\x28\x5f\x5e\x5a\x59\x5b\x58\x90\x48\x89\xc8\x48\x05");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_64_dump[code_idx];
	*xxxx = oep;
	code_idx += sizeof(oep);

	/*
	nop
	jmp rax
	*/
	code_tmp = (char *)&code_64_dump[code_idx];
	sprintf(code_tmp, "\x90\xff\xE0");
	code_idx += strlen(code_tmp);

	//
	// 32bit code
	//
	code_idx = 0;
	unsigned char code_32_dump[128] = { 0, };
	code_tmp = (char *)code_32_dump;
	sprintf(code_tmp, "\x60\x31\xed\x89\xe5\x83\xc5\x24\x8b\x6d");
	code_idx += strlen(code_tmp);

	code_tmp = (char *)&code_32_dump[code_idx];
	code_tmp[0] = 0;
	code_idx += 1;

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\x89\xea\x81\xc2");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_32_dump[code_idx];
	*xxxx = loadliblary;
	code_idx += sizeof(loadliblary);

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\x90\x31\xc9\x89\xe9\x81\xc1");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_32_dump[code_idx];
	*xxxx = section_headers[add_idx].VirtualAddress;
	code_idx += sizeof(section_headers[add_idx].VirtualAddress);

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\x90\x51\xff\x12\x90\x89\xe9\x81\xc1");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_32_dump[code_idx];
	*xxxx = oep;
	code_idx += sizeof(oep);

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\x90\x89\xea\x81\xc2");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_32_dump[code_idx];
	*xxxx = section_headers[add_idx].VirtualAddress;
	code_idx += sizeof(section_headers[add_idx].VirtualAddress);

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\x90\x89\x0a\x61\x89\xe0\x83\xc0\x04\x8b");
	code_idx += strlen(code_tmp);

	code_tmp = (char *)&code_32_dump[code_idx];
	code_tmp[0] = 0;
	code_idx += 1;

	code_tmp = (char *)&code_32_dump[code_idx];
	sprintf(code_tmp, "\xff\xa0");
	code_idx += strlen(code_tmp);

	xxxx = (unsigned long *)&code_32_dump[code_idx];
	*xxxx = section_headers[add_idx].VirtualAddress;
	code_idx += sizeof(section_headers[add_idx].VirtualAddress);

	//
	//
	//
	unsigned long new_fp = section_headers[add_idx].PointerToRawData + (unsigned long)strlen(&dump[path_idx]) + 1;
	printf(" - new file point=%08x\n\n", new_fp);

#ifdef _WIN64
	printf("	[ code 64 ]\n");
	for (int i = 0; i < sizeof(code_64_dump); ++i)
	{
		if (i % 16 == 0)
			printf("\n	");
		printf("%02x ", code_64_dump[i]);
	}
	printf("\n\n");

	memcpy(&dump[new_fp], code_64_dump, sizeof(code_64_dump));
#else
	printf("	[ code 32 ]\n");
	for (int i = 0; i < sizeof(code_32_dump); ++i)
	{
		if (i % 16 == 0)
			printf("\n	");
		printf("%02x ", code_32_dump[i]);
	}
	printf("\n\n");

	memcpy(&dump[new_fp], code_32_dump, sizeof(code_32_dump));
#endif

	//
	// write
	//
	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i)
	{
		printf(" - section name=%s:: ", section_headers[i].Name);
		printf("file ptr=%08x, va offset=%08x\n", section_headers[i].PointerToRawData, section_headers[i].VirtualAddress);
	}

	HANDLE out_file_handle = CreateFile(L"output.dll", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	if (out_file_handle == INVALID_HANDLE_VALUE)
	{
		printf("output hf\n");
		return 0;
	}
	std::shared_ptr<void> out_file_handle_closer(out_file_handle, CloseHandle);

	DWORD written = 0;
	if (!WriteFile(out_file_handle, buffer, file_size, &written, nullptr))
	{
		printf("output wf\n");
	}

	printf(" * patch %s\n\n", argv[1]);
	_getch();

	return 0;
}
