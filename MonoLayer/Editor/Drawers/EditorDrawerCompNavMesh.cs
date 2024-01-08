using System;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompNavMesh : EditorDrawerComp<NavMeshComp>
{
	public EditorDrawerCompNavMesh(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		
	}

	public override bool Draw(string name, ref NavMeshComp val)
	{
		bool isChanged = base.Draw(name, ref val);
		if (isChanged)
			val.IsDirty = true;
		return isChanged;
	}
}
}