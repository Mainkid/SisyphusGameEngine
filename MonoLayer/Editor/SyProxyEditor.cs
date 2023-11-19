using System;
using System.Reflection;
using System.Runtime.CompilerServices;
using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Logger;

namespace SyEngine.Editor
{
public class SyProxyEditor
{
	private SyEcs _ecs;

	private Type[] _compsTypesBuffer = new Type[10];

	private void DrawEntityCustomComps(uint engineEnt)
	{
		if (!_ecs.ToGameEnt(engineEnt, out int gameEnt))
			return;

		for (var i = 0; i < _compsTypesBuffer.Length; i++)
			_compsTypesBuffer[i] = null;

		int count = _ecs.World.GetComponentTypes(gameEnt, ref _compsTypesBuffer);
		
		for (var i = 0; i < count; i++)
		{
			var type = _compsTypesBuffer[i];
			if (type == typeof(TransformComp) ||
			    type == typeof(MeshComp))
				continue;

			DrawCustomComp(gameEnt, type);
		}
	}

	private void DrawCustomComp(int ent, Type compType)
	{
		GeDrawCompHeader(compType.Name);

		var    pool = _ecs.World.GetPoolByType(compType);
		object comp = pool.GetRaw(ent);

		var isCompChanged = false;

		var fields = compType.GetFields(BindingFlags.Instance | BindingFlags.Public);
		foreach (var field in fields)
		{
			bool isFieldChanged = false;

			string name = field.Name;
			object val  = field.GetValue(comp);

			if (field.FieldType == typeof(int))
				isFieldChanged |= DrawIntField(name, ref val);
			else if (field.FieldType == typeof(float))
				isFieldChanged |= DrawFloatField(name, ref val);
			else if (field.FieldType == typeof(bool))
				isFieldChanged |= DrawBoolField(name, ref val);

			if (isFieldChanged)
				field.SetValue(comp, val);

			isCompChanged |= isFieldChanged;
		}
		
		if (isCompChanged)
			pool.SetRaw(ent, comp);
	}

	private bool DrawIntField(string name, ref object val)
	{
		var prevVal = (int)val;
		int newVal  = GeDrawIntField(name, prevVal);
		if (prevVal == newVal)
			return false;
		val = newVal;
		return true;
	}

	private bool DrawFloatField(string name, ref object val)
	{
		var   prevVal = (float)val;
		float newVal  = GeDrawFloatField(name, prevVal);
		if (Math.Abs(prevVal - newVal) <= float.Epsilon)
			return false;
		val = newVal;
		return true;
	}

	private bool DrawBoolField(string name, ref object val)
	{
		var  prevVal = (bool)val;
		bool newVal  = GeDrawBoolField(name, prevVal);
		if (prevVal == newVal)
			return false;
		val = newVal;
		return true;
	}


	//-----------------------------------------------------------
	//-----------------------------------------------------------
	internal void EgInit(SyProxyEcs proxy)
	{
		_ecs = proxy.Ecs;
	}

	internal void EgDrawEntityCustomComps(uint engineEnt)
	{
		try
		{
			DrawEntityCustomComps(engineEnt);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEditor, e.Message);
		}
	}

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void GeDrawCompHeader(string name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int GeDrawIntField(string name, int val);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern float GeDrawFloatField(string name, float val);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern bool GeDrawBoolField(string name, bool val);
}
}