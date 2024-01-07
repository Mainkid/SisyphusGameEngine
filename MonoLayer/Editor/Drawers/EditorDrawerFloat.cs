using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerFloat : EditorDrawerBase<float>
{
	public EditorDrawerFloat(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref float val)
		=> SyProxyEditor.GeDrawFloatField(name, ref val, float.MinValue, float.MaxValue, 0.1f);
}
}