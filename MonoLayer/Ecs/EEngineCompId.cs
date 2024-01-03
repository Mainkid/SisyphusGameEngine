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
}

public static class EngineCompIdHelper
{
	private static readonly Dictionary<Type, EEngineCompId> _map = new Dictionary<Type, EEngineCompId>()
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
		{ typeof(HingeJointComp), EEngineCompId.HingeJoint }
	};

	public static EEngineCompId? GetFromCompType(Type compType)
	{
		if (_map.TryGetValue(compType, out var id))
			return id;
		return null;
	}
}
}