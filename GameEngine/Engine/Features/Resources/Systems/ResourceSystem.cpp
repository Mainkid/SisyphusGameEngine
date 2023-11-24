#include "ResourceSystem.h"
#include "../../Engine/Features/Resources/ResourceInfo.h"
#include "../../Engine/Contexts/EngineContext.h"
#include "../../Engine/Features/Rendering/HUD/ResourceHelper.h"
#include "../../Engine/Features/Resources/ResourceService.h"
#include "../../Core/ServiceLocator.h"
#include "json.hpp"
#include <uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>


SyResult ResourceSystem::Init()
{
	ec=ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
	rs->GenerateMetaFiles(_gameAssetsPath);
	rs->GenerateMetaFiles(_engineAssetsPath);
	rs->LoadResourceLibrary(_gameAssetsPath);
	rs->LoadResourceLibrary(_engineAssetsPath,false,true);
	rs->LoadBaseAssets();
	return SyResult();
}

SyResult ResourceSystem::Run()
{
	

	return SyResult();
}

SyResult ResourceSystem::Destroy()
{
	return SyResult();
}





