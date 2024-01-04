using System;
using System.Collections.Generic;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs
{
public enum EEngineCompId
{
	SceneObject,
	Transform,
	Mesh,
	Light,

	Collider,
	Rigid,
	FixedJoint,
	HingeJoint,

	Skybox,
	Particles,
	Sound,
	Camera,
}

public static class EngineCompIdHelper
{
	private static readonly Dictionary<Type, EEngineCompId> _typeToId = new Dictionary<Type, EEngineCompId>()
	{
		{ typeof(SceneObjectComp), EEngineCompId.Mesh },
		{ typeof(TransformComp), EEngineCompId.Transform },
		{ typeof(MeshComp), EEngineCompId.Mesh },
		{ typeof(LightComp), EEngineCompId.Light },
		{ typeof(ColliderComp), EEngineCompId.Collider },
		{ typeof(RigidComp), EEngineCompId.Rigid },
		{ typeof(SkyboxComp), EEngineCompId.Skybox },
		{ typeof(ParticlesComp), EEngineCompId.Particles },
		{ typeof(SoundComp), EEngineCompId.Sound },
		{ typeof(FixedJointComp), EEngineCompId.FixedJoint },
		{ typeof(HingeJointComp), EEngineCompId.HingeJoint },
		{ typeof(CameraComp), EEngineCompId.Camera }
	};

	private static Dictionary<EEngineCompId, Type> _idToType;

	public static EEngineCompId? TypeToId(Type compType)
	{
		if (_typeToId.TryGetValue(compType, out var id))
			return id;
		return null;
	}

	public static Type IdToType(EEngineCompId id)
	{
		if (_idToType == null)
		{
			_idToType = new Dictionary<EEngineCompId, Type>();
			foreach (var pair in _typeToId)
				_idToType[pair.Value] = pair.Key;
		}
		if (_idToType.TryGetValue(id, out var type))
			return type;
		throw new Exception($"type for id {id} is missing");
	}
}
}