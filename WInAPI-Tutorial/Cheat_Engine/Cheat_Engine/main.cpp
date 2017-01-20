#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <Tlhelp32.h>
#include <malloc.h>
#include <set>
using namespace std;
#pragma warning(disable:4996)
template<typename  T>
BOOLEAN findValue(DWORD pID, T value);
BOOLEAN listProcess();
template<typename  T> 
BOOLEAN NextScan(set<T*>&saved, set<T*>&saving, T value);
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
	DWORD minMem = 0;
	GetSystemInfo(&si);
	minMem = (DWORD)si.lpMinimumApplicationAddress;    // set BaseAddress

	HANDLE h;
	long long count = 0;
	T * FindData = NULL;    // Value for save of Find datas

	set<T *> t,u;

	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	while (VirtualQueryEx(h, (LPVOID)minMem, &mbi, sizeof(mbi)) == sizeof(mbi)) {
		if (minMem < (DWORD)si.lpMaximumApplicationAddress \
			&& (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE \
				|| mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ) \
			&& mbi.State == MEM_COMMIT)					// Check Address & Permission & State
		{
			FindData = (T *)malloc(mbi.RegionSize);		// Allocat page size
			if (ReadProcessMemory(h, mbi.BaseAddress, FindData, mbi.RegionSize, NULL)) {
				printf("\n================================Founing================================\n");
				for (int i = 0; i < mbi.RegionSize / sizeof(T); i++) {		// Searching...
					if (FindData[i] == value) {
						printf("FindData: 0x%p\n", (LONGLONG)mbi.BaseAddress + i);
						t.insert((T*)((char *)mbi.BaseAddress + i));
						count++;
						if (count % 4 == 0) printf("\n");
					}
				}
			}
			free(FindData);
		}
		minMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}

	
	int conti = 1;

	while (conti == 1) {
		printf("\nContinue?");
		scanf("%d", &conti);	// seconde scan

		NextScan(t, u, value);
		for (auto k = u.begin(); k != u.end(); k++) {
			printf("FindData: 0x%p Data %d\n", *k, **k);
		}
		t.clear();

		t.insert(u.begin(), u.end());
		u.clear();
	}

	return 1;
}

template<typename T>
BOOLEAN NextScan(set<T*>&saved, set<T*>&saving, T value) {
	for (auto k = saved.begin(); k != saved.end(); k++) {
		if (**k == value) {
			saving.insert(*k);
		}
	}
	return 1;
}