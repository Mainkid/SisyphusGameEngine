#pragma once
#include "imgui.h"
#include "../Api/SyMonoMethod.h"
#include "../Api/SyMonoObj.h"
#include "../Ecs/SyMonoProxyDatas.h"

namespace mono
{
	class SyMonoInput : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Input";
		inline static const std::string CLASS_NAME = "SyProxyInput";

		inline static const std::string GE_IS_KEY_PRESSED = "GeIsKeyPressed";
		inline static const std::string GE_IS_KEY_RELEASED = "GeIsKeyReleased";
		inline static const std::string GE_IS_KEY_DOWN = "GeIsKeyDown";
		inline static const std::string GE_GET_MOUSE_POS = "GeGetMousePos";
		inline static const std::string GE_GET_MOUSE_POS_DELTA = "GeGetMousePosDelta";
		inline static const std::string GE_GET_MOUSE_WHEEL = "GetMouseWheel";

	private:
		static bool GeIsKeyPressed(int key);
		static bool GeIsKeyReleased(int key);
		static bool GeIsKeyDown(int key);
		static void GeGetMousePos(ProxyVector2* out);
		static void GeGetMousePosDelta(ProxyVector2* out);
		static float GeGetMouseWheel();

	protected:
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
	};
}
