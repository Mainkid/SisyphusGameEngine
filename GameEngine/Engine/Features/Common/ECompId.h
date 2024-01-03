#pragma once

enum class ECompId
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
};

inline std::ostream& operator<<(std::ostream& os, ECompId id)
{
	switch (id)
	{
	case ECompId::SceneObject:
		os << "SceneObject";
		break;
	case ECompId::Transform:
		os << "Transform";
		break;
	case ECompId::Mesh:
		os << "Mesh";
		break;
	case ECompId::Light:
		os << "Light";
		break;
	case ECompId::Collider:
		os << "Collider";
		break;
	case ECompId::Rigid:
		os << "Rigid";
		break;
	case ECompId::FixedJoint:
		os << "FixedJoint";
		break;
	case ECompId::HingeJoint:
		os << "HingeJoint";
		break;
	case ECompId::Skybox:
		os << "Skybox";
		break;
	case ECompId::Particles:
		os << "Particles";
		break;
	case ECompId::Sound:
		os << "Sound";
		break;
	default: 
		os << "Missing";
	}
	return os;
}

/*
class ECompIdExt
{
public:
	static constexpr std::string ToStr(ECompIdExt id)
	{
		switch (id)
		{
		case ECompId::SceneObject: return "SceneObject";
		case ECompId::Transform: return "Transform";
		case ECompId::Mesh: return "Mesh";
		case ECompId::Light: return "Light";

		case ECompId::Collider: return "Collider";
		case ECompId::Rigid: return "Rigid";
		case ECompId::FixedJoint: return "FixedJoint";
		case ECompId::HingeJoint: return "HingeJoint";

		case ECompId::Skybox: return "Skybox";
		case ECompId::Particles: return "Particles";
		case ECompId::Sound: return "Sound";
		default: return "Missing";
		}
	}
};
*/