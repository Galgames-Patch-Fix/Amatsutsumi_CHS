#pragma once
#include <Windows.h>
#include <detours.h>

VOID WriteResourceTable(PBYTE lpResourceDataEntry, LPCWSTR lpResourceFileName);
VOID WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize);
BOOL SetHook(DWORD dwRawAddr, DWORD dwTarAddr, SIZE_T szRawSize);
DWORD SetAddr(LPCSTR lpProcName, LPCWSTR lpLibFileName);
BOOL DetourAttachFunc(PVOID ppPointer, PVOID pDetour);
BOOL DetourDetachFunc(PVOID ppPointer, PVOID pDetour);