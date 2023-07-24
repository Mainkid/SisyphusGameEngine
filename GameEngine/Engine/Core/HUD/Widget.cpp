#include "Widget.h"
#include "Hud.h"

void Widget::Render()
{
	isFocused = ImGui::IsWindowFocused();
	if (isFocused)
		hud->SetFocusedWindowName(this->windowID);
	
}
