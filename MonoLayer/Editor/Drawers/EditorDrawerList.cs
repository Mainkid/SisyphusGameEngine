using System;
using System.Collections.Generic;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerList<TList, TItem> : EditorDrawerBase<TList> 
	where TList : List<TItem>
{
	public EditorDrawerList(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }

	public override bool Draw(string name, ref TList val)
	{
		var isChanged = false;

		if (val == null)
		{
			val       = (TList)new List<TItem>();
			isChanged = true;
		}
		
		SyProxyEditor.GeBeginGroup(name);

		if (SyProxyEditor.GeDrawFoldout($"{name} [{val.Count}]"))
		{
			SyProxyEditor.GeIndent(true);

			for (int i = 0; i < val.Count; i++)
			{
				int action = SyProxyEditor.GeDrawArrayItemButtons(i);
				if (action == 1)
				{
					val.RemoveAt(i--);
					isChanged = true;
				}
				else if (action == 2 && i > 0)
				{
					(val[i], val[i - 1]) = (val[i - 1], val[i]);
					isChanged            = true;
				}
				else if (action == 3 && i < val.Count - 1)
				{
					(val[i], val[i + 1]) = (val[i + 1], val[i]);
					isChanged            = true;
				}
				else
				{
					SyProxyEditor.GeSameLine();
					
					var item = val[i];
					isChanged |= Editor.DrawField($"[{i}]", ref item);
					val[i]    =  item;
				}
			}
            
			if (SyProxyEditor.GeDrawButton("Add Item"))
			{
				val.Add(default);
				isChanged = true;
			}
			
			SyProxyEditor.GeIndent(false);
		}
		
		SyProxyEditor.GeEndGroup();
		return isChanged;
	}
}
}