#include <stdio.h>
#include <Windows.h>
#include <Tlhelp32.h>
#include <malloc.h>
#pragma warning(disable:4996)
template<typename  T>
BOOLEAN findValue(DWORD pID, T value);
BOOLEAN listProcess();
DWORD test = 100;						// Testing vlaue

int main(void) {
	DWORD pID = GetCurrentProcessId();	// defalut current process id
	DWORD vFind;						// Find to Value

	listProcess();

	printf("Insert pID: ");
	scanf("%d", &pID);
	printf("Value to find: ");
	scanf("%d", &vFind);

	findValue(pID, vFind);
}

BOOLEAN listProcess() {
	PROCESSENTRY32 pe;					// processentry32 
	pe.dwSize = sizeof(PROCESSENTRY32);	// input pe dwSize
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);		// ALL SnapShot of process
	Process32First(hSnapShot, &pe);

	printf("[th32ProcessID]\tszExeFile\tcntThreads\tth32ParentProcessID\n");
	do {
		HANDLE proh = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);	// Open Process
		if (proh != 0) {
			printf("[%d]%.10S\t%d\t%d\n", pe.th32ProcessID, pe.szExeFile, pe.cntThreads, pe.th32ParentProcessID);
		}
		CloseHandle(proh);
	} while (Process32Next(hSnapShot, &pe));
	CloseHandle(hSnapShot);
	return 1;
}

template<typename  T>
BOOLEAN findValue(DWORD pID, T value)
{
	SYSTEM_INFO si;
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID minMem = 0;
	GetSystemInfo(&si);
	minMem = si.lpMinimumApplicationAddress;	// set BaseAddress

	HANDLE h;
	long long count = 0;
	T * FindData = NULL;	// Value for save of Find datas

	DWORD old;
	DWORD data=101;
							//DWORD old;
							//VirtualProtectEx(hproc, addr, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &old);

	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	while (1) {
		if (VirtualQueryEx(h, (LPVOID)minMem, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			if (minMem < si.lpMaximumApplicationAddress \
				&& (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE \
					|| mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ) \
				&& mbi.State == MEM_COMMIT)						// Check Address & Permission & State
			{
				FindData = (T *)malloc(mbi.RegionSize);			// Allocat page size
				if (ReadProcessMemory(h, mbi.BaseAddress, FindData, mbi.RegionSize, NULL)) {
					for (int i = 0; i < mbi.RegionSize / sizeof(T); i++) {						// Searching...
						if (FindData[i] == value && (DWORD*)mbi.BaseAddress + i == &test) {
							printf("FindData: 0x%p \n", (LONGLONG)mbi.BaseAddress + i);
							LPVOID tt = (LPVOID)((LONGLONG)mbi.BaseAddress + i);
							VirtualProtectEx(h, tt, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &old);
							// Memory write
							WriteProcessMemory(h, mbi.BaseAddress, &data, sizeof(data), NULL);
							// Memory write
							VirtualProtectEx(h, tt, mbi.RegionSize, old, NULL);
							count++;
							if (count % 4 == 0) printf("\n");
						}
					}
				} /* else {
				  printf("%d\n", GetLastError());
				  } */
				free(FindData);
			}
		}
		minMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
		//minMem = (LPVOID)((__int3264)mbi.BaseAddress + (__int3264)mbi.RegionSize);	// 64bit 

	}

	return 1;
}