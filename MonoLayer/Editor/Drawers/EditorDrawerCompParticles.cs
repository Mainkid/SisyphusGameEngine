using System;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompParticles : EditorDrawerComp<ParticlesComp>
{
	public EditorDrawerCompParticles(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		
	}

	public override bool Draw(string name, ref ParticlesComp val)
	{
		bool isChanged = base.Draw(name, ref val);
		if (isChanged)
			val.IsDirty = true;
		return isChanged;
	}
}
}