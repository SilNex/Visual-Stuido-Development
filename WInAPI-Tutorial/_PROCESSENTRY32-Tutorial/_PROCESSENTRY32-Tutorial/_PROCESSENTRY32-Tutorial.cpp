#include <stdio.h>
#include <Windows.h>
#include <Tlhelp32.h>
#pragma warning(disable:4996)

int main(void) {
	PROCESSENTRY32 pe;					// processentry32 선언
	pe.dwSize = sizeof(PROCESSENTRY32);	// 크기 입력
	DWORD pID = GetCurrentProcessId();	// 현재 프로세스의 pid 입력
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pID);	//프로세스 스넵샷 가져오기
	Process32First(hSnapShot, &pe);
	printf("[th32ProcessID]\tszExeFile\tcntThreads\tth32ParentProcessID\n");
	while (Process32Next(hSnapShot, &pe)) {
		HANDLE proh = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		if (proh != 0) {
			printf("[%d]%S\t%d\t%d\n", pe.th32ProcessID, pe.szExeFile, pe.cntThreads, pe.th32ParentProcessID);
		}
		CloseHandle(proh);
	}
	CloseHandle(hSnapShot);
}
