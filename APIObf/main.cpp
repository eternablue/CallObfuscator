#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <winternl.h>

constexpr __forceinline uint32_t hash(const char* str)
{
    uint32_t hash = 5381, c = 0;
    while (c = *str++)
        hash = hash * 33 + c;
    return hash;
}

__forceinline uint64_t find_export_by_hash(uint32_t hashname)
{
    PPEB peb = (PPEB)((PTEB)NtCurrentTeb())->ProcessEnvironmentBlock;
    PLIST_ENTRY head = &peb->Ldr->InMemoryOrderModuleList;

    for (PLIST_ENTRY current = head->Flink; current != head; current = current->Flink)
    {
        PLDR_DATA_TABLE_ENTRY data_table = CONTAINING_RECORD(current, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		uint64_t image_base = (uint64_t)data_table->DllBase;

        uint32_t export_rva = ((PIMAGE_NT_HEADERS64)(image_base + ((PIMAGE_DOS_HEADER)image_base)->e_lfanew))->OptionalHeader.DataDirectory[0].VirtualAddress;
        if (!export_rva)
            continue;

        IMAGE_EXPORT_DIRECTORY* exports = (IMAGE_EXPORT_DIRECTORY*)(image_base + export_rva);

        for (uint32_t i = 0; i < exports->NumberOfNames; ++i)
            if (hash((char*)(image_base + ((uint32_t*)(image_base + exports->AddressOfNames))[i])) == hashname)
                return image_base + ((uint32_t*)(image_base + exports->AddressOfFunctions))[((uint16_t*)(image_base + exports->AddressOfNameOrdinals))[i]];
    }
}

#define CALL(x, ...) ((decltype(&x))([]() { constexpr uint32_t hashfunc = hash(#x); return find_export_by_hash(hashfunc); }()))(__VA_ARGS__)

void main()
{    
    CALL(Sleep, 2000);
}