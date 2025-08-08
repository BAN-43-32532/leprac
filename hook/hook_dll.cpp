// hook_dll.cpp
#include <atomic>
#include <MinHook.h>
#include <Windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <objbase.h> // for CoCreateInstance

// 全局变量
HMODULE          g_originalDll = NULL;
HMODULE          g_customDll   = NULL;
std::atomic<bool> g_hooksInstalled(false);
CRITICAL_SECTION g_cs;

// 函数指针类型
using DirectInput8Create_t   = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
using DllCanUnloadNow_t      = HRESULT(WINAPI*)();
using DllGetClassObject_t    = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
using DllRegisterServer_t    = HRESULT(WINAPI*)();
using DllUnregisterServer_t  = HRESULT(WINAPI*)();
using CoCreateInstance_t     = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);

// 原始函数指针
DirectInput8Create_t  Original_DirectInput8Create   = nullptr;
DllCanUnloadNow_t     Original_DllCanUnloadNow      = nullptr;
DllGetClassObject_t   Original_DllGetClassObject    = nullptr;
DllRegisterServer_t   Original_DllRegisterServer    = nullptr;
DllUnregisterServer_t Original_DllUnregisterServer  = nullptr;
CoCreateInstance_t    Original_CoCreateInstance     = nullptr;

void Log(char const* fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  char exePath[MAX_PATH];
  GetModuleFileNameA(NULL, exePath, MAX_PATH);

  std::string fullPath = exePath;
  auto pos = fullPath.find_last_of("\\/");
  std::string dirPath = (pos != std::string::npos) ? fullPath.substr(0, pos) : ".";

  std::string logPath = dirPath + "\\test.txt";

  std::ofstream logFile(logPath, std::ios::app);
  if (logFile.is_open()) {
    logFile << buffer << "\n";
  }
}

bool LoadCustomDll() {
    if (g_customDll) return true;

    char modulePath[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, modulePath, MAX_PATH);

    std::string dir(modulePath);
    size_t pos = dir.find_last_of("\\/");
    if (pos == std::string::npos) {
        Log("LoadCustomDll: cannot find path separator");
        return false;
    }

    std::string dllPath = dir.substr(0, pos + 1) + "test\\dinput8.dll";
    std::ostringstream oss;
    oss << "LoadCustomDll: loading " << dllPath;
    Log(oss.str().c_str());

    g_customDll = LoadLibraryA(dllPath.c_str());
    if (!g_customDll) {
        std::ostringstream err;
        err << "LoadCustomDll: LoadLibraryA failed, err=" << GetLastError();
        Log(err.str().c_str());
    }
    return g_customDll != nullptr;
}

HRESULT WINAPI Hooked_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
    EnterCriticalSection(&g_cs);
    bool loaded = LoadCustomDll();
    LeaveCriticalSection(&g_cs);

    if (loaded) {
        auto fn = (DirectInput8Create_t)GetProcAddress(g_customDll, "DirectInput8Create");
        if (fn) {
            Log("Hooked_DirectInput8Create: calling custom");
            return fn(hinst, dwVersion, riidltf, ppvOut, punkOuter);
        }
    }
    Log("Hooked_DirectInput8Create: calling original");
    return Original_DirectInput8Create ? Original_DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter) : E_FAIL;
}

HRESULT WINAPI Hooked_DllCanUnloadNow() {
    EnterCriticalSection(&g_cs);
    bool loaded = LoadCustomDll();
    LeaveCriticalSection(&g_cs);

    if (loaded) {
        auto fn = (DllCanUnloadNow_t)GetProcAddress(g_customDll, "DllCanUnloadNow");
        if (fn) {
            Log("Hooked_DllCanUnloadNow: custom");
            return fn();
        }
    }
    Log("Hooked_DllCanUnloadNow: original");
    return Original_DllCanUnloadNow ? Original_DllCanUnloadNow() : S_FALSE;
}

HRESULT WINAPI Hooked_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    EnterCriticalSection(&g_cs);
    bool loaded = LoadCustomDll();
    LeaveCriticalSection(&g_cs);

    if (loaded) {
        auto fn = (DllGetClassObject_t)GetProcAddress(g_customDll, "DllGetClassObject");
        if (fn) {
            Log("Hooked_DllGetClassObject: custom");
            return fn(rclsid, riid, ppv);
        }
    }
    Log("Hooked_DllGetClassObject: original");
    return Original_DllGetClassObject ? Original_DllGetClassObject(rclsid, riid, ppv) : CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI Hooked_DllRegisterServer() {
    EnterCriticalSection(&g_cs);
    bool loaded = LoadCustomDll();
    LeaveCriticalSection(&g_cs);

    if (loaded) {
        auto fn = (DllRegisterServer_t)GetProcAddress(g_customDll, "DllRegisterServer");
        if (fn) {
            Log("Hooked_DllRegisterServer: custom");
            return fn();
        }
    }
    Log("Hooked_DllRegisterServer: original");
    return Original_DllRegisterServer ? Original_DllRegisterServer() : E_FAIL;
}

