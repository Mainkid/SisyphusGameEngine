#include "SoundSystem.h"

SyResult SoundSystem::Init()
{
    //sgpImplementation = new FAudioImp;
    return SyResult();
}

SyResult SoundSystem::Run()
{
  
    return SyResult();
}

SyResult SoundSystem::Destroy()
{
    return SyResult();
}



/////////////////////////////////////////////////////////////
//void SoundSystem::Update() 
//{
//    sgpImplementation->Update();
//}
//
//int SoundSystem::ErrorCheck(FMOD_RESULT result)
//{
//    if (result != FMOD_OK) {
//        std::cout << "FMOD ERROR " << result << std::endl;
//        return 1;
//    }
//    // cout << "FMOD all good" << endl;
//    return 0;
//}
//
//void SoundSystem::Shutdown()
//{
//    delete sgpImplementation;
//}
//
//void SoundSystem::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
//{
//    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
//    if (tFoundIt != sgpImplementation->mSounds.end())
//        return;
//
//    FMOD_MODE eMode = FMOD_DEFAULT;
//    eMode |= b3d ? FMOD_3D : FMOD_2D;
//    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
//    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
//
//    FMOD::Sound* pSound = nullptr;
//    SoundSystem::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
//    if (pSound) {
//        sgpImplementation->mSounds[strSoundName] = pSound;
//    }
//
//}
//
//void SoundSystem::UnLoadSound(const std::string& strSoundName)
//{
//    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
//    if (tFoundIt == sgpImplementation->mSounds.end())
//        return;
//
//    SoundSystem::ErrorCheck(tFoundIt->second->release());
//    sgpImplementation->mSounds.erase(tFoundIt);
//}
//
//void SoundSystem::PlayMSound(const std::string& strSoundName, const DirectX::SimpleMath::Vector3& vPosition, float fVolumedB)
//{
//    int nChannelId = sgpImplementation->mnNextChannelId++;
//    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
//    if (tFoundIt == sgpImplementation->mSounds.end())
//    {
//        LoadSound(strSoundName);
//        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
// /*       if (tFoundIt == sgpImplementation->mSounds.end())
//        {
//            return nChannelId;
//        }*/
//    }
//    FMOD::Channel* pChannel = nullptr;
//    SoundSystem::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
//    if (pChannel)
//    {
//        FMOD_MODE currMode;
//        tFoundIt->second->getMode(&currMode);
//        if (currMode & FMOD_3D) {
//            FMOD_VECTOR position = VectorToFmod(vPosition);
//            SoundSystem::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
//        }
//        SoundSystem::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
//        SoundSystem::ErrorCheck(pChannel->setPaused(false));
//        sgpImplementation->mChannels[nChannelId] = pChannel;
//    }
//    //return nChannelId;
//}
//
//void SoundSystem::SetChannel3dPosition(int nChannelId, const DirectX::SimpleMath::Vector3& vPosition)
//{
//    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
//    if (tFoundIt == sgpImplementation->mChannels.end())
//        return;
//
//    FMOD_VECTOR position = VectorToFmod(vPosition);
//    SoundSystem::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
//}
//
//void SoundSystem::SetChannelVolume(int nChannelId, float fVolumedB)
//{
//    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
//    if (tFoundIt == sgpImplementation->mChannels.end())
//        return;
//
//    SoundSystem::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
//}
//
//void SoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
//    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
//    if (tFoundIt != sgpImplementation->mBanks.end())
//        return;
//    FMOD::Studio::Bank* pBank;
//    SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
//    if (pBank) {
//        sgpImplementation->mBanks[strBankName] = pBank;
//    }
//}
//
//void SoundSystem::LoadEvent(const std::string& strEventName) {
//    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundit != sgpImplementation->mEvents.end())
//        return;
//    FMOD::Studio::EventDescription* pEventDescription = NULL;
//    SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
//    if (pEventDescription) {
//        FMOD::Studio::EventInstance* pEventInstance = NULL;
//        SoundSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
//        if (pEventInstance) {
//            sgpImplementation->mEvents[strEventName] = pEventInstance;
//        }
//    }
//}
//
//void SoundSystem::PlayEvent(const  std::string& strEventName) {
//    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundit == sgpImplementation->mEvents.end()) {
//        LoadEvent(strEventName);
//        tFoundit = sgpImplementation->mEvents.find(strEventName);
//        if (tFoundit == sgpImplementation->mEvents.end())
//            return;
//    }
//    tFoundit->second->start();
//}
//
//void SoundSystem::StopEvent(const  std::string& strEventName, bool bImmediate) {
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return;
//
//    FMOD_STUDIO_STOP_MODE eMode;
//    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
//    SoundSystem::ErrorCheck(tFoundIt->second->stop(eMode));
//}
//
//bool SoundSystem::IsEventPlaying(const  std::string& strEventName) const {
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return false;
//
//    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
//    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
//        return true;
//    }
//    return false;
//}
//
////void SoundSystem::GetEventParameter(const  std::string& strEventName, const  std::string& strEventParameter, float* parameter) {
////    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
////    if (tFoundIt == sgpImplementation->mEvents.end())
////        return;
////
////    FMOD::Studio::ParameterInstance* pParameter = NULL;
////    SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strEventParameter.c_str(), &pParameter));
////    SoundSystem::ErrorCheck(pParameter->getValue(parameter));
////}
////
////void SoundSystem::SetEventParameter(const  std::string& strEventName, const  std::string& strParameterName, float fValue) {
////    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
////    if (tFoundIt == sgpImplementation->mEvents.end())
////        return;
////
////    FMOD::Studio::ParameterInstance* pParameter = NULL;
////    SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
////    SoundSystem::ErrorCheck(pParameter->setValue(fValue));
////}
//
//float  SoundSystem::dbToVolume(float dB)
//{
//    return powf(10.0f, 0.05f * dB);
//}
//
//float  SoundSystem::VolumeTodb(float volume)
//{
//    return 20.0f * log10f(volume);
//}
//
//FMOD_VECTOR SoundSystem::VectorToFmod(const DirectX::SimpleMath::Vector3& vPosition) {
//    FMOD_VECTOR fVec;
//    fVec.x = vPosition.x;
//    fVec.y = vPosition.y;
//    fVec.z = vPosition.z;
//    return fVec;
//}