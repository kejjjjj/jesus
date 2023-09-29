#include "pch.hpp"

dvar_s* Dvar_FindMalleableVar(const char* name)
{
    DWORD addr = 0x56b5d0;
    __asm
    {
        mov edi, name
        call[addr]
    }
}