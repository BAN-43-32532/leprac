// minimalist DLL injector for DX11 process with bitness check and diagnostics
#include <iostream>
#include <string>
#include <windows.h>

using LPFN_ISWOW64PROCESS = BOOL(WINAPI*)(HANDLE, PBOOL);

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Usage: injector <pid> <path_to_dll>" << std::endl;
    return 1;
  }

  DWORD      pid        = std::stoul(argv[1]);
  auto const dllPathArg = argv[2];

  // Resolve absolute DLL path
  char absPath[MAX_PATH];
  if (GetFullPathNameA(dllPathArg, MAX_PATH, absPath, nullptr) == 0) {
    std::cerr << "GetFullPathNameA failed: " << GetLastError() << std::endl;
    return 1;
  }
  char const* dllPath = absPath;

  // Open target process
  HANDLE hProcess = OpenProcess(
    PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
    FALSE,
    pid
  );
  if (!hProcess) {
    std::cerr << "Failed to open process " << pid << ": " << GetLastError() << std::endl;
    return 1;
  }

  // Check architecture match (injector vs target)
  BOOL isCurrentWow64 = FALSE, isTargetWow64 = FALSE;
  auto fnWow64 =
    reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsWow64Process"));
  if (fnWow64) {
    fnWow64(GetCurrentProcess(), &isCurrentWow64);
    fnWow64(hProcess, &isTargetWow64);
  }
  // On 64-bit Windows: x64 -> isWow64 false, x86 -> isWow64 true
  if (isCurrentWow64 == FALSE && isTargetWow64 == TRUE) {
    std::cerr << "Bitness mismatch: injector is 64-bit but target process is 32-bit." << std::endl;
    std::cerr << "Please compile injector as 32-bit for this target." << std::endl;
    CloseHandle(hProcess);
    return 1;
  }

  // Allocate memory for DLL path in target process
  size_t pathLen   = strlen(dllPath) + 1;
  LPVOID remoteMem = VirtualAllocEx(hProcess, nullptr, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!remoteMem) {
    std::cerr << "VirtualAllocEx failed: " << GetLastError() << std::endl;
    CloseHandle(hProcess);
    return 1;
  }

  // Write DLL path into target memory
  if (!WriteProcessMemory(hProcess, remoteMem, dllPath, pathLen, nullptr)) {
    std::cerr << "WriteProcessMemory failed: " << GetLastError() << std::endl;
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return 1;
  }

  // Get address of LoadLibraryA
  HMODULE hKernel32   = GetModuleHandleA("kernel32.dll");
  auto    loadLibAddr = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryA"));
  if (!loadLibAddr) {
    std::cerr << "GetProcAddress failed: " << GetLastError() << std::endl;
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return 1;
  }

  // Create remote thread
  HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibAddr, remoteMem, 0, nullptr);
  if (!hThread) {
    std::cerr << "CreateRemoteThread failed: " << GetLastError() << std::endl;
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return 1;
  }

  // Wait and get result
  WaitForSingleObject(hThread, INFINITE);
  DWORD remoteResult = 0;
  GetExitCodeThread(hThread, &remoteResult);
  if (remoteResult == 0) {
    std::cerr << "Remote LoadLibraryA failed (returned NULL)." << std::endl;
  } else {
    std::cout << "Injection succeeded, module base: 0x" << std::hex << remoteResult << std::dec << std::endl;
  }

  // Cleanup
  VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
  CloseHandle(hThread);
  CloseHandle(hProcess);

  return 0;
}
