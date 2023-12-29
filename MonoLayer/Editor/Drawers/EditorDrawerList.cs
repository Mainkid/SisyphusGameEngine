using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerList<TList, TItem> : EditorDrawerBase<TList> 
	where TList : List<TItem>
{
	public EditorDrawerList(SyProxyEditor editor) : base(editor) { }

	public override bool Draw(string name, ref TList val)
	{
		var isChanged = false;

		if (val == null)
		{
			val       = (TList)new List<TItem>();
			isChanged = true;
		}

		if (SyProxyEditor.GeDrawArrayHead($"{name} [{val.Count}]"))
		{
			SyProxyEditor.GeIndent(true);

			for (int i = 0; i < val.Count; i++)
			{
				var item = val[i];
				isChanged |= Editor.DrawField($"[{i}]", ref item);
				val[i] = item;
				int action = SyProxyEditor.GeDrawArrayItemButtons(i);
				if (action == 1)
				{
					val.RemoveAt(i--);
					isChanged = true;
				}
				else if (action == 2 && i > 0)
				{
					(val[i], val[i-1]) = (val[i - 1], val[i]);
					isChanged          = true;
				}
				else if (action == 3 && i < val.Count - 1)
				{
					(val[i], val[i+1]) = (val[i+1], val[i]);
					isChanged          = true;
				}
			}

			SyProxyEditor.GeIndent(false);

			if (SyProxyEditor.GeDrawArrayAddButton())
			{
				val.Add(default);
				isChanged = true;
			}
		}
		return isChanged;
	}
}
}