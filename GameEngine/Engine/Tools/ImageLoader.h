#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <vector>
#include "filesystem"


class ImageLoader {
public:
	static bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	static void* LoadImageFromFile(const char* filename, int* out_width, int* out_height);
	static std::vector<void*> LoadSkyboxFromFile(const char* filename, int* out_width, int* out_height);

private:
	//template <typename T>
	//static std::vector<void*> LoadSkyboxOfFormat(const char* filename, int* out_width, int* out_height);

	//template <unsigned char>
	//static void* LoadData(const char* filename, int* out_width, int* out_height);

	//template <float>
	//static void* LoadData(const char* filename, int* out_width, int* out_height);

	//template <typename T>
	//static void* LoadData(const char* filename, int* out_width, int* out_height);
};


//std::vector<void*> ImageLoader::LoadSkyboxOfFormat(const char* filename, int* out_width, int* out_height)
//{
//	std::vector<void*> skyboxEdges;
//	/*std::filesystem::path path = filename;
//	std::string extension = path.extension().string();
//
//	std::vector<void*> skyboxEdges;
//	int image_width = 0;
//	int image_height = 0;
//	int skybox_dim = 0;
//	int channels = 0;
//	T* image_data;
//
//	
//	image_data = static_cast<T*>(LoadData<T>(filename, &image_width, &image_height));
//	
//	skybox_dim = image_width / 4.0f;
//
//	T* edgeXPlus = new T[skybox_dim * skybox_dim * channels];
//	T* edgeXMinus = new T[skybox_dim * skybox_dim * channels];
//	T* edgeYPlus = new T[skybox_dim * skybox_dim * channels];
//	T* edgeYMinus = new T[skybox_dim * skybox_dim * channels];
//	T* edgeZPlus = new T[skybox_dim * skybox_dim * channels];
//	T* edgeZMinus = new T[skybox_dim * skybox_dim * channels];
//
//	for (int i = 0; i < skybox_dim; i++)
//		for (int j = 0; j < skybox_dim * channels; j += channels)
//		{
//			for (int k = 0; k < channels; k++)
//			{
//				edgeXPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + image_width * channels * i + j + k];
//				edgeXMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels * 2 + image_width * channels * i + j + k];
//				edgeYPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels + image_width * channels * i + j + k];
//				edgeYMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width * 2 + skybox_dim * channels + image_width * channels * i + j + k];
//				edgeZMinus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels + image_width * channels * i + j + k];
//				edgeZPlus[i * skybox_dim * channels + j + k] = image_data[skybox_dim * channels * image_width + skybox_dim * channels * 3 + image_width * channels * i + j + k];
//
//			}
//		}
//
//	skyboxEdges.push_back(edgeXMinus);
//	skyboxEdges.push_back(edgeXPlus);
//	skyboxEdges.push_back(edgeYPlus);
//	skyboxEdges.push_back(edgeYMinus);
//	skyboxEdges.push_back(edgeZMinus);
//	skyboxEdges.push_back(edgeZPlus);
//
//	*out_width = skybox_dim;
//	*out_height = skybox_dim;
//	std::string out = "out.png";*/
//
//	return skyboxEdges;
//}
//
//template <unsigned char>
//void* ImageLoader::LoadData(const char* filename, int* out_width, int* out_height)
//{
//	return (stbi_load(filename, out_width, out_height, nullptr, 4));
//
//}
//
//template <float>
//void* ImageLoader::LoadData(const char* filename, int* out_width, int* out_height)
//{
//	return  (stbi_loadf(filename, out_width, out_height, nullptr, 4));
//}
//
//template <typename T>
//void* ImageLoader::LoadData(const char* filename, int* out_width, int* out_height)
//{
//	char a = 0;
//	return &a;
//}
