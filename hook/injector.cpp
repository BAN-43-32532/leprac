// injector.cpp
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

// 简易日志
void Log(const wchar_t* msg) {
    std::wcout << msg << std::endl;
}

// 获取进程 ID
DWORD GetProcessIdByName(const wchar_t* processName) {
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe)) {
        if (_wcsicmp(pe.szExeFile, processName) == 0) {
            CloseHandle(snap);
            return pe.th32ProcessID;
        }
    }
    CloseHandle(snap);
    return 0;
}

// 注入 DLL
bool InjectDLL(DWORD pid, const wchar_t* dllPath) {
    Log(L"InjectDLL: opening process");
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) { Log(L"InjectDLL: OpenProcess failed"); return false; }

    SIZE_T size = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    LPVOID remote = VirtualAllocEx(hProc, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote) { Log(L"InjectDLL: VirtualAllocEx failed"); CloseHandle(hProc); return false; }

    if (!WriteProcessMemory(hProc, remote, dllPath, size, NULL)) {
        Log(L"InjectDLL: WriteProcessMemory failed");
        VirtualFreeEx(hProc, remote, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    auto hK32 = GetModuleHandleW(L"kernel32.dll");
    auto pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hK32, "LoadLibraryW");
    if (!pLoadLibrary) {
        Log(L"InjectDLL: GetProcAddress LoadLibraryW failed");
        VirtualFreeEx(hProc, remote, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, pLoadLibrary, remote, 0, NULL);
    if (!hThread) {
        Log(L"InjectDLL: CreateRemoteThread failed");
        VirtualFreeEx(hProc, remote, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    VirtualFreeEx(hProc, remote, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProc);

    if (exitCode == 0) {
        Log(L"InjectDLL: remote LoadLibrary returned 0");
        return false;
    }
    Log(L"InjectDLL: injection succeeded");
    return true;
}

int wmain() {
    SetConsoleOutputCP(CP_UTF8);
    Log(L"=== dinput8.dll Injector ===");

    const wchar_t* targetExe = L"th_dnh_woo.exe";
    DWORD pid = GetProcessIdByName(targetExe);
    if (!pid) {
        Log(L"Injector: target process not found");
        return 1;
    }
    std::wcout << L"Found PID: " << pid << std::endl;

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring dir(exePath);
    dir = dir.substr(0, dir.find_last_of(L"\\/"));

    std::wstring dllPath = dir + L"\\hook_dll.dll";
    std::wcout << L"Injecting DLL: " << dllPath << std::endl;

    if (!InjectDLL(pid, dllPath.c_str())) {
        Log(L"Injector: injection failed");
        return 1;
    }
    Log(L"Injector: injection complete");
    return 0;
}
