#include <stdio.h>
#include <windows.h>
#include <Tlhelp32.h>
#include <malloc.h>
#pragma warning(disable:4996)

#define ARRAY_EACH_SIZE 100				// �迭 ũ��

BYTE *byteArray(unsigned int n, unsigned int size) {
	BYTE *arr = new BYTE[size];
	int i;
	for (i = 0; i<size; i++) {
		arr[i] = n % 256;
		n /= 256;
	}
	return arr;
}

PDWORD FindMem(HANDLE hProc, unsigned int nFind, unsigned int nSize) {
	SYSTEM_INFO si;						// �޸� �ּ� �ּҰ�, �ִ밪�� ���
	MEMORY_BASIC_INFORMATION mbi;		// ������ ���� ���
	DWORD nMem = 0, i, j, result_ct = 0;// ���� �޸� �ּ� ������ �� �� ���꿡 �ʿ��� ����

	BYTE *srcArray;						// �޸𸮿��� ã�Ƴ� ��
	BYTE *destArray;					// �޸𸮿��� �о ��
	DWORD *FindData = (DWORD *)malloc(ARRAY_EACH_SIZE * sizeof(DWORD));
										// ã�Ƴ� ���� ����
	srcArray = byteArray(nFind, nSize);	// ��ȯ

	GetSystemInfo(&si);
	nMem = (DWORD)si.lpMinimumApplicationAddress; 
										//�޸� �ּ��� �ּҰ��� ���Ѵ�.

	do {
		if (VirtualQueryEx(hProc, (LPVOID)nMem, &mbi, sizeof(mbi)) == sizeof(mbi)) {					// �������� ������ �о��
			if (mbi.RegionSize > 0 && mbi.Type == MEM_PRIVATE && mbi.State == MEM_COMMIT) {				// �������� ��밡������ �˾Ƴ���
				destArray = new BYTE[mbi.RegionSize];													// �޸𸮸� ���� �غ� �Ѵ�
				if (ReadProcessMemory(hProc, mbi.BaseAddress, destArray, mbi.RegionSize, NULL) != 0) {	// �޸𸮸� �д´�
					for (i = 0; i<(DWORD)mbi.RegionSize; i++) {											// ���� �޸𸮿� ã�� �޸𸮸� ���Ѵ�
						for (j = 0; j<nSize; j++) {
							if (i + nSize + 1 > mbi.RegionSize)	
								break;
							if (destArray[i + j] != srcArray[j]) {
								break;
							} 
							else if (j == nSize - 1) {													// ���� ã�Ƴ�
								if (result_ct % ARRAY_EACH_SIZE == 0)									// �迭�� ũ�⸦ �����Ѵ�
									FindData = (DWORD *)realloc(FindData, nSize*ARRAY_EACH_SIZE*(result_ct / ARRAY_EACH_SIZE + 1));
								FindData[result_ct] = (DWORD)mbi.BaseAddress + i;						// �迭�� �ּҸ� �����Ѵ�
								result_ct++;
							}
						}
					}
				}
				delete destArray;																		// �޸𸮸� �о����� ����
			}
			nMem = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;	//���� ������ �ּ� ���
		}
	} while (nMem < (DWORD)si.lpMaximumApplicationAddress);			// �ִ� �ּҸ� �Ѿ���� �������� ��������
	delete srcArray;												// �޸� ����
	return FindData;												// ����� ����
}


void main() {
	PROCESSENTRY32 pe;
	DWORD pID = GetCurrentProcessId();
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pID); //���μ��� ����Ʈ�� �̾Ƴ� �غ� �Ѵ�.

	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);			// ���μ��� �˻� ����

	printf("Process List: \n");

	while (Process32Next(hSnapShot, &pe)) {	// ���μ��� �˻�
		HANDLE k = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		if (k != 0)
			printf("    [%d]%S\n", pe.th32ProcessID, pe.szExeFile);
		CloseHandle(k);
	}
	CloseHandle(hSnapShot);					// ���μ��� �˻� ��

	DWORD nFind;
	DWORD nSize;

	printf("Select Process(Id): ");
	scanf("%d", &pID);
	printf("Value to Find: ");
	scanf("%d", &nFind);
	printf("Size of Value: ");
	scanf("%d", &nSize);

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	DWORD *p = FindMem(h, nFind, nSize);

	for (int i = 0; i<200; i++)
		printf("%d:\t0x%X\t%S\n", i + 1, p[i], pe.szExeFile);
	printf("And %d Addresses more..\n", _msize(p) - 200);

	free(p); //delete p;
}
