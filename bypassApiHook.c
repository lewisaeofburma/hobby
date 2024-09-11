#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

#pragma comment(lib, "ntdll.lib")

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

// This structure will be populated with syscall information from ntdll.dll
unsigned char syscall_stub[] = {
    0x4C, 0x8B, 0xD1,                    // mov r10, rcx
    0xB8, 0x00, 0x00, 0x00, 0x00,        // mov eax, <syscall_number>
    0x0F, 0x05,                          // syscall
    0xC3                                 // ret
};

void InvokeSyscall(int syscall_number, PVOID args) {
    // Patch syscall number into the stub
    *(int*)(syscall_stub + 4) = syscall_number;

    // Cast the syscall stub to a function and call it
    auto syscall_function = (_NtQuerySystemInformation)(syscall_stub);
    syscall_function(args);
}

int main() {
    // Use NtQuerySystemInformation syscall number for the current version of Windows
    // This number may vary between Windows versions
    int syscall_number = 0x33;  // Example number for NtQuerySystemInformation on certain versions of Windows

    ULONG len = 0;
    PVOID buffer = NULL;

    // Invoke the syscall using our direct syscall execution method
    InvokeSyscall(syscall_number, buffer);

    printf("Syscall executed!\n");

    return 0;
}