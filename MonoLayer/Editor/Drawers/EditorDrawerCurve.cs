using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCurve : EditorDrawerBase<SyCurve>
{
	public EditorDrawerCurve(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref SyCurve val)
		=> SyProxyEditor.GeDrawCurveField(name, ref val);
}
}