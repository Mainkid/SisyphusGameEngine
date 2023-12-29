using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector4 : EditorDrawerBase<SyVector4>
{
	public EditorDrawerVector4(SyProxyEditor editor) : base(editor) { }
	
	protected override SyVector4 DrawImpl(string name, SyVector4 val)
		=> SyProxyEditor.GeDrawVector4Field(name, val);
}
}