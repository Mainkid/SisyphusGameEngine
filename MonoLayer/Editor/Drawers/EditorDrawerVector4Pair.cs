using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector4Pair : EditorDrawerBase<SyVector4Pair>
{
	public EditorDrawerVector4Pair(SyProxyEditor editor) : base(editor) { }
	
	protected override SyVector4Pair DrawImpl(string name, SyVector4Pair val)
	{
		val.A = SyProxyEditor.GeDrawVector4Field($"{name}.A", val.A);
		val.B = SyProxyEditor.GeDrawVector4Field($"{name}.B", val.B);
		return val;
	}
}
}