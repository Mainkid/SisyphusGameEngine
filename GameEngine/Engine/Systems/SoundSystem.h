#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "MeshLoader.h"  //


//#ifndef FAUDIOENGINE
//#define FAUDIOENGINE

//..\..\vendor\FMod\api\studio\inc\

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

//#endif

struct HardwareContext;
struct EngineContext;
struct RenderContext;

class SoundSystem : public SystemBase
{
private:
	

public:

	//FAudioImp* sgpImplementation = nullptr;


	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

	//// ==Run ?
 //  // static 
	//void Update(); 
	////static 
	//void Shutdown();
	////static 
	//int ErrorCheck(FMOD_RESULT result);

	//void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	//void LoadEvent(const std::string& strEventName);
	//void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	//void UnLoadSound(const std::string& strSoundName);
	////void Set3dListenerAndOrientation(const MVector3& vPos = MVector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	//void PlayMSound(const std::string& strSoundName, const DirectX::SimpleMath::Vector3& vPosition = DirectX::SimpleMath::Vector3{ 0, 0, 0 }, float fVolumedB = 1.0f);
	//void PlayEvent(const std::string& strEventName);
	////void StopChannel(int nChannelId);
	//void StopEvent(const std::string& strEventName, bool bImmediate = false);
	//void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
	//void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
	////void StopAllChannels();
	//void SetChannel3dPosition(int nChannelId, const DirectX::SimpleMath::Vector3& vPosition);
	//void SetChannelVolume(int nChannelId, float fVolumedB);
	////bool IsPlaying(int nChannelId) const;
	//bool IsEventPlaying(const std::string& strEventName) const;
	//float dbToVolume(float db);
	//float VolumeTodb(float volume);
	//FMOD_VECTOR VectorToFmod(const DirectX::SimpleMath::Vector3& vPosition);


private:
	HardwareContext* _hc = nullptr;
	RenderContext* _rc = nullptr;
	EngineContext* _ec = nullptr;
};

