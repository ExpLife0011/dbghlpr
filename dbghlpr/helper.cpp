#include <Windows.h>
#include <TlHelp32.h>
#include <strsafe.h>
#include <stdio.h>
#include <helper.h>

unsigned long long __stdcall setup(void *args)
{
	ldr_package_ptr ldr = (ldr_package_ptr)args;
	unsigned long long delta = (unsigned long long)ldr->image_base - ldr->nt_header->OptionalHeader.ImageBase;
	///
	/// set reloc
	///
	PIMAGE_BASE_RELOCATION base_reloc = ldr->base_reloc;
	while (base_reloc->VirtualAddress)
	{
		if (base_reloc->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION))
		{
			unsigned long long count = (base_reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(unsigned short);
			unsigned short *list = (unsigned short *)(base_reloc + 1);
			unsigned long long *ptr = nullptr;

			for (unsigned int i = 0; i < count; ++i)
			{
				if (list[i])
				{
					ptr = (unsigned long long *)((unsigned long long)ldr->image_base + (base_reloc->VirtualAddress + (list[i] & 0xFFF)));
					*ptr += delta;
				}
			}
		}

		base_reloc = (PIMAGE_BASE_RELOCATION)((unsigned long long)base_reloc + base_reloc->SizeOfBlock);
	}
	///
	/// set import descriptor
	///
	PIMAGE_IMPORT_DESCRIPTOR import_desc = ldr->import_dir;
	while (import_desc->Characteristics)
	{
		PIMAGE_THUNK_DATA origin_first_dunk = (PIMAGE_THUNK_DATA)((unsigned long long)ldr->image_base + import_desc->OriginalFirstThunk);
		PIMAGE_THUNK_DATA first_dunk = (PIMAGE_THUNK_DATA)((unsigned long long)ldr->image_base + import_desc->FirstThunk);
		HMODULE module_handle = ldr->load_library((LPCSTR)ldr->image_base + import_desc->Name);

		if (!module_handle)
		{
			return 0;
		}

		while (origin_first_dunk->u1.AddressOfData)
		{
#ifdef _WIN64
			unsigned long long func = (unsigned long long)ldr->get_proc_address(module_handle, (LPCSTR)(origin_first_dunk->u1.Ordinal & 0xFFFF));
#else
			unsigned long func = (unsigned long)ldr->get_proc_address(module_handle, (LPCSTR)(origin_first_dunk->u1.Ordinal & 0xFFFF));
#endif
			if (origin_first_dunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				func = (unsigned long long)ldr->get_proc_address(module_handle, (LPCSTR)(origin_first_dunk->u1.Ordinal & 0xFFFF));
				if (!func)
				{
					return 0;
				}

				first_dunk->u1.Function = func;
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME import_by_name = (PIMAGE_IMPORT_BY_NAME)((unsigned long long)ldr->image_base + origin_first_dunk->u1.AddressOfData);
				func = (unsigned long long)ldr->get_proc_address(module_handle, (LPCSTR)import_by_name->Name);
				if (!func)
				{
					return 0;
				}

				first_dunk->u1.Function = func;
			}

			++origin_first_dunk;
			++first_dunk;
		}

		++import_desc;
	}
	///
	/// call main
	///
	if (ldr->nt_header->OptionalHeader.AddressOfEntryPoint)
	{
		DllMainT entry_point = (DllMainT)((unsigned long long)ldr->image_base + ldr->nt_header->OptionalHeader.AddressOfEntryPoint);

		return entry_point((HMODULE)ldr->image_base, DLL_PROCESS_ATTACH, &ldr->args);
	}

	return 0;
}
unsigned long long __stdcall setup_end_point(void *args) { return 0; }

