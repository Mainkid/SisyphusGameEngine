using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCurve : EditorDrawerBase<SyCurve>
{
	public EditorDrawerCurve(SyProxyEditor editor) : base(editor) { }

	public override bool Draw(string name, ref SyCurve val)
		=> SyProxyEditor.GeDrawCurveField(name, ref val);
}
}