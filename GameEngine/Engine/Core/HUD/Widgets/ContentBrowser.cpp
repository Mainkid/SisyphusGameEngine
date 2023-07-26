#include "ContentBrowser.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../../DirectXSDK/stb_image.h"

ContentBrowser::ContentBrowser(Hud* _hud)
{
	this->hud = _hud;
	InitImagesSRV();
	
}

void ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");
	Widget::Render();
	
	

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = max((int)panelWidth / cellSize, 1);
	ImGui::Columns(columnCount, 0, false);

	if (curDirectory != std::filesystem::path(assetsDirectory))
	{
		ImGui::ImageButton("...", iconSRV[EContent::Folder],{ thumbnailSize,thumbnailSize });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			curDirectory = curDirectory.parent_path();
		}
		ImGui::Text("...");
		ImGui::NextColumn();
	}

	

	for (auto& directoryEntry : std::filesystem::directory_iterator(curDirectory))
	{
		const auto& path = directoryEntry.path();
		auto relativePath=std::filesystem::relative(directoryEntry.path(), assetsDirectory);
		std::string  fileNameStr = relativePath.filename().string();
		std::string fileExtension =relativePath.extension().string();

		ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
		ImVec2 uv1 = ImVec2(128.0f / 128.0f, 128.0f / 128.0f);    // UV coordinates for (32,32) in our texture
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
		ImGui::ImageButton("a",iconSRV[cb_extensionsDB[fileExtension]], ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 0), ImVec2(1, 1), bg_col, tint_col);

		
		if (ImGui::IsItemHovered()	 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				curDirectory /= path.filename();
		}
		ImGui::Text(fileNameStr.c_str());



		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

bool ContentBrowser::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
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
	EngineCore::instance()->device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	EngineCore::instance()->device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

void ContentBrowser::InitImagesSRV()
{
	int size;
	for (auto& keyVal : cb_iconsPathsDB)
	{
		iconSRV[keyVal.first] = nullptr;
		LoadTextureFromFile(keyVal.second.string().c_str(), &iconSRV[keyVal.first], &size, &size);
	}
	std::cout << std::endl;
}

ContentBrowser::~ContentBrowser()
{

}
