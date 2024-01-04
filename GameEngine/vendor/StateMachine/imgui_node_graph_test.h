#pragma once
#include "../ImGui/imgui.h"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);
inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs);
void ShowExampleAppCustomNodeGraph(bool* opened);
