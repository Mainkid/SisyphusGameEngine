using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector3 : EditorDrawerBase<SyVector3>
{
	public EditorDrawerVector3(SyProxyEditor editor) : base(editor) { }
	
	protected override SyVector3 DrawImpl(string name, SyVector3 val)
		=> SyProxyEditor.GeDrawVector3Field(name, val);
}
}