using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using SyEngine.Datas;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;
using SyEngine.Editor.Drawers;
using SyEngine.Helpers;
using SyEngine.Logger;
using SyEngine.Resources;

namespace SyEngine.Editor
{
public class SyProxyEditor
{
	private SyEcs _ecs;
		
	private Type[] _compsTypesBuffer = new Type[20];

	private Dictionary<Type, IEditorDrawer> _drawers;

	private List<Type> _allCompsTypes       = new List<Type>();
	private List<Type> _availableCompsTypes = new List<Type>();
	private string[]   _availableCompsNames;
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private int _prevCompsCount = -1;
	
	private void DrawEntityComps(uint engineEnt)
	{
		if (!_ecs.ToGameEnt(engineEnt, out int gameEnt))
			return;
		
		//--------- Existing Components ---------------
		for (var i = 0; i < _compsTypesBuffer.Length; i++)
			_compsTypesBuffer[i] = null;

		int compsCount = _ecs.World.GetComponentTypes(gameEnt, ref _compsTypesBuffer);

		for (var i = 0; i < compsCount; i++)
		{
			var compType = _compsTypesBuffer[i];
			if (TryGetDrawer(compType, out var drawer) && drawer is IEditorDrawerComp compDrawer)
			{
				compDrawer.DrawComp(gameEnt);
			}
		}
		
		//----------- Add Component ------------------
		if (_prevCompsCount != compsCount)
		{
			_prevCompsCount = compsCount;
			
			_availableCompsTypes = new List<Type>(_allCompsTypes);
			for (var i = 0; i < compsCount; i++)
				_availableCompsTypes.Remove(_compsTypesBuffer[i]);
			_availableCompsNames = new string[_availableCompsTypes.Count];
			for (var i = 0; i < _availableCompsNames.Length; i++)
				_availableCompsNames[i] = _availableCompsTypes[i].Name;
		}
		int result = GeDrawAddCompMenu(_availableCompsNames);
		if (result >= 0)
		{
			var compType = _availableCompsTypes[result];
			_ecs.AddCompRaw(compType, gameEnt);
		}
	}

	internal bool DrawField(string name, Type type, ref object val)
	{
		if (!TryGetDrawer(type, out var drawer))
			return false;
		return drawer.DrawRaw(name, ref val);
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
		
		if (type == typeof(SceneObjectComp))
			return false;
		
		if (type.IsEnum)
		{
			var drawerType = typeof(EditorDrawerEnum<>).MakeGenericType(type);
			drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this, _ecs);
			_drawers.Add(type, drawer);
			return true;
		}
		if (type.GetInterface(nameof(SyEcs.IInternalComp)) != null)
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
				drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this, _ecs);
				_drawers.Add(type, drawer);
				return true;
			}
			if (defType == typeof(List<>))
			{
				var drawerType = typeof(EditorDrawerList<,>).MakeGenericType(type, genType);
				drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this, _ecs);
				_drawers.Add(type, drawer);
				return true;
			}
		}
		if (type.IsArray || type.IsDotNetType())
		{
			//TODO
			return false;
		}
		if (type.IsClass || type.IsValueType && !type.IsPrimitive)
		{
			var drawerType = typeof(EditorDrawerReflect<>).MakeGenericType(type);
			drawer = (IEditorDrawer)Activator.CreateInstance(drawerType, this, _ecs);
			_drawers.Add(type, drawer);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private void EgInit(SyProxyEcs proxyEcs)
	{
		_ecs = proxyEcs.Ecs;

		_drawers = new Dictionary<Type, IEditorDrawer>
		{
			{ typeof(int), new EditorDrawerInt(this, _ecs) },
			{ typeof(float), new EditorDrawerFloat(this, _ecs) },
			{ typeof(bool), new EditorDrawerBool(this, _ecs) },
			{ typeof(SyVector2), new EditorDrawerVector2(this, _ecs) },
			{ typeof(SyVector3), new EditorDrawerVector3(this, _ecs) },
			{ typeof(SyVector4), new EditorDrawerVector4(this, _ecs) },
			{ typeof(SyColor), new EditorDrawerColor(this, _ecs) },
			{ typeof(SyCurve), new EditorDrawerCurve(this, _ecs) },
			{ typeof(SySceneEnt?), new EditorDrawerSceneEntNull(this, _ecs) },

			{ typeof(TransformComp), new EditorDrawerCompTransform(this, _ecs) },
			{ typeof(ParticlesComp), new EditorDrawerCompParticles(this, _ecs) },
			{ typeof(NavMeshComp), new EditorDrawerCompNavMesh(this, _ecs) }
		};

		var compInterfaceType = typeof(SyEcs.IComp);
		foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies())
		{
			foreach (var type in assembly.GetTypes())
				if (!type.IsInterface && compInterfaceType.IsAssignableFrom(type))
					_allCompsTypes.Add(type);
		}
	}

	private void EgDrawEntityComps(uint engineEnt)
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
	internal static extern int GeBeginComp(string name, bool isRemovable);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeEndComp();

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeBeginGroup(string name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeEndGroup();

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeSameLine();
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern void GeDrawText(string name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawIntField(string name, ref int val);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawFloatField(string name, ref float val, float min, float max, float step);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawBoolField(string name, ref bool val);
    
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawVector2Field(string name, ref SyVector2 val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawVector3Field(string name, ref SyVector3 val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawVector4Field(string name, ref SyVector4 val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawCurveField(string name, ref SyCurve curve);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawColorField(string name, bool withAlpha, ref SyColor val);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawEnumField(string name, string[] items, ref int selected);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawResField(string name, EResourceType resType, ref string uuid);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawEntityField(string name, ref bool isValid, ref uint engineEnt);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern int GeDrawArrayItemButtons(int idx);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawButton(string name);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern bool GeDrawFoldout(string name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	internal static extern int GeDrawAddCompMenu(string[] compsNames);
}
}