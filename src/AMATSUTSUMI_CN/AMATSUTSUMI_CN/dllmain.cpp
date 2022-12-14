#include "Tools.h"

//アマツツミ PKG
#define CMVS_342_
#define SCRIPTADDR 0x0045A7B0
#define SCRIPTXOR 0x0045A896
#define SCRIPTCOM 0x0045A8C1
#define IMAGEADDR 0x00430685

PCHAR g_pFileName = 0;
DWORD g_dwExeImageBase = 0;
CHAR g_sReadFolder[] = ".\\FileHook\\";

typedef HGLOBAL (WINAPI* pLoadResource)(HMODULE hModule,HRSRC hResInfo);
pLoadResource rawLoadResource = 0;

typedef ATOM(WINAPI* pRegisterClassA)(const WNDCLASSA* lpWndClass);
pRegisterClassA rawRegisterClassA = 0;

HGLOBAL WINAPI newLoadResource(HMODULE hModule, HRSRC hResInfo)
{
	HGLOBAL result = rawLoadResource(hModule, hResInfo);
	if (((PDWORD)hResInfo)[1] == 1008611)
	{
		return (HGLOBAL)((PDWORD)hResInfo)[0];
	}
	return result;
}

ATOM WINAPI newRegisterClassA(WNDCLASSA* lpWndClass)
{
	lpWndClass->lpszMenuName = (LPCSTR)0x82;
	DetourDetachFunc(&rawRegisterClassA, newRegisterClassA);
	return rawRegisterClassA(lpWndClass);
}

BOOL FileExist()
{
	DWORD isFileExist = 0;;
	CHAR repPath[MAX_PATH] = { 0 };

	lstrcatA(repPath, g_sReadFolder);
	lstrcatA(repPath, g_pFileName);

	isFileExist = GetFileAttributesA(repPath);

	if ((isFileExist != INVALID_FILE_ATTRIBUTES) && (isFileExist != FILE_ATTRIBUTE_DIRECTORY))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID __declspec(naked)CMVS_342_PS3Hook()
{
	__asm
	{
		pushfd
		pushad
		mov eax, dword ptr[esp + 0x10]
		mov g_pFileName, eax
	}

	if (FileExist())
	{
		__asm
		{
			mov dword ptr[esp + 0x18], offset g_sReadFolder
		}
		*(BYTE*)SCRIPTXOR = 0xEB;
		*(BYTE*)SCRIPTCOM = 0xEB;
	}
	else
	{
		*(BYTE*)SCRIPTXOR = 0x74;
		*(BYTE*)SCRIPTCOM = 0x74;
	}

	__asm
	{
		popad
		popfd
		ret
	}
}

VOID __declspec(naked)CMVS_342_PB3Hook()
{
	__asm
	{
		pushfd
		pushad
		mov g_pFileName, ebx
	}

	if (FileExist())
	{
		__asm mov dword ptr[esp], offset g_sReadFolder
	}

	__asm
	{
		popad
		popfd
		ret
	}
}

VOID PatchBytes()
{
	BYTE patchTitle[] = { 0xA1,0xA1 };
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14E4B0), patchTitle, sizeof(patchTitle));

	BYTE patchCharSet[] = { 0x86 };
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x514BB), patchCharSet, sizeof(patchCharSet));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x5147B), patchCharSet, sizeof(patchCharSet));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x513AE), patchCharSet, sizeof(patchCharSet));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x5131E), patchCharSet, sizeof(patchCharSet));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x70A1), patchCharSet, sizeof(patchCharSet));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x7E4CD), patchCharSet, sizeof(patchCharSet));

	BYTE patchBorders[] = { 0xFE };
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x7216), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x715B), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x6277), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x719A), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x71D7), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x7286), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x74C7), patchBorders, sizeof(patchBorders));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x7C97), patchBorders, sizeof(patchBorders));

	BYTE patchFont[] = "SimHei";
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14C204), patchFont, sizeof(patchFont));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14DDA8), patchFont, sizeof(patchFont));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x180100), patchFont, sizeof(patchFont));
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x180B00), patchFont, sizeof(patchFont));

	CHAR str1[] = "启动设置";
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14E8DC), str1, sizeof(str1));

	CHAR str2[] = "[标准]";
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14E8C8), str2, sizeof(str2));

	CHAR str3[] = "字体选择";
	WriteMemory((LPVOID)(g_dwExeImageBase + 0x14E54C), str3, sizeof(str3));
	
}

VOID PatchResource()
{
	rawLoadResource = (pLoadResource)SetAddr("LoadResource", L"kernel32.dll");
	DetourAttachFunc(&rawLoadResource, newLoadResource);

	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4410), L".\\FileHook\\Menu130.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4420), L".\\FileHook\\Dialog132.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4430), L".\\FileHook\\Dialog133.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4440), L".\\FileHook\\Dialog134.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4450), L".\\FileHook\\Dialog135.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4460), L".\\FileHook\\Dialog136.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4470), L".\\FileHook\\Dialog137.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4480), L".\\FileHook\\Dialog138.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E4490), L".\\FileHook\\Dialog139.bin");
	WriteResourceTable((PBYTE)(g_dwExeImageBase + 0x1E44A0), L".\\FileHook\\Dialog147.bin");
}

VOID PatchFile()
{
	SetHook(SCRIPTADDR, (DWORD)CMVS_342_PS3Hook, 0x9);
	SetHook(IMAGEADDR, (DWORD)CMVS_342_PB3Hook, 0x6);
}

VOID PatchMenu()
{
	rawRegisterClassA = (pRegisterClassA)RegisterClassA;
	DetourAttachFunc(&rawRegisterClassA, newRegisterClassA);
}

VOID StartHook()
{
	g_dwExeImageBase = (DWORD)GetModuleHandleW(NULL);

	//PatchMenu();
	PatchFile();
	PatchBytes();
	PatchResource();

}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		StartHook();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

EXTERN_C VOID __declspec(dllexport) DirA(){}