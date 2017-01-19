#include <stdio.h>
#include <Windows.h>
#include <Tlhelp32.h>
#include <malloc.h>
#pragma warning(disable:4996)
template<typename  T>
BOOLEAN findValue(DWORD pID, T value);
BOOLEAN listProcess() {
	PROCESSENTRY32 pe;					// processentry32 선언
	pe.dwSize = sizeof(PROCESSENTRY32);	// 크기 입력
	DWORD pID = GetCurrentProcessId();	// defalut 값 현재 프로세스의 pid 입력
	DWORD vFind;						// Find to Value를 저장할 변수
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);	//프로세스 스넵샷을 위한 핸들
	Process32First(hSnapShot, &pe);		//첫번째 스넵샷

	printf("[th32ProcessID]\tszExeFile\tcntThreads\tth32ParentProcessID\n");
	do {								//모든 프로세스의 스넵샷을 찍어 출력
		HANDLE proh = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		if (proh != 0) {
			printf("[%d]%.10S\t%d\t%d\n", pe.th32ProcessID, pe.szExeFile, pe.cntThreads, pe.th32ParentProcessID);
		}
		CloseHandle(proh);
	} while (Process32Next(hSnapShot, &pe));
	CloseHandle(hSnapShot);				// 스넵샷 핸들 삭제
	return 1;
}
DWORD test = 100;

int main(void) {
	DWORD pID = GetCurrentProcessId();	// defalut 값 현재 프로세스의 pid 입력
	DWORD vFind;						// Find to Value를 저장할 변수

	listProcess();

	printf("Insert pID: ");
	scanf("%d", &pID);
	printf("Value to find: ");
	scanf("%d", &vFind);

	findValue(pID, vFind);

	/*h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	while (VirtualQueryEx(h,(LPVOID)minMem,&mbi,sizeof(mbi)) == sizeof(mbi) ) {
		if (minMem < (DWORD)si.lpMaximumApplicationAddress \
			&& (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE) \
			&& mbi.State == MEM_COMMIT) {
			FindData = (char *)malloc(mbi.RegionSize * sizeof(char));			// 페이지 크기만큼 동적 할당
			if (ReadProcessMemory(h, mbi.BaseAddress, FindData, sizeof(char)*mbi.RegionSize, NULL)) {
				printf("\n================================Founing================================\n");
				for (int i = 0; i < mbi.RegionSize; i++) {						// Searching...
					if (FindData[i] == vFind) {
						printf("FindData: 0x%p ", (LONGLONG)mbi.BaseAddress+i);
						count++;
						if (count % 4 == 0) printf("\n");
					}
				}
			}
			else{
				printf("%d\n", GetLastError());
			}
			free(FindData);
		}
		minMem=(DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}*/

	//printf("%S", FindData);
	
	//free(vFind);
}

template<typename  T>
BOOLEAN findValue(DWORD pID, T value)
{
	SYSTEM_INFO si;
	MEMORY_BASIC_INFORMATION mbi;
	DWORD minMem = 0;
	GetSystemInfo(&si);
	minMem = (DWORD)si.lpMinimumApplicationAddress;

	HANDLE h;
	long long count = 0;
	T * FindData = NULL;				// 페이지를 저장할 배열


	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	while (VirtualQueryEx(h, (LPVOID)minMem, &mbi, sizeof(mbi)) == sizeof(mbi)) {
		if (minMem < (DWORD)si.lpMaximumApplicationAddress \
			&& (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE \
				|| mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ ) \
			&& mbi.State == MEM_COMMIT) 
		{
			FindData = (T *)malloc(mbi.RegionSize);			// 페이지 크기만큼 동적 할당
			if (ReadProcessMemory(h, mbi.BaseAddress, FindData, mbi.RegionSize, NULL)) {
				//printf("\n================================Founing================================\n");
				for (int i = 0; i < mbi.RegionSize/sizeof(T); i++) {						// Searching...
					if (FindData[i] == value && (DWORD*)mbi.BaseAddress + i == &test) {
						printf("FindData: 0x%p ", (LONGLONG)mbi.BaseAddress + i);
						count++;
						if (count % 4 == 0) printf("\n");
					}
				}
			}
			else {
				printf("%d\n", GetLastError());
			}
			free(FindData);
		}
		minMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}
	return 1;
}