#pragma once
#include "../../../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../../Mesh/MeshLoader.h"  


#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <math.h>
#include <iostream>
#include "../../../Core/Tools/Structures/Vector.h"
#include "../../../Core/Tools/ErrorLogger.h"
#include "../../Resources/ResourceService.h"
#include "../Components/FSoundComponent.h"
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>



struct HardwareContext;
struct EngineContext;
struct RenderContext;

struct Implementation {

	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* _mpStudioSystem;
	FMOD::System* _mpSystem;

	int _mnNextChannelId;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	//typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
	//typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

	SoundMap _mSounds;
	ChannelMap _mChannels;
};

class SoundSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

	static int ErrorCheck(FMOD_RESULT result);

	//void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	//void LoadEvent(const std::string& strEventName);
	void LoadSound(const boost::uuids::uuid& uuid, bool b3d = true, bool bLooping = false);// , bool bStream = false);
	void UnLoadSound(const boost::uuids::uuid& uuid);
	//void Set3dListenerAndOrientation(const SyVector3& vPos = SyVector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	int PlayFSound(const boost::uuids::uuid& uuid, const SyVector3& vPosition = SyVector3{ 0, 0, 0 }, float fVolumedB = 1.0f);
	//void PlayEvent(const std::string& strEventName);
	//void StopChannel(int nChannelId);
	//void StopEvent(const std::string& strEventName, bool bImmediate = false);
	//void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
	//void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
	//void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const SyVector3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);
	void TogglePauseChannel(int nChannelId,bool isPaused);
	void ToggleStopChannel(int nChannelId);
	bool CheckIsPlaying(int nChannelId) const;
	//bool IsEventPlaying(const std::string& strEventName) const;
	float VolumeRounding(float& volume);
	//float VolumeTodb(float volume);
	FMOD_VECTOR VectorToFmod(const SyVector3& vPosition);


private:
	HardwareContext* _hc = nullptr;
	RenderContext* _rc = nullptr;
	EngineContext* _ec = nullptr;
	ResourceService* _rs = nullptr;

	std::unordered_map<int, boost::uuids::uuid> deletionMap;
	std::unordered_map<int, boost::uuids::uuid> activeMap;

	std::unordered_map<boost::uuids::uuid, int, boost::hash<boost::uuids::uuid>> soundComponentsMap;
	std::unordered_map<boost::uuids::uuid, int, boost::hash<boost::uuids::uuid>> tmpComponentsMap;

	
};

