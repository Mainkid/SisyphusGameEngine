using SyEngine.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector3 : EditorDrawerBase<SyVector3>
{
	public EditorDrawerVector3(SyProxyEditor editor) : base(editor) { }
	
	public override bool Draw(string name, ref SyVector3 val)
		=> SyProxyEditor.GeDrawVector3Field(name, ref val);
}
}