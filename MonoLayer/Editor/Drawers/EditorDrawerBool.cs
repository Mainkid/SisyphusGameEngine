namespace SyEngine.Editor.Drawers
{
public class EditorDrawerBool : EditorDrawerBase<bool>
{
	public EditorDrawerBool(SyProxyEditor editor) : base(editor) { }
	
	protected override bool DrawImpl(string name, bool val)
		=> SyProxyEditor.GeDrawBoolField(name, val);

}
}