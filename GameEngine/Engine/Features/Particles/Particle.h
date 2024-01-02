#pragma once
#include "../Resources/ResourceBase.h"
#include "SimpleMath.h"
#include <random>
#include "../../boost/boost/uuid/uuid.hpp"
#include "../../Serialization/Serializable.h"

enum EInputType
{
	SimpleFloat,
    RandomBetweenFloats,
    SimpleVector,
    RandomBetweenVectors,
    SimpleCurve
};

struct FloatPair
{
    float f1 = 0.0f;
    float f2 = 0.0f;

    //SER_DATA(FloatPair, f1,f2);
};

struct VectorPair
{
    DirectX::SimpleMath::Vector4 vec1 = DirectX::SimpleMath::Vector4::Zero;
    DirectX::SimpleMath::Vector4 vec2 = DirectX::SimpleMath::Vector4::Zero;

    //SER_DATA(VectorPair, vec1, vec2);
};

struct UniformCurve
{
    DirectX::SimpleMath::Vector2 P0;
    DirectX::SimpleMath::Vector2 P1;
    DirectX::SimpleMath::Vector2 P2;
    DirectX::SimpleMath::Vector2 P3;
    DirectX::SimpleMath::Vector4 IsUsing;
    bool IsReversed;

    //SER_DATA(UniformCurve, P0, P1, P2, P3, IsUsing);
};

struct ParticleInputDataF
{
    float Fvalue = 0.0f;                                     
    FloatPair RandomBetweenConstsF;
    UniformCurve Curve;
    DirectX::SimpleMath::Vector4 V4value;
    VectorPair RandomBetweenConstsV;
    EInputType InputType = SimpleFloat;

	//SER_DATA(ParticleInputDataF, Fvalue, RandomBetweenConstsF, InputType);
};

//struct ParticleInputDataV4
//{
//    
//    EInputType InputType = SimpleVector;
//
//    //SER_DATA(ParticleInputDataV4, V4value, RandomBetweenConstsV, InputType);
//};

struct ParticleBurst
{
    float Time;                                       // Simulation time when burst will be triggered;
    float TimeSinceLastBurst = 0.0f;                  
    ParticleInputDataF Count;                         // Particles to emmit;
    bool IsLooping;
    float Probability;                                // Probability of the burst;

    //SER_DATA(ParticleBurst, Time, Count, Probability, IsLooping);
};

enum EParticleEmitShape
{
	Sphere,
    Cone
};

struct Particle
{
    DirectX::SimpleMath::Vector4 pos = DirectX::SimpleMath::Vector4::Zero;
    DirectX::SimpleMath::Vector4 size = DirectX::SimpleMath::Vector4::Zero;
    DirectX::SimpleMath::Vector4 velocity = DirectX::SimpleMath::Vector4(1, 0, 0, 0);
    DirectX::SimpleMath::Vector4 lifeTime = DirectX::SimpleMath::Vector4(0.0f, 5.0f, 5.0, 1.0f);
    DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
    DirectX::SimpleMath::Vector4 state = DirectX::SimpleMath::Vector4(false, false, false, false);
    DirectX::SimpleMath::Vector4 shapeAngleRadius = DirectX::SimpleMath::Vector4(0, 0, 0, 0);

};

struct SortListParticle
{
    SortListParticle()
    {

    }
    SortListParticle(int ind, float dist)
    {
        index = ind;
        distance = dist;
    }
    int index;
    float distance;
};

