//#include "FAudioEngine.h"
//
//
//Implementation::Implementation()
//{
//    _mnNextChannelId = 0;
//    _mpStudioSystem = NULL;
//    FAudioEngine::ErrorCheck(FMOD::Studio::System::create(&_mpStudioSystem));
//    FAudioEngine::ErrorCheck(_mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
//   
//    _mpSystem = NULL;
//    FAudioEngine::ErrorCheck(_mpStudioSystem->getCoreSystem(&_mpSystem));
//}
//
//Implementation::~Implementation() {
//    FAudioEngine::ErrorCheck(_mpStudioSystem->unloadAll());
//    FAudioEngine::ErrorCheck(_mpStudioSystem->release());
//}
//
//void Implementation::Update() {
//    std::vector<ChannelMap::iterator> pStoppedChannels;
//    for (auto it = _mChannels.begin(), itEnd = _mChannels.end(); it != itEnd; ++it)
//    {
//        bool bIsPlaying = false;
//        it->second->isPlaying(&bIsPlaying);
//        if (!bIsPlaying)
//        {
//            pStoppedChannels.push_back(it);
//        }
//    }
//    for (auto& it : pStoppedChannels)
//    {
//        _mChannels.erase(it);
//    }
//    FAudioEngine::ErrorCheck(_mpStudioSystem->update());
//}
//
//Implementation* sgpImplementation = nullptr;
//
//void FAudioEngine::Init()
//{
//  sgpImplementation = new Implementation;
//}
//
//void FAudioEngine::Update() 
//{
//    sgpImplementation->Update();
//}
//
//int FAudioEngine::ErrorCheck(FMOD_RESULT result) 
//{
//    if (result != FMOD_OK) {
//        std::cout << "FMOD ERROR " << result << std::endl;
//        return 1;
//    }
//    std::cout << "FMOD all good" << std::endl;
//    return 0;
//}
//
//void FAudioEngine::Shutdown()
//{
//    delete sgpImplementation;
//}
//
//void FAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
//{
//    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
//    if (tFoundIt != sgpImplementation->_mSounds.end())
//        return;
//
//    FMOD_MODE eMode = FMOD_DEFAULT;
//    eMode |= b3d ? FMOD_3D : FMOD_2D;
//    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
//    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
//
//    FMOD::Sound* pSound = nullptr;
//    FAudioEngine::ErrorCheck(sgpImplementation->_mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
//    if (pSound) {
//        sgpImplementation->_mSounds[strSoundName] = pSound;
//    }
//
//}
//
//void FAudioEngine::UnLoadSound(const std::string& strSoundName)
//{
//    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
//    if (tFoundIt == sgpImplementation->_mSounds.end())
//        return;
//
//    FAudioEngine::ErrorCheck(tFoundIt->second->release());
//    sgpImplementation->_mSounds.erase(tFoundIt);
//}
//
//void FAudioEngine::SetChannel3dPosition(int nChannelId, const SyVector3& vPosition)
//{
//    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
//    if (tFoundIt == sgpImplementation->_mChannels.end())
//        return;
//
//    FMOD_VECTOR position = VectorToFmod(vPosition);
//    FAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
//}
//
//void FAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
//{
//    auto tFoundIt = sgpImplementation->_mChannels.find(nChannelId);
//    if (tFoundIt == sgpImplementation->_mChannels.end())
//        return;
//
//    FAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
//}
//
//void FAudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
//    auto tFoundIt = sgpImplementation->_mBanks.find(strBankName);
//    if (tFoundIt != sgpImplementation->_mBanks.end())
//        return;
//    FMOD::Studio::Bank* pBank;
//    FAudioEngine::ErrorCheck(sgpImplementation->_mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
//    if (pBank) {
//        sgpImplementation->_mBanks[strBankName] = pBank;
//    }
//}
//
//void FAudioEngine::LoadEvent(const std::string& strEventName) {
//    auto tFoundit = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundit != sgpImplementation->_mEvents.end())
//        return;
//    FMOD::Studio::EventDescription* pEventDescription = NULL;
//    FAudioEngine::ErrorCheck(sgpImplementation->_mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
//    if (pEventDescription) {
//        FMOD::Studio::EventInstance* pEventInstance = NULL;
//        FAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
//        if (pEventInstance) {
//            sgpImplementation->_mEvents[strEventName] = pEventInstance;
//        }
//    }
//}
//
//int FAudioEngine::PlayMSound(const std::string& strSoundName, const SyVector3& vPosition, float fVolumedB)
//{
//    int nChannelId = sgpImplementation->_mnNextChannelId++;
//    auto tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
//    if (tFoundIt == sgpImplementation->_mSounds.end())
//    {
//        LoadSound(strSoundName);
//        tFoundIt = sgpImplementation->_mSounds.find(strSoundName);
//        if (tFoundIt == sgpImplementation->_mSounds.end())
//        {
//            return nChannelId;
//        }
//    }
//    FMOD::Channel* pChannel = nullptr;
//    FAudioEngine::ErrorCheck(sgpImplementation->_mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
//    if (pChannel)
//    {
//        FMOD_MODE currMode;
//        tFoundIt->second->getMode(&currMode);
//        if (currMode & FMOD_3D) {
//            FMOD_VECTOR position = VectorToFmod(vPosition);
//            FAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
//        }
//        FAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
//        FAudioEngine::ErrorCheck(pChannel->setPaused(false));
//        sgpImplementation->_mChannels[nChannelId] = pChannel;
//    }
//    return nChannelId;
//}
//
//void FAudioEngine::PlayEvent(const  std::string& strEventName) {
//    auto tFoundit = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundit == sgpImplementation->_mEvents.end()) {
//        LoadEvent(strEventName);
//        tFoundit = sgpImplementation->_mEvents.find(strEventName);
//        if (tFoundit == sgpImplementation->_mEvents.end())
//            return;
//    }
//    tFoundit->second->start();
//}
//
//void FAudioEngine::StopEvent(const  std::string& strEventName, bool bImmediate) {
//    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->_mEvents.end())
//        return;
//
//    FMOD_STUDIO_STOP_MODE eMode;
//    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
//    FAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
//}
//
//bool FAudioEngine::IsEventPlaying(const  std::string& strEventName) const {
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
//
////void FAudioEngine::GetEventParameter(const  std::string& strEventName, const  std::string& strEventParameter, float* parameter) {
////    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
////    if (tFoundIt == sgpImplementation->_mEvents.end())
////        return;
////
////    FMOD::Studio::getCoreSystem* pParameter = NULL;
////    FAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strEventParameter.c_str(), &pParameter));
////    FAudioEngine::ErrorCheck(pParameter->getValue(parameter));
////}
////
////void FAudioEngine::SetEventParameter(const  std::string& strEventName, const  std::string& strParameterName, float fValue) {
////    auto tFoundIt = sgpImplementation->_mEvents.find(strEventName);
////    if (tFoundIt == sgpImplementation->_mEvents.end())
////        return;
////
////    FMOD::Studio::ParameterInstance* pParameter = NULL;
////    FAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
////    FAudioEngine::ErrorCheck(pParameter->setValue(fValue));
////}
//
//float  FAudioEngine::dbToVolume(float dB)
//{
//    return powf(10.0f, 0.05f * dB);
//}
//
//float  FAudioEngine::VolumeTodb(float volume)
//{
//    return 20.0f * log10f(volume);
//}
//
//FMOD_VECTOR FAudioEngine::VectorToFmod(const SyVector3& vPosition) {
//    FMOD_VECTOR fVec;
//    fVec.x = vPosition.x;
//    fVec.y = vPosition.y;
//    fVec.z = vPosition.z;
//    return fVec;
//}