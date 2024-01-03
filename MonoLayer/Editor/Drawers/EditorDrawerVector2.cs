using System;
using SyEngine.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector2 : EditorDrawerBase<SyVector2>
{
	public EditorDrawerVector2(SyProxyEditor editor) : base(editor) { }

	public override bool Draw(string name, ref SyVector2 val)
		=> SyProxyEditor.GeDrawVector2Field(name, ref val);
}
}