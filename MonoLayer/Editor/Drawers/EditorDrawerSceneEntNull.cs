using System;
using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerSceneEntNull : EditorDrawerBase<SySceneEnt?>
{
	public EditorDrawerSceneEntNull(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref SySceneEnt? val)
	{
		bool isValid   = val?.InternalEnt != null;
		int  gameEnt   = isValid ? val.Value.InternalEnt.Value : default;
		uint engineEnt = default;
		
		if (isValid && !Ecs.ToEngineEnt(gameEnt, out engineEnt))
		{
			val = null;
			return true;
		}

		bool isChanged = SyProxyEditor.GeDrawEntityField(name, ref isValid, ref engineEnt);
		
		if (isChanged)
		{
			if (isValid)
			{
				if (Ecs.ToGameEnt(engineEnt, out gameEnt))
					val = new SySceneEnt(gameEnt);
				else
					val = null;
			}
			else
			{
				val = null;
			}
		}
		return isChanged;
	}
}
}