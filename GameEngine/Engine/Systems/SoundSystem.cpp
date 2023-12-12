#include "SoundSystem.h"
#include "../Components/TransformComponent.h"
//#include "../Components/CameraComponent.h"
#include "../Scene/CameraHelper.h"

Implementation::Implementation()
{
    _mnNextChannelId = 0;
    _mpStudioSystem = NULL;
    SoundSystem::ErrorCheck(FMOD::Studio::System::create(&_mpStudioSystem));
    SoundSystem::ErrorCheck(_mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    _mpSystem = NULL;
    SoundSystem::ErrorCheck(_mpStudioSystem->getCoreSystem(&_mpSystem));
}

Implementation::~Implementation() {
    SoundSystem::ErrorCheck(_mpStudioSystem->unloadAll());
    SoundSystem::ErrorCheck(_mpStudioSystem->release());
}

void Implementation::Update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = _mChannels.begin(), itEnd = _mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        _mChannels.erase(it);
    }
    // SoundSystem::ErrorCheck(_mpStudioSystem->Update());
}



Implementation* sgpImplementation = nullptr;

//============================================================
SyResult SoundSystem::Init()
{
    SY_EL->RegisterChannel("AUDI");
    sgpImplementation = new Implementation;
    return SyResult();
}

SyResult SoundSystem::Run()
{
    auto View = _ecs->view<FSoundComponent>();
    for (auto& Entity : View)
    {
        auto& Scom = _ecs->get<FSoundComponent>(Entity);
        std::string name = Scom.SoundPath;
          
       // on
       if (Scom.IsPlaying)
       {

           //auto& rigidBodyC = _ecs->get<SyRBodyComponent>(entity);
           //auto* transformC = _ecs->try_get<TransformComponent>(entity);
           //if (transformC == nullptr)
           //{
           //    _ecs->emplace<TransformComponent>(entity);
           //    CallEvent<SyOnAddComponentEvent>("AddTransform", SyEComponentTypes::TRANSFORM, entity);
           //    SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
           //        "Transform Component required for RigidBody Component is missing on entity (%s). The Transform Component has been added in the next frame.",
           //        SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
           //    continue;
           //}


           if (!Scom.IsON)
           {
                Scom.IsON = true;
                LoadSound(name, Scom.Sound3D, Scom.IsLooping);
                auto [ñameraComponent, ñameraTransform] = CameraHelper::Find(_ecs);
                TransformComponent& tc = _ecs->get<TransformComponent>(Entity);
                Scom.ChanelID = PlayFSound(name, tc._position, Scom.Volume);

                Scom.Sound3D ?
                    ((Scom.ChanelID = PlayFSound (   name,
                                                     tc._position - ñameraTransform._position,
                                                     Scom.Volume)))
                    : (Scom.ChanelID = PlayFSound(name));
           }

           SetChannelVolume(Scom.ChanelID, Scom.Volume);

           if (Scom.Sound3D)
           {
               auto [ñameraComponent, ñameraTransform] = CameraHelper::Find(_ecs);
               TransformComponent& tc = _ecs->get<TransformComponent>(Entity);
               SetChannel3dPosition(Scom.ChanelID, tc._position - ñameraTransform._position);
           }
       }
       //off
       if (!Scom.IsPlaying)
       {
           Scom.IsON = false;
           UnLoadSound(name);
       }  


       // auto off
       if (Scom.IsON)
       {
           bool bIsPlaying = false;
           sgpImplementation->_mChannels.find(Scom.ChanelID)->second->isPlaying(&bIsPlaying);
          
           if (!bIsPlaying)
           {
               Scom.IsON = false;
               Scom.IsPlaying = false;
               UnLoadSound(name);
           }
       } 
    }
    sgpImplementation->Update();
    return SyResult();
}

SyResult SoundSystem::Destroy()
{
// TODO S: I can do this (UnLoad all Sound( ok this)?
// A: Seems to be okay.
    auto View = _ecs->view<FSoundComponent>();
    for (auto& Entity : View)
    {
        auto& Scom = _ecs->get<FSoundComponent>(Entity);
        std::string name = Scom.SoundPath;
        UnLoadSound(name);
    }
 
    delete sgpImplementation;
    return SyResult();
}
//============================================================


