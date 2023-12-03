using System;
using System.Collections.Generic;
using SyEngine.Core.Comps;

namespace SyEngine.Core
{
public enum EEngineCompId
{
	Transform,
	Mesh,
	Light,
	Collider,
	Rigid
}

public static class EngineCompIdHelper
{
	private static readonly Dictionary<Type, EEngineCompId> _map = new Dictionary<Type, EEngineCompId>()
	{
		{typeof(TransformComp), EEngineCompId.Transform},
		{typeof(MeshComp), EEngineCompId.Mesh},
		{typeof(LightComp), EEngineCompId.Light},
		{typeof(ColliderComp), EEngineCompId.Collider},
		{typeof(RigidComp), EEngineCompId.Rigid},
	};

	public static EEngineCompId? GetFromCompType(Type compType)
	{
		if (_map.TryGetValue(compType, out var id))
			return id;
		return null;
	}
}
}