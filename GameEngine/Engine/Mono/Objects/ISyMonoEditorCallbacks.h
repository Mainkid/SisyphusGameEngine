#pragma once

#include <mono/jit/jit.h>
#include "../SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoEditorCallbacks
	{
	public:
		virtual void OnDrawCompHeader(MonoString* rawName) = 0;
		virtual int OnDrawIntField(MonoString* rawName, int val) = 0;
		virtual float OnDrawFloatField(MonoString* rawName, float val) = 0;
		virtual bool OnDrawBoolField(MonoString* rawName, bool val) = 0;
		virtual ProxyVector3 OnDrawVector3Field(MonoString* rawName, ProxyVector3 val) = 0;
		virtual ProxyVector4 OnDrawColorField(MonoString* rawName, ProxyVector4 val) = 0;
		virtual int OnDrawEnumField(MonoString* rawName, MonoArray* rawItems, int selected) = 0;
	};
}