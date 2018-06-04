#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4819)

#include <Windows.h>
#include <TlHelp32.h>
#include <winternl.h>

#include <stdio.h>
#include <conio.h>
#include <memory>

#include <asm.h>

//
unsigned long __stdcall alignment(unsigned long region_size, unsigned long image_aligin);
int keystone_patch(int argc, char *argv[])
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
	sprintf((char *)add_section.Name, ".ldr");
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
				printf(" - kernel32 f\n");
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

	unsigned long new_fp = section_headers[add_idx].PointerToRawData + (unsigned long)strlen(&dump[path_idx]) + 1;
	printf(" - new file point=%08x\n\n", new_fp);

	if (loadliblary == 0) // patch int
	{
		printf(" loadliblary not found\n");
		unsigned long long ldr_offset = (unsigned long long)LoadLibraryA - (unsigned long long)GetModuleHandle(L"KERNEL32.DLL");

		return 0;
	}

	//
	unsigned char code_tmp[128] = { 0, };
	size_t idx = 0;
	assembler::x86 cpu_x86;

	unsigned char code_64_tmp[128] = { 0, };
	size_t idx_64 = 0;
	assembler::x64 cpu_x64;

	//
	auto x86_asm_code = [&cpu_x86, &idx, &code_tmp](char *format, ...)->bool
	{
		char mnemonic[512] = { 0, };
		va_list ap;
		va_start(ap, format);
		vsprintf(mnemonic, format, ap);
		va_end(ap);

		printf(" [+] asm=> %s	", mnemonic);

		unsigned char asm_code[16] = { 0, };
		size_t size = 0;

		if (!assembler::assemble(&cpu_x86, mnemonic, asm_code, &size))
		{
			printf("\n	[-] fail\n");
			return false;
		}
		printf("\n");

		memcpy(&code_tmp[idx], asm_code, size);
		idx += size;

		return true;
	};

	auto x64_asm_code = [&cpu_x64, &idx_64, &code_64_tmp](char *format, ...)->bool
	{
		char mnemonic[512] = { 0, };
		va_list ap;
		va_start(ap, format);
		vsprintf(mnemonic, format, ap);
		va_end(ap);

		printf(" [+] asm=> %s	", mnemonic);

		unsigned char asm_code[16] = { 0, };
		size_t size = 0;

		if (!assembler::assemble(&cpu_x64, mnemonic, asm_code, &size))
		{
			printf("\n	[-] fail\n");
			return false;
		}
		printf("\n");

		memcpy(&code_64_tmp[idx_64], asm_code, size);
		idx_64 += size;

		return true;
	};

	//
#ifdef _WIN64
#define X64_CODE_ASM(str, ...)	if (!x64_asm_code(str, __VA_ARGS__))	return false
	X64_CODE_ASM("push rax");
	X64_CODE_ASM("push rbx");
	X64_CODE_ASM("push rcx");
	X64_CODE_ASM("push rdx");
	X64_CODE_ASM("push rsi");
	X64_CODE_ASM("push rdi");
	X64_CODE_ASM("nop");

	X64_CODE_ASM("mov rax,rcx");
	X64_CODE_ASM("mov rdx,rax");
	X64_CODE_ASM("add rdx,0x%x", loadliblary);
	X64_CODE_ASM("nop");

	X64_CODE_ASM("mov rcx,rax");
	X64_CODE_ASM("add rcx,0x%x", section_headers[add_idx].VirtualAddress);
	X64_CODE_ASM("nop");

	X64_CODE_ASM("call qword ptr ds:[rdx]");
	X64_CODE_ASM("nop");

	X64_CODE_ASM("pop rax");
	X64_CODE_ASM("pop rbx");
	X64_CODE_ASM("pop rcx");
	X64_CODE_ASM("pop rdx");
	X64_CODE_ASM("pop rsi");
	X64_CODE_ASM("pop rdi");

	X64_CODE_ASM("mov rax,rcx");
	X64_CODE_ASM("add rax,0x%x", oep);
	X64_CODE_ASM("nop");

	X64_CODE_ASM("jmp rax");

	printf("	[ code 64 ]\n");
	for (int i = 0; i < sizeof(code_64_tmp); ++i)
	{
		if (i % 16 == 0)
			printf("\n	");
		printf("%02x ", code_64_tmp[i]);
	}
	printf("\n\n");

	memcpy(&dump[new_fp], code_64_tmp, sizeof(code_64_tmp));
#else
#define X86_CODE_ASM(str, ...)	if (!x86_asm_code(str, __VA_ARGS__))	return false

	X86_CODE_ASM("pushad");
	X86_CODE_ASM("xor ebp,ebp");
	X86_CODE_ASM("mov ebp,esp");
	X86_CODE_ASM("add ebp,0x24");
	X86_CODE_ASM("mov ebp,dword ptr ss:[ebp]");
	X86_CODE_ASM("mov edx,ebp");
	X86_CODE_ASM("add edx,0x%x", loadliblary); // offset LoadLibraryA
	X86_CODE_ASM("nop");

	X86_CODE_ASM("xor ecx,ecx");
	X86_CODE_ASM("mov ecx,ebp");
	X86_CODE_ASM("add ecx,0x%x", section_headers[add_idx].VirtualAddress); // new ldr section offset
	X86_CODE_ASM("nop");

	X86_CODE_ASM("push ecx");
	X86_CODE_ASM("call dword ptr ds:[edx]");
	X86_CODE_ASM("nop");

	X86_CODE_ASM("mov ecx,ebp");
	X86_CODE_ASM("add ecx,0x%x", oep); // oep offset
	X86_CODE_ASM("nop");

	X86_CODE_ASM("mov edx,ebp");
	X86_CODE_ASM("add edx,0x%x", section_headers[add_idx].VirtualAddress); // new ldr section offset 
	X86_CODE_ASM("nop");

	X86_CODE_ASM("mov dword ptr ds:[edx],ecx");
	X86_CODE_ASM("popad");

	X86_CODE_ASM("mov eax,esp");
	X86_CODE_ASM("add eax,0x4");
	X86_CODE_ASM("mov eax,dword ptr ds:[eax]");
	X86_CODE_ASM("jmp dword ptr ds:[eax+0x%x]", section_headers[add_idx].VirtualAddress); // new ldr section offset

	printf("	[ code 32 ]\n");
	for (int i = 0; i < sizeof(code_tmp); ++i)
	{
		if (i % 16 == 0)
			printf("\n	");
		printf("%02x ", code_tmp[i]);
	}
	printf("\n\n");

	memcpy(&dump[new_fp], code_tmp, sizeof(code_tmp));
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