HRESULT WINAPI Hooked_DllUnregisterServer() {
    EnterCriticalSection(&g_cs);
    bool loaded = LoadCustomDll();
    LeaveCriticalSection(&g_cs);

    if (loaded) {
        auto fn = (DllUnregisterServer_t)GetProcAddress(g_customDll, "DllUnregisterServer");
        if (fn) {
            Log("Hooked_DllUnregisterServer: custom");
            return fn();
        }
    }
    Log("Hooked_DllUnregisterServer: original");
    return Original_DllUnregisterServer ? Original_DllUnregisterServer() : E_FAIL;
}

HRESULT WINAPI Hooked_CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) {
    LPOLESTR clsidStr = nullptr;
    LPOLESTR iidStr = nullptr;
    StringFromCLSID(rclsid, &clsidStr);
    StringFromIID(riid, &iidStr);
    Log("Hooked_CoCreateInstance: CLSID=%ws IID=%ws", clsidStr, iidStr);
    CoTaskMemFree(clsidStr);
    CoTaskMemFree(iidStr);
    return Original_CoCreateInstance ? Original_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv) : E_FAIL;
}

DWORD WINAPI InstallHooks(LPVOID) {
    Log("InstallHooks: initializing MinHook");
    if (MH_Initialize() != MH_OK) {
        Log("InstallHooks: MH_Initialize failed");
        return 1;
    }

    g_originalDll = GetModuleHandleA("dinput8.dll");
    if (!g_originalDll) {
        Log("InstallHooks: loading original dinput8.dll");
        g_originalDll = LoadLibraryA("dinput8.dll");
        if (!g_originalDll) {
            Log("InstallHooks: LoadLibraryA failed");
            return 1;
        }
    }

    #define CREATE_HOOK_EX(target, hook, original)                                              \
        do {                                                                                     \
            if (MH_CreateHook(reinterpret_cast<LPVOID>(target), (LPVOID)(hook),                 \
                             reinterpret_cast<LPVOID*>(&original)) != MH_OK) {                 \
                Log("MH_CreateHook for " #hook " failed");                                     \
                return 1;                                                                        \
            }                                                                                    \
            Log("Hook created: " #hook);                                                         \
        } while (0)

    #define CREATE_HOOK(name)                                                                    \
        do {                                                                                     \
            auto addr = GetProcAddress(g_originalDll, #name);                                    \
            if (!addr) { Log("GetProcAddress " #name " failed"); return 1; }                 \
            if (MH_CreateHook(addr, &Hooked_##name, reinterpret_cast<void**>(&Original_##name)) != MH_OK) { \
                Log("MH_CreateHook " #name " failed"); return 1;                              \
            }                                                                                    \
            Log("Hook created: " #name);                                                         \
        } while (0)

    CREATE_HOOK(DirectInput8Create);
    CREATE_HOOK(DllCanUnloadNow);
    CREATE_HOOK(DllGetClassObject);
    CREATE_HOOK(DllRegisterServer);
    CREATE_HOOK(DllUnregisterServer);

    // Hook CoCreateInstance
    CREATE_HOOK_EX(CoCreateInstance, Hooked_CoCreateInstance, Original_CoCreateInstance);

    #undef CREATE_HOOK
    #undef CREATE_HOOK_EX

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        Log("InstallHooks: MH_EnableHook failed");
        return 1;
    }
    g_hooksInstalled = true;
    Log("InstallHooks: all hooks enabled");
    return 0;
}

void UninstallHooks() {
    if (!g_hooksInstalled) return;
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    g_hooksInstalled = false;
    Log("UninstallHooks: hooks removed");
}

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&g_cs);
        Log("DllMain: PROCESS_ATTACH");
        CreateThread(NULL, 0, InstallHooks, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        Log("DllMain: PROCESS_DETACH");
        UninstallHooks();
        DeleteCriticalSection(&g_cs);
        if (g_customDll) FreeLibrary(g_customDll);
        break;
    }
    return TRUE;
}
