#include "FAudioEngine.h"


Implementation::Implementation()
{
    mnNextChannelId = 0;
    mpStudioSystem = NULL;
    FAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    FAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
   
    mpSystem = NULL;
    FAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation() {
    FAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
    FAudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
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
        mChannels.erase(it);
    }
    FAudioEngine::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;

void FAudioEngine::Init()
{
  sgpImplementation = new Implementation;
}

void FAudioEngine::Update() 
{
    sgpImplementation->Update();
}

int FAudioEngine::ErrorCheck(FMOD_RESULT result) 
{
    if (result != FMOD_OK) {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    std::cout << "FMOD all good" << std::endl;
    return 0;
}

void FAudioEngine::Shutdown()
{
    delete sgpImplementation;
}

void FAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    FAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }

}

void FAudioEngine::UnLoadSound(const std::string& strSoundName)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;

    FAudioEngine::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->mSounds.erase(tFoundIt);
}

void FAudioEngine::SetChannel3dPosition(int nChannelId, const MVector3& vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(vPosition);
    FAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void FAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    FAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void FAudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end())
        return;
    FMOD::Studio::Bank* pBank;
    FAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void FAudioEngine::LoadEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    FAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        FAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

int FAudioEngine::PlayMSound(const std::string& strSoundName, const MVector3& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    FAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = VectorToFmod(vPosition);
            FAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        FAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        FAudioEngine::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void FAudioEngine::PlayEvent(const  std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        LoadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            return;
    }
    tFoundit->second->start();
}

void FAudioEngine::StopEvent(const  std::string& strEventName, bool bImmediate) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    FAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool FAudioEngine::IsEventPlaying(const  std::string& strEventName) const {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

//void FAudioEngine::GetEventParameter(const  std::string& strEventName, const  std::string& strEventParameter, float* parameter) {
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return;
//
//    FMOD::Studio::getCoreSystem* pParameter = NULL;
//    FAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strEventParameter.c_str(), &pParameter));
//    FAudioEngine::ErrorCheck(pParameter->getValue(parameter));
//}
//
//void FAudioEngine::SetEventParameter(const  std::string& strEventName, const  std::string& strParameterName, float fValue) {
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return;
//
//    FMOD::Studio::ParameterInstance* pParameter = NULL;
//    FAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
//    FAudioEngine::ErrorCheck(pParameter->setValue(fValue));
//}

float  FAudioEngine::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float  FAudioEngine::VolumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}

FMOD_VECTOR FAudioEngine::VectorToFmod(const MVector3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}