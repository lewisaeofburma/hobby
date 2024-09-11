#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

int GetSyscallNumber(const char* functionName) {
    // Get the handle to Ntdll.dll
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        return -1;
    }

    // Get the address of the function (e.g., NtQuerySystemInformation)
    void* functionAddress = GetProcAddress(hNtdll, functionName);
    if (!functionAddress) {
        return -1;
    }

    // The syscall number is stored in the 4th byte of the function's first few instructions
    // mov r10, rcx (3 bytes) + mov eax, syscall_number (2 bytes)
    unsigned char* funcPtr = (unsigned char*)functionAddress;
    int syscallNumber = *(int*)(funcPtr + 4);

    return syscallNumber;
}

int main() {
    int syscallNumber = GetSyscallNumber("NtQuerySystemInformation");
    if (syscallNumber != -1) {
        printf("Syscall number for NtQuerySystemInformation: 0x%x\n", syscallNumber);
    } else {
        printf("Failed to find syscall number\n");
    }

    return 0;
}