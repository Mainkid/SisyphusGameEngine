#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb_image.h"
#include "ImageLoader.h"
#include "../../Engine/Contexts/HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "filesystem"



bool ImageLoader::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	auto hc=ServiceLocator::instance()->Get<HardwareContext>();

	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height,NULL , 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	hc->device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hc->device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

void* ImageLoader::LoadImageFromFile(const char* filename, int* out_width, int* out_height)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == nullptr)
		return nullptr;
	*out_width = image_width;
	*out_height = image_height;
	return image_data;
}

std::vector<void*> ImageLoader::LoadSkyboxFromFile(const char* filename, int* out_width, int* out_height)
{
	{
		std::filesystem::path path = filename;
		std::string extension = path.extension().string();


		//if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp")



		std::vector<void*> skyboxEdges;
		int image_width = 0;
		int image_height = 0;
		int skybox_dim = 0;
		int channels = 4;
		float* image_data;


		image_data = stbi_loadf(filename, &image_width, &image_height, nullptr, channels);

		skybox_dim = image_width / 4.0f;

		float* edgeXPlus = new float[skybox_dim * skybox_dim * channels];
		float* edgeXMinus = new float[skybox_dim * skybox_dim * channels];
		float* edgeYPlus = new float[skybox_dim * skybox_dim * channels];
		float* edgeYMinus = new float[skybox_dim * skybox_dim * channels];
		float* edgeZPlus = new float[skybox_dim * skybox_dim * channels];
		float* edgeZMinus = new float[skybox_dim * skybox_dim * channels];

		for (int i = 0; i < skybox_dim; i++)
			for (int j = 0; j < skybox_dim * channels; j += channels)
			{
				for (int k = 0; k < channels; k++)
				{
					edgeXPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + image_width * channels * i + j + k];
					edgeXMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels * 2 + image_width * channels * i + j + k];
					edgeYPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels + image_width * channels * i + j + k];
					edgeYMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width * 2 + skybox_dim * channels + image_width * channels * i + j + k];
					edgeZMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels + image_width * channels * i + j + k];
					edgeZPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels * 3 + image_width * channels * i + j + k];
				}
			}

		skyboxEdges.push_back(edgeXMinus);
		skyboxEdges.push_back(edgeXPlus);
		skyboxEdges.push_back(edgeYPlus);
		skyboxEdges.push_back(edgeYMinus);
		skyboxEdges.push_back(edgeZMinus);
		skyboxEdges.push_back(edgeZPlus);

		*out_width = skybox_dim;
		*out_height = skybox_dim;
		std::string out = "out.png";

		return skyboxEdges;
	}
}

