namespace SyEngine.Editor.Drawers
{
public class EditorDrawerInt : EditorDrawerBase<int>
{
	public EditorDrawerInt(SyProxyEditor editor) : base(editor) { }
	
	protected override int DrawImpl(string name, int val)
		=> SyProxyEditor.GeDrawIntField(name, val);
}
}