#pragma once
#include "ResourceBase.h"
#include "SimpleMath.h"
#include <random>
#include "../../boost/boost/uuid/uuid.hpp"

struct ParticleInputDataF
{
    float Fvalue;                                     // Single Float Input;
    std::pair<float,float> RandomBetweenConstsF; // Random Between Two Constants;
    //CurveClass curve;                               // TODO: Add Curves;
};

struct ParticleInputDataV3
{
    DirectX::SimpleMath::Vector3 V3value;
    std::pair<DirectX::SimpleMath::Vector3,
        DirectX::SimpleMath::Vector3> RandomBetweenConstsV;

};

struct ParticleBurst
{
    float Time;                                       // Simulation time when burst will be triggered;
    ParticleInputDataF Count;                         // Particles to emmit;
    //Interval;
    float Probability;                                // Probability of the burst;
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

};

struct Particle
{
    boost::uuids::uuid ParticleSystemUuid;
	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Vector3 Color;
	DirectX::SimpleMath::Vector3 Velocity;
	float LifeTime;  
	float Duration;
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

