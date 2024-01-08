#include "SyMonoInput.h"

#include "imgui.h"

using namespace mono;


bool SyMonoInput::GeIsKeyPressed(int key)
{
	return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
}

bool SyMonoInput::GeIsKeyReleased(int key)
{
	return ImGui::IsKeyReleased(static_cast<ImGuiKey>(key));
}

bool SyMonoInput::GeIsKeyDown(int key)
{
	return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
}

void SyMonoInput::GeGetMousePos(ProxyVector2* out)
{
	auto vec = ImGui::GetMousePos();
	out->X = vec.x;
	out->Y = vec.y;
}

void SyMonoInput::GeGetMousePosDelta(ProxyVector2* out)
{
	auto vec = ImGui::GetIO().MouseDelta;
	out->X = vec.x;
	out->Y = vec.y;
}

float SyMonoInput::GeGetMouseWheel()
{
	return ImGui::GetIO().MouseWheel;
}

const std::string& SyMonoInput::GetMonoClassName()
{
	return CLASS_NAME;
}

const std::string& SyMonoInput::GetNamespace()
{
	return NAMESPACE;
}

bool SyMonoInput::IsUserClass()
{
	return false;
}

SyResult SyMonoInput::OnAfterCreate()
{
	BindCallback(GE_IS_KEY_PRESSED, &GeIsKeyPressed);
	BindCallback(GE_IS_KEY_RELEASED, &GeIsKeyReleased);
	BindCallback(GE_IS_KEY_DOWN, &GeIsKeyDown);
	BindCallback(GE_GET_MOUSE_POS, &GeGetMousePos);
	BindCallback(GE_GET_MOUSE_POS_DELTA, &GeGetMousePosDelta);
	BindCallback(GE_GET_MOUSE_WHEEL, &GeGetMouseWheel);

	return {};
}

SyResult SyMonoInput::OnBeforeDestroy()
{
	return {};
}
