#include "EditorGridRenderSystem.h"
#include "SimpleMath.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Rendering/RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"

SyResult EditorGridRenderSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	grid=CreateGrid(1, 16);
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorGridRender system initialization successful.");
	return SyResult();
}

SyResult EditorGridRenderSystem::Run()
{
	UINT strides[1] = { 16 };
	UINT offsets[1] = { 0 };


	hc->context->RSSetState(rc->cullBackRS.Get());
	auto ettID = ec->scene->registry.view<CameraComponent>().front();
	CB_BaseEditorBuffer data;
	data.baseData.world = Matrix::Identity;
	data.baseData.view = ec->scene->camera->view;
	data.baseData.worldView = ec->scene->camera->view;
	data.baseData.worldViewProj = ec->scene->camera->view * ec->scene->camera->projection;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT res = hc->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &data, sizeof(CB_BaseEditorBuffer));
	hc->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
	hc->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
	hc->renderTarget->SetRenderTarget(hc->depthStencilView.Get());
	hc->context->IASetInputLayout(rc->editorGridRenderer->layout.Get());
	hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	hc->context->IASetIndexBuffer(grid->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	hc->context->IASetVertexBuffers(0, 1, grid->vertexBuffer->buffer.GetAddressOf(),
		strides, offsets);
	hc->renderTarget->SetRenderTarget(hc->depthStencilView.Get());
	hc->context->VSSetShader(rc->editorGridRenderer->vertexShader.Get(), nullptr, 0);
	hc->context->PSSetShader(rc->editorGridRenderer->pixelShader.Get(), nullptr, 0);
	hc->context->DrawIndexed(grid->indexBuffer->size, 0, 0);



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

	std::vector<int> indices;
	std::vector<Vector4> vertices;
	int indexCtr = 0;
	bool state = true;

	for (int i = -width / 2; i <= width / 2; i++)
	{
		for (int j = -width / 2; j <= width / 2; j++)
		{
			if (state)
			{
				indices.push_back(indexCtr);
				indices.push_back(indexCtr + 1);
				indices.push_back(width + indexCtr + 1);
				indexCtr++;
			}
			else
			{
				indices.push_back(indexCtr);
				indices.push_back(width + indexCtr + 1);
				indices.push_back(width + indexCtr);
				
			}
			vertices.push_back(Vector4(i, 0, j, 1));
			state = !state;
		}
		indices.push_back(indexCtr);
		indices.push_back(indexCtr + 1);
		indices.push_back(width + indexCtr + 1);
		indexCtr++;

		indices.push_back(indexCtr);
		indices.push_back(width + indexCtr + 1);
		indices.push_back(width + indexCtr);
		indexCtr++;
		state = true;
		int a = 0;
	}
	/*for (int i = 0; i < (width * 2 - 2)*3; i++)
		indices.pop_back();*/

	for (int i = -width / 2; i <= width / 2; i++)
	{
		vertices.push_back(Vector4(width / 2,i , 0, 1));
	}

	return std::make_shared<Mesh>(vertices, indices);
}
