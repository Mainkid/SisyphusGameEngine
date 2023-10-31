#include "EditorGridRenderSystem.h"
#include "SimpleMath.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Rendering/RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Scene/CameraHelper.h"

SyResult EditorGridRenderSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorGridRender system initialization successful.");
	grid1M = CreateGrid(1, minGridSize);
	grid10M = CreateGrid(10, maxGridSize);
	return SyResult();
}

SyResult EditorGridRenderSystem::Run()
{
	
	CB_GridEditorBuffer dataOpaque;
	UINT strides[1] = { 16 };
	UINT offsets[1] = { 0 };

	hc->context->OMSetBlendState(rc->GridBlendState.Get(), nullptr, 0xffffffff);
	auto [camera, cameraTransform] = CameraHelper::Find(_ecs);

	dataOpaque.baseData.worldViewProj =
	camera.view * camera.projection;
	hc->context->OMSetDepthStencilState(rc->OffStencilState.Get(),0);
	

	dataOpaque.eyePos = Vector4(cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z,10);
	Vector3 vec3 = Vector3((int)dataOpaque.eyePos.x- minGridSize / 2, 0, (int)dataOpaque.eyePos.z-minGridSize/2);
	dataOpaque.baseData.worldView = Matrix::CreateTranslation(vec3);
	dataOpaque.baseData.worldViewProj = Matrix::CreateTranslation(vec3) *
		camera.view*camera.projection;
	
	

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		
		HRESULT res = hc->context->Map(rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_GridEditorBuffer));
		hc->context->Unmap(rc->ShadowConstBuffer->buffer.Get(), 0);
		hc->context->VSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());
		hc->context->PSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());
		hc->renderTarget->SetRenderTarget( hc->depthStencilView.Get());
		hc->context->IASetInputLayout(rc->EditorGridRenderer->layout.Get());
		hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		hc->context->IASetIndexBuffer(grid1M->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		hc->context->IASetVertexBuffers(0, 1, grid1M->vertexBuffer->buffer.GetAddressOf(),
			strides, offsets);
	
		
		hc->context->VSSetShader(rc->EditorGridRenderer->vertexShader.Get(), nullptr, 0);
		hc->context->PSSetShader(rc->EditorGridRenderer->pixelShader.Get(), nullptr, 0);
		hc->context->DrawIndexed(grid1M->indexBuffer->size, 0, 0);

		dataOpaque.eyePos.w = 10000000;
		vec3 = Vector3(((int)(dataOpaque.eyePos.x/10))*10 - maxGridSize*10 / 2, 0, ((int)(dataOpaque.eyePos.z/10))*10 - maxGridSize*10 / 2);
		dataOpaque.baseData.worldView = Matrix::CreateTranslation(vec3);
		dataOpaque.baseData.worldViewProj = Matrix::CreateTranslation(vec3) *
			camera.view * camera.projection;

		res = hc->context->Map(rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_GridEditorBuffer));
		hc->context->Unmap(rc->ShadowConstBuffer->buffer.Get(), 0);
		hc->context->VSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());
		hc->context->PSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());

		hc->context->IASetIndexBuffer(grid10M->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		hc->context->IASetVertexBuffers(0, 1, grid10M->vertexBuffer->buffer.GetAddressOf(),
			strides, offsets);
		hc->context->DrawIndexed(grid10M->indexBuffer->size, 0, 0);

		return SyResult();
}


SyResult EditorGridRenderSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorGridRender system destruction successful.");
	return SyResult();
}

std::shared_ptr<Mesh> EditorGridRenderSystem::CreateGrid(float metersPerCell,int width)
{
	using namespace DirectX::SimpleMath;

	// Initialize the index into the vertex and index arrays.
	int index = 0;
	float positionX;
	float positionZ;
	std::vector<int> indices((width - 1) * (width - 1) * 8);
	std::vector<Vector4> vertices((width - 1) * (width - 1) * 8);
	// Load the vertex array and index array with data.
	for (int j = 0; j < (width - 1); j++)
	{
		for (int i = 0; i < (width - 1); i++)
		{
			// Line 1 - Upper left.
			positionX = (float)i*metersPerCell;
			positionZ = (float)(j + 1)* metersPerCell;

			vertices[index]= Vector4(positionX, 0.0f, positionZ,1);
			indices[index] = index;
			index++;

			// Line 1 - Upper right.
			positionX = (float)(i + 1) * metersPerCell;
			positionZ = (float)(j + 1) * metersPerCell;

			vertices[index] = Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 2 - Upper right
			positionX = (float)(i + 1) * metersPerCell;
			positionZ = (float)(j + 1) * metersPerCell;

			vertices[index] = Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 2 - Bottom right.
			positionX = (float)(i + 1) * metersPerCell;
			positionZ = (float)j * metersPerCell;

			vertices[index]= Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 3 - Bottom right.
			positionX = (float)(i + 1) * metersPerCell;
			positionZ = (float)j * metersPerCell;

			vertices[index]= Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 3 - Bottom left.
			positionX = (float)i * metersPerCell;
			positionZ = (float)j * metersPerCell;

			vertices[index] = Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 4 - Bottom left.
			positionX = (float)i * metersPerCell;
			positionZ = (float)j * metersPerCell;

			vertices[index] = Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;

			// Line 4 - Upper left.
			positionX = (float)i * metersPerCell;
			positionZ = (float)(j + 1) * metersPerCell;

			vertices[index]= Vector4(positionX, 0.0f, positionZ, 1);
			indices[index] = index;
			index++;
		}
	}

	return std::make_shared<Mesh>(vertices, indices);
}
