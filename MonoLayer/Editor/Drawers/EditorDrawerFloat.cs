namespace SyEngine.Editor.Drawers
{
public class EditorDrawerFloat : EditorDrawerBase<float>
{
	public EditorDrawerFloat(SyProxyEditor editor) : base(editor) { }
	
	protected override float DrawImpl(string name, float val)
		=> SyProxyEditor.GeDrawFloatField(name, val);
}
}