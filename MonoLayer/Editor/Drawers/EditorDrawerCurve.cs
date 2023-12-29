using SyEngine.Core.Datas;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCurve : EditorDrawerBase<SyCurve>
{
	public EditorDrawerCurve(SyProxyEditor editor) : base(editor) { }

	protected override SyCurve DrawImpl(string name, SyCurve val)
		=> SyProxyEditor.GeDrawCurveField(name, val);
}
}