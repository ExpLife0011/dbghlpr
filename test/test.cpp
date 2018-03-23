#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <conio.h>
#include <strsafe.h>

#pragma comment( lib, "ntdll.lib" )

VOID WINAPI Logv()
{
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle(L"logv");
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE)
			, FOREGROUND_RED
			| FOREGROUND_GREEN
			| FOREGROUND_BLUE
		);
	}
}

void link_patch(wchar_t *dll_name)
{
	PTEB teb = NtCurrentTeb();
	PPEB peb = teb->ProcessEnvironmentBlock;

	PPEB_LDR_DATA ldr = peb->Ldr;
	LIST_ENTRY le = ldr->InMemoryOrderModuleList;
	LIST_ENTRY *le2 = (LIST_ENTRY *)((unsigned long)le.Blink - 8);

	while (1)
	{
		PLDR_DATA_TABLE_ENTRY ldte = (PLDR_DATA_TABLE_ENTRY)le2;
		if (ldte->FullDllName.Buffer == nullptr)
			break;

		int n = wcslen(ldte->FullDllName.Buffer);

		if (wcsstr(ldte->FullDllName.Buffer, dll_name))
		{
			LIST_ENTRY *f = le2->Flink;
			LIST_ENTRY *b = le2->Blink;

			f->Blink = b;
			b->Flink = f;

			break;
		}

		le2 = le2->Blink;
	}
}

void main()
{
	HMODULE module_handle = LoadLibrary(L"test.dll");
	if (module_handle)
	{
		printf("loaded test module\n");
		link_patch(L"test.dll");
	}

	static wchar_t *u_str = L"unicode test string\n";
	static char *a_str = "ascii test string";

	MessageBoxA(nullptr, a_str, "msgboxa", MB_OK);
	MessageBoxW(nullptr, u_str, L"msgboxw", MB_OK);

	int begin = GetTickCount();
	for(int i = 0; i<5; ++i)
	{
		printf("step %d\n", i);
		Sleep(1000);
	}
	printf("test end\n");
	int end = GetTickCount();

	printf("count: %d\n", end-begin);
	_getch();
}
