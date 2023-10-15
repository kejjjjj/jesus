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
        Dvar_Reregister(dvar, flags, name, type, description, defaultValue, domain);
        return dvar;
    }

    return Dvar_RegisterNew(name, type, flags, description, defaultValue, domain);
   
}

dvar_s* Dvar_RegisterNew(const char* name, dvar_type _type, int flags, const char* description, dvar_value defaultValue, dvar_limits domain)
{
    return engine_call<dvar_s * __cdecl>(false, 0x056C130, name, _type, flags, description, defaultValue, domain);
}
void Dvar_Reregister(dvar_s* dvar, int flags, const char* name, dvar_type _type, const char* desc, dvar_value defaultValue, dvar_limits _domain)
{


    __asm {
        push _domain;
        push defaultValue;
        push desc;
        push _type;
        push name;
        mov eax, dvar;
        mov edi, flags;
        mov esi, 0x56BFF0;
        call esi;
        add esp, 20;

    }
}
dvar_s* Dvar_RegisterVariant(const char* dvarName, dvar_type _type, int flags, const char* description, dvar_value defaultValue, dvar_limits limits)
{
    dvar_s* r = 0;

    __asm {
        mov eax, dvarName;
        push limits;
        push defaultValue;
        push description;
        push flags;
        push _type;
        mov esi, 0x56C350;
        call esi;
        add esp, 20;
        mov r, eax;
    }

    return r;
}
dvar_s* Dvar_RegisterFloat(const char* name, float value, float min, float max, int flags, const char* description)
{
    return engine_call<dvar_s * __cdecl>(false, 0x56C460, name, value, min, max, flags, description);
}
dvar_s* Dvar_RegisterInt(const char* name, int value, int min, int max, int flags, const char* description)
{
    return engine_call<dvar_s * __cdecl>(false, 0x56C410, name, value, min, max, flags, description);

}
dvar_s* Dvar_RegisterBool(const char* name, int flags, bool value, const char* description)
{
    dvar_s* result = 0;

    __asm {
        movzx al, value;
        push description;
        push flags;
        push name;
        mov esi, 0x56C3C0;
        call esi;
        add esp, 0xC;
        mov result, eax;
    }

    return result;

}
void GetDvarIntValue(Operand* result, Operand* source)
{
    __asm mov result, esi;
    __asm mov source, edx;

    decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_GET_DVAR_INT);
    detour_func.cast_call<void(*)(Operand*, Operand*)>(source, result);

    if (source->dataType == expDataType::VAL_STRING) {
        //printf("%s\n", source->internals.string);
        if (!strcmp(source->internals.string, "com_maxfps")) {
            result->internals.intVal = 69;
        }
    }

    return;

}