#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#pragma warning(disable:4996)
DWORD test = 100;

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
	minMem = (DWORD)si.lpMinimumApplicationAddress;	// set BaseAddress

	HANDLE h;
	long long count = 0;
	T * FindData = NULL;	// Value for save of Find datas


	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	while (VirtualQueryEx(h, (LPVOID)minMem, &mbi, sizeof(mbi)) == sizeof(mbi)) {
		if (minMem < (DWORD)si.lpMaximumApplicationAddress \
			&& (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE \
				|| mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ) \
			&& mbi.State == MEM_COMMIT)						// Check Address & Permission & State
		{
			FindData = (T *)malloc(mbi.RegionSize);			// Allocat page size
			if (ReadProcessMemory(h, mbi.BaseAddress, FindData, mbi.RegionSize, NULL)) {
				//printf("\n================================Founing================================\n");
				for (int i = 0; i < mbi.RegionSize / sizeof(T); i++) {						// Searching...
					if (FindData[i] == value && (DWORD*)mbi.BaseAddress + i == &test) {
						printf("FindData: 0x%p \n", (LONGLONG)mbi.BaseAddress + i);
						count++;
						if (count % 4 == 0) printf("\n");
					}
				}
			} /* else {
			  printf("%d\n", GetLastError());
			  } */
			free(FindData);
		}
		minMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}
	return 1;
}

int main(void) {	
	listProcess();
	DWORD pID, value;
	printf("pID & value");
	scanf("%d %d", &pID, &value);
	findValue(pID, value);

	HANDLE h;
	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, NULL);
	SYSTEM_INFO si;
	MEMORY_BASIC_INFORMATION mbi;
	DWORD mMem = 0;
	GetSystemInfo(&si);

	mMem = (DWORD)si.lpMinimumApplicationAddress;

	while (1) {
		if (VirtualQueryEx(h, (LPCVOID)mMem, &mbi, NULL)==sizeof(mbi)) {
			if (mMem < (DWORD)si.lpMaximumApplicationAddress \
				&& (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE) \
				&& mbi.State == MEM_COMMIT) {
				// find data에서 정보를 가져와야함
			}
		}
	}
	
	//WriteProcessMemory(h, );
	listProcess();

}
