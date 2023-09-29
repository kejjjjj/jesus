// dear imgui: wrappers for C++ standard library (STL) types (std::string, etc.)
// This is also an example of how you may wrap your own similar types.

// Changelog:
// - v0.10: Initial version. Added InputText() / InputTextMultiline() calls with std::string

#pragma once

#include <string>

namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

    IMGUI_API bool ButtonCentered(const char* label, float alignment = 0.5f, ImVec2 size = ImVec2(0, 0));
    IMGUI_API void TextCentered(const char* label, ...);
    IMGUI_API void TextCenteredVertical(const char* label, ...);
    IMGUI_API bool IsKeyPressed(const BYTE key);

    IMGUI_API bool IsHovered(ImVec2 mins, ImVec2 maxs);
    IMGUI_API bool IsClicked(ImVec2 mins, ImVec2 maxs);
}
