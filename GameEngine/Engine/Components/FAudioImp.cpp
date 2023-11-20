//#include "FAudioImp.h"
//
//FAudioImp::~FAudioImp()
//{
//    FAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
//    FAudioEngine::ErrorCheck(mpStudioSystem->release());
//}
//
//FAudioImp::FAudioImp()
//{
//    mpStudioSystem = NULL;
//    FAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
//    FAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
//
//    mpSystem = NULL;
//    FAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
//}
//
//void FAudioImp::Update()
//{
//    std::vector<ChannelMap::iterator> pStoppedChannels;
//    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
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
//        mChannels.erase(it);
//    }
//    FAudioEngine::ErrorCheck(mpStudioSystem->update());
//}


/////////////////////////////////////////
//FAudioImp::~FAudioImp()
//{
//    SoundSystem::ErrorCheck(mpStudioSystem->unloadAll());
//    SoundSystem::ErrorCheck(mpStudioSystem->release());
//}
//
//FAudioImp::FAudioImp()
//{
//    mpStudioSystem = NULL;
//    SoundSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
//    SoundSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
//
//    mpSystem = NULL;
//    SoundSystem::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
//}
//
//void FAudioImp::Update()
//{
//    std::vector<ChannelMap::iterator> pStoppedChannels;
//    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
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
//        mChannels.erase(it);
//    }
//    SoundSystem::ErrorCheck(mpStudioSystem->update());
//}