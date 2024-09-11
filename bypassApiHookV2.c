#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

#pragma comment(lib, "ntdll.lib")

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

// Syscall stub for direct syscall execution
unsigned char syscall_stub[] = {
    0x4C, 0x8B, 0xD1,                    // mov r10, rcx
    0xB8, 0x00, 0x00, 0x00, 0x00,        // mov eax, <syscall_number>
    0x0F, 0x05,                          // syscall
    0xC3                                 // ret
};

void InvokeSyscall(int syscall_number, SYSTEM_INFORMATION_CLASS infoClass, PVOID buffer, ULONG bufferLength, PULONG returnLength) {
    // Patch the correct syscall number into the stub
    *(int*)(syscall_stub + 4) = syscall_number;

    // Cast the syscall stub to a function pointer and execute it
    _NtQuerySystemInformation syscall_function = (_NtQuerySystemInformation)(syscall_stub);
    NTSTATUS status = syscall_function(infoClass, buffer, bufferLength, returnLength);

    if (status == 0) {
        printf("Syscall succeeded!\n");
    } else {
        printf("Syscall failed with status: 0x%X\n", status);
    }
}

int main() {
    // Get the correct syscall number dynamically (use a static number as an example here)
    int syscall_number = 0x33;  // Example number for NtQuerySystemInformation in some versions of Windows

    // Define a buffer to hold the system information
    ULONG bufferLength = 0x10000;
    PVOID buffer = malloc(bufferLength);
    ULONG returnLength = 0;

    if (buffer) {
        // Invoke the syscall directly, bypassing the typical API
        InvokeSyscall(syscall_number, SystemProcessInformation, buffer, bufferLength, &returnLength);

        // Process the returned information (in a real scenario, you would parse this)
        printf("System Information Retrieved.\n");

        free(buffer);
    }

    return 0;
}