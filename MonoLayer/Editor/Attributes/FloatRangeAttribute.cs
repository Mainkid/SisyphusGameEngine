using System;
using System.Collections.Generic;

namespace SyEngine.Editor.Attributes
{
[AttributeUsage(AttributeTargets.Field)]
public class FloatRangeAttribute : EditorFieldBaseAttribute
{
	public readonly float Min;
	public readonly float Max;
	public readonly float Step;
	
	public FloatRangeAttribute(float min = float.MinValue, float max = float.MaxValue, float step = 0.1f)
	{
		Min  = min;
		Max  = max;
		Step = step;
	}
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public override int Priority => 0;

	public override bool Draw(
		object holder,
		string name, 
		ref object rawVal, 
		List<EditorFieldBaseAttribute> attrs,
		int attrIdx)
	{
		if (rawVal is float val)
		{
			if (SyProxyEditor.GeDrawFloatField(name, ref val, Min, Max, Step))
			{
				rawVal = val;
				return true;
			}
		}
		return false;
	}
}
}