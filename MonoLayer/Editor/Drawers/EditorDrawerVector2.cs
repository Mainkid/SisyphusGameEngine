using System;
using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector2 : EditorDrawerBase<SyVector2>
{
	public EditorDrawerVector2(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref SyVector2 val)
		=> SyProxyEditor.GeDrawVector2Field(name, ref val);
}
}