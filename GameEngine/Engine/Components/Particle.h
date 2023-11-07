#pragma once
#include "ResourceBase.h"
#include "SimpleMath.h"
#include <random>
#include "../../boost/boost/uuid/uuid.hpp"
#include "../Serialization/Serializable.h"

struct ParticleInputDataF
{
    float Fvalue;                                     // Single Float Input;
    std::vector<float> RandomBetweenConstsF; // Random Between Two Constants;
    //CurveClass curve;                               // TODO: Add Curves;

	SER_DATA(ParticleInputDataF, Fvalue, RandomBetweenConstsF);
};

struct ParticleInputDataV3
{
    DirectX::SimpleMath::Vector3 V3value;
    std::vector<DirectX::SimpleMath::Vector3> RandomBetweenConstsV;

    SER_DATA(ParticleInputDataV3, V3value, RandomBetweenConstsV);
};

struct ParticleBurst
{
    float Time;                                       // Simulation time when burst will be triggered;
    ParticleInputDataF Count;                         // Particles to emmit;
    //Interval;
    float Probability;                                // Probability of the burst;

    SER_DATA(ParticleBurst, Time, Count, Probability);
};

enum EParticleEmitShape
{
	Sphere,
    Cone
};

struct SharedParticlesData:public ResourceBase
{
    //Base parameters
    float Duration;                                   // The length of time Particle System emmiting particles;
    bool IsLooping;                                   // Is looping?
    ParticleInputDataF StartDelayTime;                // Delay before emitting;
    ParticleInputDataF StartLifeTime;                 // Lifetime of particles;
    ParticleInputDataF StartSpeed;                    // Speed, applied to the particle when created;
    ParticleInputDataF StartSize;                     // StartSize of particle;
    ParticleInputDataV3 StartColor;                   // Start Color;
    uint32_t MaxParticles;

    //Emission
    ParticleInputDataF RateOverTime;                  // Particles emitted per second;
    std::vector<ParticleBurst> ParticleBursts;        // Particles Bursts;

    //Shape
    EParticleEmitShape ParticleEmitShape;
    float Angle;
    float Radius;



    SER_DATA(SharedParticlesData, Duration,
        IsLooping, StartDelayTime, StartLifeTime,
        StartSpeed, StartSize, StartColor, MaxParticles, RateOverTime, ParticleBursts,
        ParticleEmitShape, Angle, Radius
    )
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