bool __stdcall install(wchar_t *module_name, unsigned long pid, module_load_information_type_ptr out_module_info, DLL_ARGS dll_args)
{
	if (!module_name)
	{
		return false;
	}

	HANDLE token_handle = nullptr;
	TOKEN_PRIVILEGES tp;
	if (OpenProcessToken((HANDLE)-1, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle))
	{
		memset(&tp, 0, sizeof(tp));
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		tp.Privileges[0].Luid.LowPart = 20;
		tp.Privileges[0].Luid.HighPart = 0;

		AdjustTokenPrivileges(token_handle, FALSE, &tp, 0, NULL, NULL);
		CloseHandle(token_handle);
	}

	HANDLE module_file_handle = CreateFile(module_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (module_file_handle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	unsigned long module_file_size = GetFileSize(module_file_handle, NULL);
	void *buffer = VirtualAlloc(NULL, module_file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!buffer)
	{
		CloseHandle(module_file_handle);
		return false;
	}

	unsigned long readn = 0;
	if (!ReadFile(module_file_handle, buffer, module_file_size, &readn, NULL))
	{
		CloseHandle(module_file_handle);
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		return false;
	}
	CloseHandle(module_file_handle);

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)buffer;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		return false;
	}

	PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)((LPBYTE)buffer + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		return false;
	}

	HANDLE process_handle = OpenProcess(MAXIMUM_ALLOWED, FALSE, pid);
	if (!process_handle)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		return false;
	}

	void *module_image = VirtualAllocEx(process_handle, NULL, nt_header->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!module_image)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	if (!WriteProcessMemory(process_handle, module_image, buffer, nt_header->OptionalHeader.SizeOfHeaders, NULL))
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	PIMAGE_SECTION_HEADER section_header = (PIMAGE_SECTION_HEADER)(nt_header + 1);
	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i)
	{
		if (!WriteProcessMemory(process_handle, (PVOID)((LPBYTE)module_image + section_header[i].VirtualAddress), (PVOID)((LPBYTE)buffer + section_header[i].PointerToRawData), section_header[i].SizeOfRawData, NULL))
		{
			VirtualFree(buffer, module_file_size, MEM_RELEASE);
			VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
			CloseHandle(process_handle);

			return false;
		}
	}

	void *loader_image = VirtualAllocEx(process_handle, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!loader_image)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	ldr_package ldr;
	memset(&ldr, 0, sizeof(ldr));

	ldr.image_base = module_image;
	ldr.nt_header = (PIMAGE_NT_HEADERS)((LPBYTE)module_image + dos_header->e_lfanew);
	ldr.base_reloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)module_image + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	ldr.import_dir = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)module_image + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	ldr.load_library = LoadLibraryA;
	ldr.get_proc_address = GetProcAddress;
	memcpy(&ldr.args, &dll_args, sizeof(dll_args));

	if (!WriteProcessMemory(process_handle, loader_image, &ldr, sizeof(ldr_package), NULL))
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
		VirtualFreeEx(process_handle, loader_image, 4096, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	if (!WriteProcessMemory(process_handle, (PVOID)((ldr_package_ptr)loader_image + 1), setup, (unsigned long long)setup_end_point - (unsigned long long)setup, NULL))
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
		VirtualFreeEx(process_handle, loader_image, 4096, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)((ldr_package_ptr)loader_image + 1), loader_image, 0, NULL);
	if (!thread_handle)
	{
		VirtualFree(buffer, module_file_size, MEM_RELEASE);
		VirtualFreeEx(process_handle, module_image, nt_header->OptionalHeader.SizeOfImage, MEM_RELEASE);
		VirtualFreeEx(process_handle, loader_image, 4096, MEM_RELEASE);
		CloseHandle(process_handle);

		return false;
	}

	out_module_info->target_process_handle = process_handle;
	out_module_info->main_thread_handle = thread_handle;

	out_module_info->loader_address = loader_image;
	out_module_info->size_of_loader = 4096;

	out_module_info->module_load_address = module_image;
	out_module_info->size_of_module_image = nt_header->OptionalHeader.SizeOfImage;

	VirtualFree(buffer, module_file_size, MEM_RELEASE);

	return true;
}
//
//
//
bool __stdcall process_name_to_pid(wchar_t *process_name, unsigned long *pid)
{
	PROCESSENTRY32 process_block32 = { 0, };
	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (!snapshot_handle)
		return false;

	process_block32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot_handle, &process_block32))
	{
		CloseHandle(snapshot_handle);
		return false;
	}

	do
	{
		if (wcsstr(process_block32.szExeFile, process_name))
		{
			*pid = process_block32.th32ProcessID;
			CloseHandle(snapshot_handle);

			return true;
		}
	} while (Process32Next(snapshot_handle, &process_block32));

	CloseHandle(snapshot_handle);
	return false;
}

unsigned long wait(wchar_t *process_name)
{
	unsigned long pid = 0;

	while (INFINITE)
	{
		if (process_name_to_pid(process_name, &pid))
		{
			break;
		}
	}

	return pid;
}

//
// 
//
bool __stdcall helper::suspend(unsigned long pid)
{
	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;
	DWORD s = 0;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (!h_snapshot)
		return FALSE;

	thread_block32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(h_snapshot, &thread_block32))
		return FALSE;

	do
	{
		if (thread_block32.th32OwnerProcessID == pid)
		{
			h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, thread_block32.th32ThreadID);

			do
			{
				s = SuspendThread(h_thread);
			} while (!s);
			CloseHandle(h_thread);
		}
	} while (Thread32Next(h_snapshot, &thread_block32));

	return TRUE;
}

bool __stdcall helper::resume(unsigned long pid)
{
	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;
	DWORD s = 0;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (!h_snapshot)
		return FALSE;

	thread_block32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(h_snapshot, &thread_block32))
		return FALSE;

	do
	{
		if (thread_block32.th32OwnerProcessID == pid)
		{
			h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, thread_block32.th32ThreadID);

			do
			{
				s = ResumeThread(h_thread);
			} while (s);

			CloseHandle(h_thread);
		}
	} while (Thread32Next(h_snapshot, &thread_block32));

	return TRUE;
}

