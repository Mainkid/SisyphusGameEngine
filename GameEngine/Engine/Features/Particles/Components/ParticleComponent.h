#pragma once
#include <math.h>
#include "../../../Core/Graphics/Shader.h"
#include "../../../Contexts/HardwareContext.h"
#include "../Particle.h"
#include "../../../Components/Texture.h"
#include "../../../Core/Rendering/Lights/LightData.h"
#include "../../../../vendor/boost/boost/uuid/uuid.hpp"
#include "../../../../vendor/boost/boost/uuid/nil_generator.hpp"
#include "../../../../vendor/boost/boost/lexical_cast.hpp"

struct CB
{
	UINT iLevel;
	UINT iLevelMask;
	UINT iWidth;
	UINT iHeight;
};

struct ParticleComponent
{
	ParticleComponent()
	{
		TimeFromStart = 0.0f;

		Angle = 60.0f;
		Duration = 5.0f;
		IsLooping = true;
		MaxParticles = 4096;
		IsLit = true;
		AmbientAmount = 0.5f;
		std::string str = "7a87a4dd-e675-4bd4-9c46-3e48b4761a7a";
		TextureUuid = boost::uuids::nil_uuid();

		ParticleBurst burst;
		ParticleInputDataF inputDataF;
		inputDataF.InputType = EInputType::SimpleFloat;
		inputDataF.Fvalue = 100.0f;
		inputDataF.RandomBetweenConstsF={ 0,0 };

		burst.Count = inputDataF;
		burst.IsLooping = true;
		burst.Probability = 0.9f;
		burst.Time = 2.0f;
		ParticleBursts.push_back(burst);


		ParticleEmitShape = EParticleEmitShape::Cone;
		Radius = 0.2f;
		inputDataF.Fvalue = 150.0f;
		RateOverTime = inputDataF;

		inputDataF.Fvalue = 100.0f;
		inputDataF.InputType = EInputType::RandomBetweenFloats;
		inputDataF.RandomBetweenConstsF = { -60,60 };
		RotationOverLifetime = inputDataF;

		SizeOverLifetime.Curve.IsUsing = Vector4(1, 1, 1, 1);
		SizeOverLifetime.Curve.P0 = Vector2(0, 0);
		SizeOverLifetime.Curve.P1 = Vector2(0.172f, 0.977f);
		SizeOverLifetime.Curve.P2 = Vector2(0.961f, 0.476f);
		SizeOverLifetime.Curve.P3 = Vector2(1.0f, 1.0f);

		SpeedOverLifetime.Curve.IsUsing = Vector4(1, 1, 1, 1);
		SpeedOverLifetime.Curve.P0 = Vector2(0, 0);
		SpeedOverLifetime.Curve.P1 = Vector2(0, 1);
		SpeedOverLifetime.Curve.P2 = Vector2(1, 0);
		SpeedOverLifetime.Curve.P3 = Vector2(0, 0);

		StartColor.InputType = EInputType::SimpleVector;
		StartColor.V4value = Vector4(1, 1, 1, 1);

		StartDelayTime.InputType = EInputType::SimpleFloat;
		StartDelayTime.Fvalue = 0.0f;

		StartLifeTime.Fvalue = 5.0f;
		StartLifeTime.InputType = EInputType::RandomBetweenFloats;
		StartLifeTime.RandomBetweenConstsF = { 4.0f, 5.0f };

		StartSize.InputType = EInputType::RandomBetweenFloats;
		StartSize.RandomBetweenConstsF = { 0.5f,0.8f };

		StartRotation.RandomBetweenConstsF = { -40.0f, 40.0f };
		StartRotation.InputType = EInputType::RandomBetweenFloats;

		StartSpeed.InputType = EInputType::SimpleFloat;
		StartSpeed.Fvalue = 2.0f;
	}

	//----User vars----
	float Duration;                                   // The length of time Particle System emmiting particles;
	bool IsLooping;                                   // Is looping?
	ParticleInputDataF StartDelayTime;                // Delay before emitting;	[float/random between floats]
	ParticleInputDataF StartLifeTime;                 // Lifetime of particles;	[float/random between floats]
	ParticleInputDataF StartSpeed;                    // Speed, applied to the particle when created;	[float/random between floats]
	ParticleInputDataF StartSize;                     // StartSize of particle;	[float/random between floats]
	ParticleInputDataF StartColor;                   // Start Color; [vector/random between vectors]
	ParticleInputDataF StartRotation;				  // [float/random between floats]
	ParticleInputDataF SizeOverLifetime;					  // [curve]
	ParticleInputDataF SpeedOverLifetime;					  // [curve]
	ParticleInputDataF RotationOverLifetime;		  // [float/random between floats]
	uint32_t MaxParticles;
	bool IsLit;
	float AmbientAmount;

	//Emission
	ParticleInputDataF RateOverTime;                  // Particles emitted per second;
	std::vector<ParticleBurst> ParticleBursts;        // Particles Bursts;

	//Shape
	EParticleEmitShape ParticleEmitShape;
	float Angle;
	float Radius;

	boost::uuids::uuid TextureUuid;
	

	//----Engine vars----
	std::shared_ptr<Texture> ParticleTexture;
	std::vector<Particle> ParticlesList;
	uint32_t GroupSizeX;
	uint32_t GroupSizeY;
	uint32_t NumParticles;
	float TimeFromStart;
	float ParticlesToEmit = 0.0f;
	LightData lightObjects[10];
	bool IsInitialized = false;

	std::unique_ptr<Buffer> PoolBuffer;
	std::unique_ptr<Buffer> DeadListBuffer;
	std::unique_ptr<Buffer> SortListBuffer;
	std::unique_ptr<Buffer> IndirectDrawBuffer;
	std::unique_ptr<Buffer> ConstBuffer;
	std::unique_ptr<Buffer> GroupCountConstBuffer;
	std::unique_ptr<Buffer> CounterBuffer;
	std::unique_ptr<Buffer> SortGpuConstBuffer;
	std::unique_ptr<Buffer> TmpGpuBuffer;
	std::unique_ptr<Buffer> IndexBuffer;
	Microsoft::WRL::ComPtr< ID3D11Buffer> CounterCpuBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PoolBufferSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SortBufferSrv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> PoolBufferUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> DeadListUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> SortBufferUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> CounterUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CounterSrv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> TmpGpuBufferUav;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TmpGpuBufferSrv;

	std::vector<SortListParticle> data = std::vector<SortListParticle>(1);

	std::vector<int> Indices;
	std::vector<int> IndexList;
	size_t Hash = 0;


	bool IsMonoDirty = true; //used only by mono sync systems
	

	//SER_COMP(ParticleComponent, SharedParticlesDataUuid)

};

