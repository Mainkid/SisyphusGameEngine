using System;
using SyEngine.Core;
using SyEngine.Core.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompTransform : EditorDrawerComp<TransformComp>
{
	public EditorDrawerCompTransform(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref TransformComp val)
	{
		var isChanged = false;
		isChanged |= Editor.DrawField("Local Position", ref val.LocalPosition);
		isChanged |= Editor.DrawField("Local Rotation", ref val.LocalRotation);
		isChanged |= Editor.DrawField("Local Scale", ref val.LocalScale);
		return isChanged;
	}
}
}