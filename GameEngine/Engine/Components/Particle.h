#pragma once
#include "../../vendor/DirectXHelpers.h"
#include "../../vendor/SimpleMath.h"
#include <random>

using namespace DirectX::SimpleMath;
struct Particle
{

    Particle()
    {
        float X = (rand() % 1000 - 500) / 500.0f;
        float Y = (rand() % 1000) / 1000.0f;
        float Z = (rand() % 1000 - 500) / 500.0f;
        pos = Vector4(X, Y, Z, 1);
        initialPos = Vector4(X, Y, Z, 1);
        X = (rand() % 1000) / 1000.0f;
        velocity = Vector4(X, 0, 0, 0);
    }

    Vector4 pos;
    Vector4 initialPos;
    //Vector3 prevPos=Vector3::Zero;
    Vector4 velocity = Vector4(1, 0, 0, 0);
    Vector4 lifeTime = Vector4(0.0f, 5.0f, 5.0, 1.0f);  //startLiftime,endLiftime
    Vector4 state = Vector4(false, false, false, false);
    /*Vector3 acceleration=Vector3::Zero;

    float energy=0;
    float size=0;
    float deltaSize=0;

    float weight=0;
    float deltaWeight=0;
    float color[4];
    float colorDelta[4];*/
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

