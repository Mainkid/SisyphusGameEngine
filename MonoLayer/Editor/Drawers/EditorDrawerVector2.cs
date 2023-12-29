using System;
using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector2 : EditorDrawerBase<SyVector2>
{
	public EditorDrawerVector2(SyProxyEditor editor) : base(editor) { }

	protected override SyVector2 DrawImpl(string name, SyVector2 val)
	{
		//return SyProxyEditor.GeDrawVector2Field(str, val);
		var vect = SyProxyEditor.GeDrawVector2Field(1, 2);
		return new SyVector2(1, 1);
	}
}
}