int SoundSystem::ErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK) {
        SY_LOG_AUDI(SY_LOGLEVEL_ERROR, "FMOD ERROR");
        return 1;
    }
    return 0;
}

void SoundSystem::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping)//, bool bStream)
{
    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->_mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    //eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    SoundSystem::ErrorCheck(sgpImplementation->_mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        sgpImplementation->_mSounds[strSoundName] = pSound;
    }

}

void SoundSystem::UnLoadSound(const std::string& strSoundName)
{
    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->_mSounds.end())
        return;

    SoundSystem::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->_mSounds.erase(tFoundIt);
}

void SoundSystem::SetChannel3dPosition(int nChannelId, const SyVector3& vPosition)
{
    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->_mChannels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(vPosition);
    SoundSystem::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void SoundSystem::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->_mChannels.end())
        return;

    SoundSystem::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

//void SoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
//    auto tFoundIt = sgpImplementation->_mBanks.find(strBankName);
//    if (tFoundIt != sgpImplementation->_mBanks.end())
//        return;
//    FMOD::Studio::Bank* pBank;
//    SoundSystem::ErrorCheck(sgpImplementation->_mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
//    if (pBank) {
//        sgpImplementation->_mBanks[strBankName] = pBank;
//    }
//}
 
//void SoundSystem::LoadEvent(const std::string& strEventName) {
//    auto tFoundit = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundit != sgpImplementation->_mEvents.end())
//        return;
//    FMOD::Studio::EventDescription* pEventDescription = NULL;
//    SoundSystem::ErrorCheck(sgpImplementation->_mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
//    if (pEventDescription) {
//        FMOD::Studio::EventInstance* pEventInstance = NULL;
//        SoundSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
//        if (pEventInstance) {
//            sgpImplementation->_mEvents[strEventName] = pEventInstance;
//        }
//    }
//}

int SoundSystem::PlayFSound(const std::string& strSoundName, const SyVector3& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->_mnNextChannelId++;
    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->_mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->_mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    SoundSystem::ErrorCheck(sgpImplementation->_mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = VectorToFmod(vPosition);
            SoundSystem::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        SoundSystem::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        SoundSystem::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->_mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

//void SoundSystem::PlayEvent(const  std::string& strEventName) {
//    auto tFoundit = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundit == sgpImplementation->_mEvents.end()) {
//        LoadEvent(strEventName);
//        tFoundit = sgpImplementation->_mEvents.find(strEventName);
//        if (tFoundit == sgpImplementation->_mEvents.end())
//            return;
//    }
//    tFoundit->second->start();
//}

//void SoundSystem::StopEvent(const  std::string& strEventName, bool bImmediate) {
//    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->_mEvents.end())
//        return;
//
//    FMOD_STUDIO_STOP_MODE eMode;
//    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
//    SoundSystem::ErrorCheck(tFoundIt->second->stop(eMode));
//}

//bool SoundSystem::IsEventPlaying(const  std::string& strEventName) const 
//{
//    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->_mEvents.end())
//        return false;
//
//    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
//    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
//        return true;
//    }
//    return false;
//}

//void SoundSystem::GetEventParameter(const  std::string& strEventName, const  std::string& strEventParameter, float* parameter) {
//    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->_mEvents.end())
//        return;
//
//    FMOD::Studio::getCoreSystem* pParameter = NULL;
//    SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strEventParameter.c_str(), &pParameter));
//    SoundSystem::ErrorCheck(pParameter->getValue(parameter));
//}
//
//void SoundSystem::SetEventParameter(const  std::string& strEventName, const  std::string& strParameterName, float fValue) {
//    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->_mEvents.end())
//        return;
//
//    FMOD::Studio::ParameterInstance* pParameter = NULL;
//    SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
//    SoundSystem::ErrorCheck(pParameter->setValue(fValue));
//}

float  SoundSystem::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

//float  SoundSystem::VolumeTodb(float volume)
//{
//    return 20.0f * log10f(volume);
//}

FMOD_VECTOR SoundSystem::VectorToFmod(const SyVector3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}