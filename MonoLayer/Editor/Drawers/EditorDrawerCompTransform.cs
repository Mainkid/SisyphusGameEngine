using SyEngine.Ecs;
using SyEngine.Ecs.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompTransform : EditorDrawerComp<TransformComp>
{
	public EditorDrawerCompTransform(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public override bool Draw(string name, ref TransformComp val)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout("World"))
		{
			isChanged |= Editor.DrawField("Position", ref val.Position);
			isChanged |= Editor.DrawField("Rotation", ref val.Rotation);
			isChanged |= Editor.DrawField("Scale", ref val.Scale);
		}
		isChanged |= Editor.DrawField("Local Position", ref val.LocalPosition);
		isChanged |= Editor.DrawField("Local Rotation", ref val.LocalRotation);
		isChanged |= Editor.DrawField("Local Scale", ref val.LocalScale);
        return isChanged;
	}
}
}