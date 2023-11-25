using SyEngine.Core;
using SyEngine.Core.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompLight : EditorDrawerComp<LightComp>
{
	public EditorDrawerCompLight(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref LightComp val)
	{
		var isChanged = false;

		isChanged |= Editor.DrawField("Type", ref val.Type);
		isChanged |= Editor.DrawField("Behaviour", ref val.Behaviour);
		isChanged |= Editor.DrawField("Color", ref val.Color);

		if (val.Type == LightComp.EType.PointLight)
		{
			isChanged |= Editor.DrawField("Radius", ref val.PointLightRadius);
			isChanged |= Editor.DrawField("Should Cast Shadows", ref val.ShouldCastShadows);
		}
		return isChanged;
	}
}
}