unsigned char g_backup[1024];
int helper::patch(void *handle, unsigned long long ip)
{
	HANDLE process_handle = handle;
	unsigned long pid = GetProcessId(process_handle);
	memset(g_backup, 0, 1024);

	if (!suspend(pid))
	{
		SetLastError(0xE0000005);
		return false;
	}

	//
	//
	//
#ifdef _WIN64
	unsigned long long rw = 0;
#else
	unsigned long rw = 0;
#endif
	unsigned long old = 0;
	BOOL r = VirtualProtectEx(process_handle, (void *)ip, 1024, PAGE_EXECUTE_READWRITE, &old);
	if (r)
	{
		r = ReadProcessMemory(process_handle, (void *)ip, g_backup, 1024, &rw);
		if (!r || rw < 5)
		{
			r = false;
		}
		else
		{
			unsigned char p[3] = { 0x90, 0xEB, 0xFD };
			r = WriteProcessMemory(process_handle, (void *)ip, p, 3, &rw);
			if (!r || rw < 3)
			{
				r = false;
			}
		}

		VirtualProtectEx(process_handle, (void *)ip, 1024, old, &old);
	}

	//
	//
	//
	if (!resume(pid))
	{
		SetLastError(0xE0000006);
		return false;
	}

	return r;
}

unsigned long helper::check_thread(unsigned long pid, unsigned long long ip)
{
	THREADENTRY32 thread_block32 = { 0, };
	HANDLE h_snapshot = NULL;
	HANDLE h_thread = NULL;

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (!h_snapshot)
		return 0;

	thread_block32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(h_snapshot, &thread_block32))
		return 0;

	CONTEXT context;
	context.ContextFlags = CONTEXT_CONTROL;

	do
	{
		if (thread_block32.th32OwnerProcessID == pid)
		{
			h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, thread_block32.th32ThreadID);

			SuspendThread(h_thread);

			if (GetThreadContext(h_thread, &context))
			{
#ifndef _WIN64
				if (context.Eip == (unsigned long)ip || context.Eip == (unsigned long)ip + 1)
#else
				if (context.Rip == ip || context.Rip == ip + 1)
#endif
				{
					CloseHandle(h_thread);
					return thread_block32.th32ThreadID;
				}
			}

			ResumeThread(h_thread);
			CloseHandle(h_thread);
		}
	} while (Thread32Next(h_snapshot, &thread_block32));

	return 0;
}

int helper::restore(void *process_handle, unsigned long tid, unsigned long long ip)
{
#ifdef _WIN64
	unsigned long long rw = 0;
#else
	unsigned long rw = 0;
#endif
	unsigned long old = 0;
	BOOL r = VirtualProtectEx(process_handle, (void *)ip, 1024, PAGE_EXECUTE_READWRITE, &old);
	if (r)
	{
		unsigned char p[3] = { 0, };
		p[0] = g_backup[0];
		p[1] = g_backup[1];
		p[2] = g_backup[2];

		r = WriteProcessMemory(process_handle, (void *)ip, p, 3, &rw);
		if (!r || rw < 3)
		{
			r = false;
		}

		VirtualProtectEx(process_handle, (void *)ip, 1024, old, &old);

		//
		//
		//
		HANDLE h_thread = OpenThread(MAXIMUM_ALLOWED, FALSE, tid);
		CONTEXT context;
		context.ContextFlags = CONTEXT_CONTROL;

		GetThreadContext(h_thread, &context);
#ifndef _WIN64
		context.Eip = (unsigned long)ip;
#else
		context.Rip = ip;
#endif
		SetThreadContext(h_thread, &context);
	}

	return r;
}

//
//
//
bool helper::is_ascii(unsigned char *data, size_t max_len)
{
	size_t len = 0;
	for (char *p = (char *)data; *p; ++len, ++p)
	{
		if (len >= max_len)
			break;
	}

	if (len < 2 || len + 1 >= max_len)
	{
		return false;
	}

	for (size_t i = 0; i < len; ++i)
	{
		if (!isprint(data[i]) && !isspace(data[i]))
		{
			return false;
		}
	}

	return true;
}

bool helper::is_unicode(unsigned char *data, size_t max_len)
{
	size_t len = 0;
	for (wchar_t *p = (wchar_t *)data; *p; ++len, ++p)
	{
		if (len >= max_len)
			break;
	}

	if (len < 2 || len + 1 >= max_len)
	{
		return false;
	}

	for (size_t i = 0; i < len * 2; i += 2)
	{
		if (data[i + 1])
		{
			return false;
		}

		if (!isprint(data[i]) && !isspace(data[i]))
		{
			return false;
		}
	}

	return true;
}
