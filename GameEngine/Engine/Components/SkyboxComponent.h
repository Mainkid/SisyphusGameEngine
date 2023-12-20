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
	//----User vars----
	boost::uuids::uuid uuid;

	//----Engine vars----
	std::unique_ptr<Buffer> VertexBuffer;
	std::unique_ptr<Buffer> IndexBuffer;
	std::shared_ptr<SkyboxResource> SkyboxRes;
	size_t Hash;

	size_t MonoHash; // read/write only by mono sync system


	SER_COMP(SkyboxComponent,
		uuid)
};
