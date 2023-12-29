using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerColor : EditorDrawerBase<SyColor>
{
	public EditorDrawerColor(SyProxyEditor editor) : base(editor) { }
	
	public override bool Draw(string name, ref SyColor val)
		=> SyProxyEditor.GeDrawColorField(name, ref val);
}
}