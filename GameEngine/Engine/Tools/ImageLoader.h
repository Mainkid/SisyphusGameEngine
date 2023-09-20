#pragma once
#include <d3d.h>
#include <d3d11.h>


class ImageLoader {
public:
	static bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	static void* LoadImageFromFile(const char* filename, int* out_width, int* out_height);
};