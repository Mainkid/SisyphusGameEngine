#pragma once
#include "SimpleMath.h"
#include "../Core/Graphics/Buffer.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wrl.h>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include "../Serialization/Serializable.h"

#include "SkyboxResource.h"


struct SkyboxComponent
{
	std::unique_ptr<Buffer> VertexBuffer;
	std::unique_ptr<Buffer> IndexBuffer;
	std::shared_ptr<SkyboxResource> SkyboxRes;
	boost::uuids::uuid uuid;
	size_t Hash;
	SER_COMP(SkyboxComponent,
		uuid)
};
