#include "Tools.h"

VOID WriteResourceTable(PBYTE lpResourceDataEntry, LPCWSTR lpResourceFileName)
{
	HANDLE hFile = 0;
	DWORD fileSize = 0;
	DWORD allocAddr = 0;
	DWORD oldProtect = 0;
	BOOL isProtect = FALSE;

	isProtect = VirtualProtectEx(GetCurrentProcess(), lpResourceDataEntry, sizeof(IMAGE_RESOURCE_DATA_ENTRY), PAGE_EXECUTE_READWRITE, &oldProtect);
	hFile = CreateFileW(lpResourceFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ((hFile != INVALID_HANDLE_VALUE) && isProtect)
	{
		fileSize = GetFileSize(hFile, NULL);
		allocAddr = (DWORD)VirtualAlloc(NULL, fileSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (allocAddr && ReadFile(hFile, (PBYTE)allocAddr, fileSize, NULL, NULL))
		{
			((IMAGE_RESOURCE_DATA_ENTRY*)lpResourceDataEntry)->OffsetToData = allocAddr;
			((IMAGE_RESOURCE_DATA_ENTRY*)lpResourceDataEntry)->Size = 1008611;
		}

		CloseHandle(hFile);
	}
}

VOID WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize)
{
	DWORD oldProtect = 0;
	BOOL isProtect = FALSE;
	BOOL isWritten = FALSE;

	isProtect = VirtualProtectEx(GetCurrentProcess(), lpAddress, nSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	isWritten = WriteProcessMemory(GetCurrentProcess(), lpAddress, lpBuffer, nSize, NULL);
	if (!isProtect || !isWritten)
	{
		MessageBoxW(NULL, L"WriteMemory Error!!", NULL, NULL);
	}
}

BOOL SetHook(DWORD dwRawAddr, DWORD dwTarAddr, SIZE_T szRawSize)
{
	BOOL isProtect = 0;
	DWORD oldProtect = 0;
	PBYTE allocateAddr = 0;
	DWORD relativeAddr = 0;
	BYTE rawJmp[] = { 0xE9,0x00,0x00,0x00,0x00 };
	BYTE retJmp[] = { 0xE9,0x00,0x00,0x00,0x00 };
	BYTE tarCal[] = { 0xE8,0x00,0x00,0x00,0x00 };

	isProtect = VirtualProtect((LPVOID)dwRawAddr, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);
	allocateAddr = (PBYTE)VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (allocateAddr && isProtect)
	{
		//Copy the Code for the original address to alloc address
		memcpy(allocateAddr, (PVOID)dwRawAddr, szRawSize);

		//Write Jmp Code
		relativeAddr = (DWORD)allocateAddr - dwRawAddr - sizeof(rawJmp);
		memcpy(&rawJmp[1], &relativeAddr, sizeof(DWORD));
		memcpy((PBYTE)dwRawAddr, rawJmp, sizeof(rawJmp));

		//Write Call TarFunc Code
		relativeAddr = dwTarAddr - (DWORD)(&allocateAddr[szRawSize]) - sizeof(tarCal);
		memcpy(&tarCal[1], &relativeAddr, sizeof(DWORD));
		memcpy(&allocateAddr[szRawSize], tarCal, sizeof(tarCal));

		//Write Ret Code
		relativeAddr = (dwRawAddr + szRawSize) - (DWORD)(&allocateAddr[szRawSize + sizeof(tarCal)]) - sizeof(retJmp);
		memcpy(&retJmp[1], &relativeAddr, sizeof(DWORD));
		memcpy(&allocateAddr[szRawSize + sizeof(tarCal)], retJmp, sizeof(retJmp));

		return TRUE;
	}
	else
	{
		MessageBoxW(NULL, L"SetHook Failed!!", NULL, NULL);
		return FALSE;
	}

}

DWORD SetAddr(LPCSTR lpProcName, LPCWSTR lpLibFileName)
{
	PBYTE realAddr = 0;
	HMODULE hDll = LoadLibraryW(lpLibFileName);

	if (hDll != NULL)
	{
		realAddr = (PBYTE)GetProcAddress(hDll, lpProcName);
		if (realAddr[6] == 0xFF && realAddr[7] == 0x25)
		{
			WriteMemory(&realAddr, &realAddr[8], 4);
			WriteMemory(&realAddr, realAddr, 4);
		}

		FreeLibrary(hDll);
		return (DWORD)realAddr;
	}
	return 0;
}

BOOL DetourAttachFunc(PVOID ppPointer, PVOID pDetour)
{
	LONG erroAttach = 0;
	LONG erroCommit = 0;

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	erroAttach = DetourAttach((PVOID*)ppPointer, pDetour);
	erroCommit = DetourTransactionCommit();

	if (erroAttach != NO_ERROR || erroCommit != NO_ERROR)
	{
		MessageBoxW(NULL, L"DetourAttach Failed!!", NULL, NULL);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL DetourDetachFunc(PVOID ppPointer, PVOID pDetour)
{
	LONG erroDetach = 0;
	LONG erroCommit = 0;

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	erroDetach = DetourDetach((PVOID*)ppPointer, pDetour);
	erroDetach = DetourTransactionCommit();
	if (erroDetach != NO_ERROR || erroCommit != NO_ERROR)
	{
		MessageBoxW(NULL, L"DetourDetach Failed!!", NULL, NULL);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}