#include "SoundSystem.h"
#include "../Components/TransformComponent.h"
#include "../../Engine/Systems/EngineContext.h"
//#include "../Components/CameraComponent.h"
#include "../Core/ECS/Events/SyPlayModeEndedEvent.h"
#include "../Core/ECS/Events/SyPlayModeStartedEvent.h"
#include "../Core/ServiceLocator.h"
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
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rs = ServiceLocator::instance()->Get<ResourceService>();
    sgpImplementation = new Implementation;
    return SyResult();
}

SyResult SoundSystem::Run()
{
    // включить демо imgui и посмотреть как указывать путь
    // один звук если включить у 2-ух компонентов, то ттолько так работает 
    // в интерфейсе до воспроизведения громкость можно вертеть как хочешь

    

    std::set<boost::uuids::uuid> deletionSet;
    std::set<boost::uuids::uuid> activeSet;

    auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPlayModeEndedEvent);
    if (eventView.begin() != eventView.end())
    {
        auto View = _ecs->view<FSoundComponent>();
        for (auto& ent : View)
        {
            auto& scom = _ecs->get<FSoundComponent>(ent);
            scom.State = ESoundState::Disabled;
            UnLoadSound(scom.SoundUuid);
        }
    }

    auto pauseEventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPauseModeEvent);
    if (pauseEventView.begin() != pauseEventView.end())
    {
        auto View = _ecs->view<FSoundComponent>();
        for (auto& ent : View)
        {
            auto& scom = _ecs->get<FSoundComponent>(ent);
            TogglePauseChannel(scom.ChanelID, true);
            scom.State = ESoundState::Paused;
        }
    }

    auto playmodeEventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPlayModeStartedEvent);
    if (playmodeEventView.begin() != playmodeEventView.end())
    {
        auto View = _ecs->view<FSoundComponent>();
        for (auto& ent : View)
        {
            auto& scom = _ecs->get<FSoundComponent>(ent);
            if (scom.State == ESoundState::Paused)
            {
                TogglePauseChannel(scom.ChanelID, false);
                scom.State = ESoundState::Playing;
            }
            else
                scom.State = ESoundState::Disabled;
        }
    }


    if (_ec->playModeState == EngineContext::EPlayModeState::EditorMode)
        return SyResult();


    auto View = _ecs->view<FSoundComponent>();
    for (auto& Entity : View)
    {
        auto& Scom = _ecs->get<FSoundComponent>(Entity);
        //std::string name = Scom.SoundPath;

        if (Scom.SoundUuid == boost::uuids::nil_uuid())
        {
            Scom.SoundUuid = _rs->baseResourceDB[EAssetType::ASSET_SOUND].uuid;
        }
          
       // on
        if (Scom.IsSoundPlaying)
        {
            activeSet.insert(Scom.SoundUuid);
            if (Scom.State == ESoundState::Disabled)
            {
                LoadSound(Scom.SoundUuid, Scom.SoundType3D, Scom.LoopedSound);
                // 3D
                if (Scom.SoundType3D)
                {
                    auto* transformC = _ecs->try_get<TransformComponent>(Entity);

                    // аdd transform comp
                    if (transformC == nullptr)
                    {
                        _ecs->emplace<TransformComponent>(Entity);
                        continue;
                    }

                    else //if (transformC != nullptr)
                    {
                        Scom.State = ESoundState::Playing;
                        auto [сameraComponent, сameraTransform] = CameraHelper::Find(_ecs);
                        TransformComponent& tc = _ecs->get<TransformComponent>(Entity);

                        Scom.ChanelID = PlayFSound( Scom.SoundUuid,
                                                    Vector3::Transform(tc._position, сameraTransform.transformMatrix.Invert()),
                                                    VolumeRounding(Scom.SoundVolume));
                        continue;
                    }
                }

                // 2D
                else //if (!Scom.SoundType3D)
                {
                    Scom.State = ESoundState::Playing;
                   
                    Scom.ChanelID = PlayFSound(Scom.SoundUuid);
                    /*FMOD::Channel* pChannel = nullptr;
                    auto tFoundIt = Scom.SoundPath;
                    sgpImplementation->_mpSystem->playSound(tFoundIt, nullptr, true, &pChannel);*/
                    continue;
                }
            }

            // Volume
            else //if (Scom.IsON)
            {
                SetChannelVolume(Scom.ChanelID, VolumeRounding(Scom.SoundVolume));
                auto [сameraComponent, сameraTransform] = CameraHelper::Find(_ecs);
                TransformComponent& tc = _ecs->get<TransformComponent>(Entity);
                SetChannel3dPosition(Scom.ChanelID, Vector3::Transform(tc._position, сameraTransform.transformMatrix.Invert()));
            }
        }

        if (!CheckIsPlaying(Scom.ChanelID))
        {
            Scom.State = ESoundState::Disabled;
            Scom.IsSoundPlaying = false;
            deletionSet.insert(Scom.SoundUuid);
            continue;
        }

        
    
       //off
       if (!Scom.IsSoundPlaying)
       {
           deletionSet.insert(Scom.SoundUuid);
           Scom.State = ESoundState::Disabled;
       }  
    }

    for (auto& soundPath : deletionSet)
    {
        if (!activeSet.contains(soundPath))
        {
            UnLoadSound(soundPath);
        }
    }
    
    //sgpImplementation->Update();
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
        UnLoadSound(Scom.SoundUuid);
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

void SoundSystem::LoadSound(const boost::uuids::uuid& uuid, bool b3d, bool bLooping)//, bool bStream)
{
    std::string strSoundName = _rs->FindFilePathByUUID(uuid);
    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->_mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    FMOD::Sound* pSound = nullptr;
    SoundSystem::ErrorCheck(sgpImplementation->_mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        sgpImplementation->_mSounds[strSoundName] = pSound;
    }

}

void SoundSystem::UnLoadSound(const boost::uuids::uuid& uuid)
{

    std::string strSoundName = _rs->FindFilePathByUUID(uuid);
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

    SoundSystem::ErrorCheck(tFoundIt->second->setVolume(fVolumedB)); 
}

void SoundSystem::TogglePauseChannel(int nChannelId, bool isPaused)
{
    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->_mChannels.end())
        return;



    auto channel = tFoundIt->second;
    channel->setPaused(isPaused);
    
}

bool SoundSystem::CheckIsPlaying(int nChannelId) const
{
    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->_mChannels.end())
        return false;

    bool isPlaying = false;
    auto channel = tFoundIt->second;
    channel->isPlaying(&isPlaying);
    
    return isPlaying;
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

int SoundSystem::PlayFSound(const boost::uuids::uuid& uuid, const SyVector3& vPosition, float fVolumedB)
{
    std::string strSoundName = _rs->FindFilePathByUUID(uuid);
    int nChannelId = sgpImplementation->_mnNextChannelId++;
    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->_mSounds.end())
    {
        LoadSound(uuid);
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
        SoundSystem::ErrorCheck(pChannel->setVolume(fVolumedB));
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

//float  SoundSystem::dbToVolume(float dB)
//{
//    return powf(10.0f, 0.05f * dB);
//}

float  SoundSystem::VolumeRounding(float& volume)
{
    if (volume <= 0.0f)
        return volume = 0.0f;

    else 
        return volume;
}

FMOD_VECTOR SoundSystem::VectorToFmod(const SyVector3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}