using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector4 : EditorDrawerBase<SyVector4>
{
	public EditorDrawerVector4(SyProxyEditor editor) : base(editor) { }
	
	public override bool Draw(string name, ref SyVector4 val)
		=> SyProxyEditor.GeDrawVector4Field(name, ref val);
}
}