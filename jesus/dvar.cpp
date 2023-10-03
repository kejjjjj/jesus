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

dvar_s* Dvar_Register(const char* name, dvar_type type, int flags, const char* description, dvar_value defaultValue, dvar_limits domain)
{
    dvar_s* dvar = Dvar_FindMalleableVar(name);

    if (dvar) {
        Dvar_Reregister(dvar, name, type, flags, defaultValue, domain);
        return dvar;
    }

    return Dvar_RegisterNew(name, type, flags, description, defaultValue, domain);
   
}

dvar_s* Dvar_RegisterNew(const char* name, dvar_type _type, int flags, const char* description, dvar_value defaultValue, dvar_limits domain)
{
    return engine_call<dvar_s * __cdecl>(false, 0x056C130, name, _type, flags, description, defaultValue, domain);
}
void Dvar_Reregister(dvar_s* dvar, const char* name, dvar_type _type, int flags, dvar_value defaultValue, dvar_limits domain)
{
    __asm {
        mov eax, dvar;
        mov edi, flags;
        push domain;
        push defaultValue;
        push _type;
        push name;
        push flags;
        push dvar;
        mov esi, 0x56BFF0;
        call esi;
        add esp, 24;

    }
}