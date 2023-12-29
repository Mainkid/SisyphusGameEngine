namespace SyEngine.Editor.Drawers
{
public class EditorDrawerBool : EditorDrawerBase<bool>
{
	public EditorDrawerBool(SyProxyEditor editor) : base(editor) { }
	
	public override bool Draw(string name, ref bool val)
		=> SyProxyEditor.GeDrawBoolField(name, ref val);

}
}