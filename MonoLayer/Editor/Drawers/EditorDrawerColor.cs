using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerColor : EditorDrawerBase<SyColor>
{
	public EditorDrawerColor(SyProxyEditor editor) : base(editor) { }
	
	protected override SyColor DrawImpl(string name, SyColor val)
		=> SyProxyEditor.GeDrawColorField(name, val);
}
}