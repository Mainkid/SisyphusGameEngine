using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;
using SyEngine.Core.Resources;
using SyEngine.Editor.Drawers;
using SyEngine.Logger;

namespace SyEngine.Editor
{
public class SyProxyEditor
{
	private SyEcs _ecs;
		
	private Type[] _compsTypesBuffer = new Type[10];

	private Dictionary<Type, IEditorDrawer> _drawers;
	//-----------------------------------------------------------
	//-----------------------------------------------------------

	private void DrawEntityComps(uint engineEnt)
	{
		if (!_ecs.ToGameEnt(engineEnt, out int gameEnt))
			return;

		for (var i = 0; i < _compsTypesBuffer.Length; i++)
			_compsTypesBuffer[i] = null;

		int count = _ecs.World.GetComponentTypes(gameEnt, ref _compsTypesBuffer);

		for (var i = 0; i < count; i++)
		{
			var compType = _compsTypesBuffer[i];
			if (TryGetDrawer(compType, out var drawer) && drawer is IEditorDrawerComp compDrawer)
			{
				compDrawer.DrawComp(gameEnt);
			}
		}
	}

	internal bool DrawField(string name, Type type, ref object val)
	{
		if (!TryGetDrawer(type, out var drawer))
			return false;
		drawer.DrawRaw(name, ref val);
		return true;
	}

	internal bool DrawField<T>(string name, ref T val)
	{
		if (!TryGetDrawer(typeof(T), out var drawer))
			return false;
		return ((EditorDrawerBase<T>)drawer).Draw(name, ref val);
	}
	
	private bool TryGetDrawer(Type type, out IEditorDrawer drawer)
	{
		if (_drawers.TryGetValue(type, out drawer))
			return true;
		if (type.IsEnum)
		{
			var drawerType = typeof(EditorDrawerEnumBase<>).MakeGenericType(type);
			drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this);
			_drawers.Add(type, drawer);
			return true;
		}
		if (type.GetInterface(nameof(SyEcs.IComp)) != null)
		{
			var drawerType = typeof(EditorDrawerComp<>).MakeGenericType(type);
			drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this, _ecs);
			_drawers.Add(type, drawer);
			return true;
		}
		if (type.TryExtractGeneric(out var defType, out var genType))
		{
			if (defType == typeof(ResRef<>))
			{
				var drawerType = typeof(EditorDrawerResRef<,>).MakeGenericType(type, genType);
				drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this);
				_drawers.Add(type, drawer);
				return true;
			}
			if (defType == typeof(List<>))
			{
				var drawerType = typeof(EditorDrawerList<,>).MakeGenericType(type, genType);
				drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this);
				_drawers.Add(type, drawer);
				return true;
			}
		}
		if (type.IsArray || type.Namespace?.StartsWith("System") == true)
		{
			//TODO
			return false;
		}
		if (type.IsClass || type.IsValueType && !type.IsPrimitive)
		{
			var drawerType = typeof(EditorDrawerReflect<>).MakeGenericType(type);
			drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this);
			_drawers.Add(type, drawer);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	internal void EgInit(SyProxyEcs proxyEcs)
	{
		_ecs = proxyEcs.Ecs;

		_drawers = new Dictionary<Type, IEditorDrawer>
		{
			{ typeof(int), new EditorDrawerInt(this) },
			{ typeof(float), new EditorDrawerFloat(this) },
			{ typeof(bool), new EditorDrawerBool(this) },
			{ typeof(SyVector3), new EditorDrawerVector3(this) },
			{ typeof(SyColor), new EditorDrawerColor(this) },

			{ typeof(TransformComp), new EditorDrawerCompTransform(this, _ecs) },
			{ typeof(LightComp), new EditorDrawerCompLight(this, _ecs) }
		};
	}

	internal void EgDrawEntityComps(uint engineEnt)
	{
		try
		{
			DrawEntityComps(engineEnt);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEditor, e.Message);
		}
	}

	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeIndent(bool isIncrease);
    
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeDrawSeparator(string name);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeDrawText(string name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern int GeDrawIntField(string name, int val);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern float GeDrawFloatField(string name, float val);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawBoolField(string name, bool val);
    
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern SyVector3 GeDrawVector3Field(string name, SyVector3 val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern SyColor GeDrawColorField(string name, SyColor val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern int GeDrawEnumField(string name, string[] items, int selected);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern string GeDrawResField(string name, EResourceType resType, string uuid);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawArrayHead(string name);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern int GeDrawArrayItemButtons(int idx);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawArrayAddButton();
}
}