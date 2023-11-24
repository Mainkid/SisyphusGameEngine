#include "ParticleRenderSystem.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Features/Lighting/Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../Features/Particles/Components/ParticleComponent.h"
#include "../../../Scene/CameraHelper.h"

SyResult ParticleRenderSystem::Init()
{
    srand(static_cast <unsigned> (time(0)));
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();

    int width, height;
    ImageLoader::LoadTextureFromFile(".//Engine//Assets//Resources//Textures//noise.png", _noiseTextureSrv.GetAddressOf(), &width, &height);


    SY_LOG_CORE(SY_LOGLEVEL_INFO, "Particle Render System initialization successful.");
    return SyResult();
}

SyResult ParticleRenderSystem::Run()
{
    _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
    auto view = _ecs->view<TransformComponent, ParticleComponent>();

    auto [camera, cameraTransform] = CameraHelper::Find(_ecs);


    for (auto& entt : view)
    {
        
        ParticleComponent& pc = _ecs->get<ParticleComponent>(entt);
        TransformComponent& tc = _ecs->get<TransformComponent>(entt);

        pc.TimeFromStart += _ec->deltaTime;

        if (pc.TimeFromStart < pc.SharedParticlesDataResource->StartDelayTime.Fvalue)
            continue;

        

        CB_ComputeShader dataGroup;
        NullSortList(pc);
        
        dataGroup.GroupCount = Vector4(pc.GroupSizeX, pc.GroupSizeY, 0, 1);
        dataGroup.viewProjBuff.projection = camera.projection;
        dataGroup.viewProjBuff.view = camera.view;
        dataGroup.eyePos = Vector4(cameraTransform.localPosition.x,
            cameraTransform.localPosition.y, cameraTransform.localPosition.z, 1);

        dataGroup.world = tc.transformMatrix;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        dataGroup.SharedParticlesData.deltaTime = Vector4(_ec->deltaTime);
        dataGroup.SharedParticlesData.startColor = Vector4(pc.SharedParticlesDataResource->StartColor.V4value);

        dataGroup.SharedParticlesData.startLifeTime.Fvalue = Vector4(pc.SharedParticlesDataResource->StartLifeTime.Fvalue,1,1,1);
        dataGroup.SharedParticlesData.startLifeTime.RandomBetweenConstsF = Vector4(pc.SharedParticlesDataResource->StartLifeTime.RandomBetweenConstsF.f1, pc.SharedParticlesDataResource->StartLifeTime.RandomBetweenConstsF.f2, 1, 1);
        dataGroup.SharedParticlesData.startLifeTime.InputType = Vector4(pc.SharedParticlesDataResource->StartLifeTime.InputType);
    	dataGroup.SharedParticlesData.startPosition = Vector4(tc.localPosition);

        dataGroup.SharedParticlesData.startSize.Fvalue = Vector4(pc.SharedParticlesDataResource->StartSize.Fvalue);
        dataGroup.SharedParticlesData.startSize.RandomBetweenConstsF = Vector4(pc.SharedParticlesDataResource->StartSize.RandomBetweenConstsF.f1, pc.SharedParticlesDataResource->StartSize.RandomBetweenConstsF.f2, 1, 1);
        dataGroup.SharedParticlesData.startSize.InputType = Vector4(pc.SharedParticlesDataResource->StartSize.InputType);

        dataGroup.SharedParticlesData.startVelocity.Fvalue = Vector4(pc.SharedParticlesDataResource->StartSpeed.Fvalue);
        dataGroup.SharedParticlesData.startVelocity.RandomBetweenConstsF = Vector4(pc.SharedParticlesDataResource->StartSpeed.RandomBetweenConstsF.f1, pc.SharedParticlesDataResource->StartSpeed.RandomBetweenConstsF.f2,1,1);
        dataGroup.SharedParticlesData.startVelocity.InputType = Vector4(pc.SharedParticlesDataResource->StartSpeed.InputType);

        dataGroup.SharedParticlesData.startRotation.Fvalue = Vector4(pc.SharedParticlesDataResource->StartRotation.Fvalue);
        dataGroup.SharedParticlesData.startRotation.RandomBetweenConstsF = Vector4(pc.SharedParticlesDataResource->StartRotation.RandomBetweenConstsF.f1, pc.SharedParticlesDataResource->StartRotation.RandomBetweenConstsF.f2, 1, 1);
        dataGroup.SharedParticlesData.startRotation.InputType = Vector4(pc.SharedParticlesDataResource->StartRotation.InputType);

        dataGroup.SharedParticlesData.shapeRadiusAndAngle = Vector4(pc.SharedParticlesDataResource->ParticleEmitShape,
            pc.SharedParticlesDataResource->Radius, pc.SharedParticlesDataResource->Angle, 1);

        dataGroup.SharedParticlesData.SizeOverLifetime.P0 = Vector2(pc.SharedParticlesDataResource->SizeOverLifetime.P0);
        dataGroup.SharedParticlesData.SizeOverLifetime.P1 = Vector2(pc.SharedParticlesDataResource->SizeOverLifetime.P1);
        dataGroup.SharedParticlesData.SizeOverLifetime.P2 = Vector2(pc.SharedParticlesDataResource->SizeOverLifetime.P2);
        dataGroup.SharedParticlesData.SizeOverLifetime.P3 = Vector2(pc.SharedParticlesDataResource->SizeOverLifetime.P3);
        dataGroup.SharedParticlesData.SizeOverLifetime.IsUsing =pc.SharedParticlesDataResource->SizeOverLifetime.IsUsing;

        dataGroup.SharedParticlesData.SpeedOverLifetime.P0 = Vector2(pc.SharedParticlesDataResource->SpeedOverLifetime.P0);
        dataGroup.SharedParticlesData.SpeedOverLifetime.P1 = Vector2(pc.SharedParticlesDataResource->SpeedOverLifetime.P1);
        dataGroup.SharedParticlesData.SpeedOverLifetime.P2 = Vector2(pc.SharedParticlesDataResource->SpeedOverLifetime.P2);
        dataGroup.SharedParticlesData.SpeedOverLifetime.P3 = Vector2(pc.SharedParticlesDataResource->SpeedOverLifetime.P3);
        dataGroup.SharedParticlesData.SpeedOverLifetime.IsUsing = pc.SharedParticlesDataResource->SpeedOverLifetime.IsUsing;

        dataGroup.SharedParticlesData.RotationOverLifetime.Fvalue = Vector4(pc.SharedParticlesDataResource->RotationOverLifetime.Fvalue);
        dataGroup.SharedParticlesData.RotationOverLifetime.InputType = Vector4(pc.SharedParticlesDataResource->RotationOverLifetime.InputType);
        dataGroup.SharedParticlesData.RotationOverLifetime.RandomBetweenConstsF = Vector4(pc.SharedParticlesDataResource->RotationOverLifetime.RandomBetweenConstsF.f1, pc.SharedParticlesDataResource->RotationOverLifetime.RandomBetweenConstsF.f2, 1, 1);

        HRESULT res = _hc->context->Map(pc.GroupCountConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataGroup, sizeof(CB_ComputeShader));
        _hc->context->Unmap(pc.GroupCountConstBuffer->buffer.Get(), 0);
        _hc->context->OMSetBlendState(_rc->ParticlesBlendState.Get(), nullptr, 0xffffffff);
        _hc->context->CSSetConstantBuffers(0, 1, pc.GroupCountConstBuffer->buffer.GetAddressOf());


        _hc->context->CSSetShader(_rc->ParticleEmitter->computeShader.Get(), nullptr, 0);
        const unsigned int end[1] = { pc.SharedParticlesDataResource->MaxParticles - 1 };
        const unsigned int start[1] = { 0 };
        _hc->context->CSSetUnorderedAccessViews(1, 1, pc.PoolBufferUav.GetAddressOf(), nullptr);
        if (!pc.IsInitialized)
        {
            _hc->context->CSSetUnorderedAccessViews(2, 1, pc.DeadListUav.GetAddressOf(), end);
            pc.IsInitialized = true;
        }
        else
        {
            _hc->context->CSSetUnorderedAccessViews(2, 1, pc.DeadListUav.GetAddressOf(), nullptr);
        }
        _hc->context->CSSetShaderResources(0, 1, pc.CounterSrv.GetAddressOf());
        _hc->context->CSSetShaderResources(1, 1, _noiseTextureSrv.GetAddressOf());
        _hc->context->CSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());

        if (pc.ParticlesToEmit >= 1.0f )
        {
            _hc->context->Dispatch(int(pc.ParticlesToEmit), 1, 1);
            pc.ParticlesToEmit = pc.ParticlesToEmit-int(pc.ParticlesToEmit);
        }

        for (auto& burst : pc.SharedParticlesDataResource->ParticleBursts)
        {
            
	        if (burst.TimeSinceLastBurst>burst.Time)
	        {
                
                float randomVal = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                if (randomVal <= burst.Probability)
                {
                    _hc->context->Dispatch(int(burst.Count.Fvalue), 1, 1);
                }
                burst.TimeSinceLastBurst = 0.0f;
	        }

            burst.TimeSinceLastBurst += _ec->deltaTime;
        }

        _hc->context->CSSetShaderResources(0, 2, _rc->RhData.NullSrv);
       

        pc.ParticlesToEmit += pc.SharedParticlesDataResource->RateOverTime.Fvalue * _ec->deltaTime;

        ID3D11UnorderedAccessView* nullViews[] = { nullptr,nullptr,nullptr };
        _hc->context->CSSetUnorderedAccessViews(1, 3, nullViews, nullptr);

        _hc->context->CSSetShader(_rc->ParticleUpdate->computeShader.Get(), nullptr, 0);
        _hc->context->CSSetUnorderedAccessViews(1, 1, pc.PoolBufferUav.GetAddressOf(), nullptr);
        _hc->context->CSSetUnorderedAccessViews(2, 1, pc.DeadListUav.GetAddressOf(), nullptr);
        _hc->context->CSSetUnorderedAccessViews(3, 1, pc.SortBufferUav.GetAddressOf(), start);
        _hc->context->CSSetConstantBuffers(0, 1, pc.GroupCountConstBuffer->buffer.GetAddressOf());


        
        _hc->context->Dispatch(pc.GroupSizeX, pc.GroupSizeY, 1);

        
        _hc->context->CSSetUnorderedAccessViews(1, 3, nullViews, 0);
        _hc->context->CSSetShader(nullptr, 0, 0);

        //=============== //
        // Sorting        //
        //=============== //

        SortGpu(pc);
        _hc->context->CSSetUnorderedAccessViews(0, 1, nullViews, 0);
        _hc->context->CSSetShader(nullptr, 0, 0);

        //=============== //
        // Multiply by 6  //
        //=============== //
        _hc->context->CopyStructureCount(pc.CounterBuffer->buffer.Get(), 0, pc.SortBufferUav.Get());
        _hc->context->CSSetShader(_rc->MultiplyBy6Shader->computeShader.Get(), nullptr, 0);
        _hc->context->CSSetUnorderedAccessViews(1, 1, pc.CounterUav.GetAddressOf(), nullptr);
        _hc->context->Dispatch(1, 1, 1);
        _hc->context->CSSetUnorderedAccessViews(1, 1, nullViews, 0);
        _hc->context->CSSetShader(nullptr, 0, 0);

        //=============== //
        // Visualisation  //
        //=============== //


        CB_ParticleVisualisation dataParticle;
        dataParticle.viewProj.projection = camera.projection;
        dataParticle.viewProj.view =camera.view;
        dataParticle.eyePos = Vector4(cameraTransform.localPosition.x,
            cameraTransform.localPosition.y, cameraTransform.localPosition.z, 1);

        /*dataParticle.lightCount = _hc->lights.size();*/
        Vector3 vec = Vector3(camera.forward.x,camera.forward.y,camera.forward.z);
        dataParticle.viewDirection = vec;
        dataParticle.cameraRot = camera.view.Invert();       //???
        dataParticle.litAmbientParams=Vector4(pc.SharedParticlesDataResource->IsLit, pc.SharedParticlesDataResource->AmbientAmount, 0, 1);


        auto view = _ecs->view<LightComponent,TransformComponent>();
        int ctr = 0;
        ID3D11ShaderResourceView* shadowSrv;
        for (auto ent : view)
        {
            auto [lightComponent,transform] = view.get<LightComponent,TransformComponent>(ent);
            if (lightComponent.LightType == ELightType::Directional)
                shadowSrv = lightComponent.DirShadowBluredSrv.Get();

            dataParticle.lightData[ctr].Pos = Vector4(transform.localPosition.x, transform.localPosition.y, transform.localPosition.z,1);
            dataParticle.lightData[ctr].Color = lightComponent.Color;
            dataParticle.lightData[ctr].Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(transform.localRotation));
            dataParticle.lightData[ctr].additiveParams = lightComponent.ParamsRadiusAndAttenuation;
            dataParticle.lightData[ctr].additiveParams.w = (float)lightComponent.LightType;

            if (lightComponent.LightType == ELightType::Directional)
            {
                shadowSrv = lightComponent.DirShadowBluredSrv.Get();
                for (int j = 0; j < 4; j++)
                {
                    dataParticle.distances[j] = lightComponent.Distances[j];
                    dataParticle.viewProjs[j] = lightComponent.ViewMatrices[j] * lightComponent.OrthoMatrices[j];
                }
            }
            ctr++;
        }
        dataParticle.lightCount = ctr;

        D3D11_MAPPED_SUBRESOURCE mappedResource2;
        res = _hc->context->Map(pc.ConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
        CopyMemory(mappedResource2.pData, &dataParticle, sizeof(CB_ParticleVisualisation));
        _hc->context->Unmap(pc.ConstBuffer->buffer.Get(), 0);


        _hc->context->VSSetConstantBuffers(0, 1, pc.ConstBuffer->buffer.GetAddressOf());
        _hc->context->PSSetConstantBuffers(0, 1, pc.ConstBuffer->buffer.GetAddressOf());

        _hc->context->VSSetShader(_rc->SimpleParticle->vertexShader.Get(), nullptr, 0);
        _hc->context->PSSetShader(_rc->SimpleParticle->pixelShader.Get(), nullptr, 0);
        _hc->context->VSSetShaderResources(0, 1, &shadowSrv);
        _hc->context->VSSetShaderResources(1, 1, pc.PoolBufferSrv.GetAddressOf());
        _hc->context->VSSetShaderResources(2, 1, pc.SortBufferSrv.GetAddressOf());
        //_hc->context->VSSetShaderResources(3, 1, pc.TextureSrv.GetAddressOf());
        _hc->context->PSSetShaderResources(0, 1, &shadowSrv);
        _hc->context->PSSetShaderResources(1, 1, pc.PoolBufferSrv.GetAddressOf());
        _hc->context->PSSetShaderResources(2, 1, pc.SortBufferSrv.GetAddressOf());
        _hc->context->PSSetShaderResources(3, 1, pc.ParticleTexture->textureSRV.GetAddressOf());

        _hc->context->PSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
        _hc->context->PSSetSamplers(1, 1, _rc->SamplerState.GetAddressOf());
        _hc->context->VSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
        _hc->context->VSSetSamplers(1, 1, _rc->SamplerState.GetAddressOf());

        _hc->context->CopySubresourceRegion(pc.IndirectDrawBuffer->buffer.Get(), 0, 0, NULL, NULL, pc.CounterBuffer->buffer.Get(), 0, NULL);

        ID3D11UnorderedAccessView* UAVs[2] = { pc.PoolBufferUav.Get(),pc.SortBufferUav.Get() };
        
        _hc->context->OMSetRenderTargets(1, _rc->GBuffer->HdrBufferRtv.GetAddressOf(), _hc->depthStencilView.Get());
       
        _hc->context->IASetIndexBuffer(pc.IndexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        _hc->context->IASetInputLayout(_rc->SimpleParticle->layout.Get());
        _hc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //_hc->context->DrawInstancedIndirect(pc.indirectDrawBuffer->buffer.Get(),0);
        _hc->context->DrawIndexedInstancedIndirect(pc.IndirectDrawBuffer->buffer.Get(), 0);

        ID3D11ShaderResourceView* nullSRV[4] = { nullptr,nullptr,nullptr,nullptr };
        _hc->context->PSSetShaderResources(0, 4, nullSRV);
        _hc->context->VSSetShaderResources(0, 4, nullSRV);
        //=============== //
        // Shadow rec.    //
        //=============== //

    }


    return SyResult();
}

SyResult ParticleRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "Particle Render System destruction successful.");
    return SyResult();

}

void ParticleRenderSystem::SortGpu(ParticleComponent& pc)
{
    NUM_ELEMENTS = pow(2,floor(log2(pc.SharedParticlesDataResource->MaxParticles)));
    auto tmp = ceil(log2(NUM_ELEMENTS / BITONIC_BLOCK_SIZE));
    MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;
    for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level = level * 2)
    {
        SetConstants(pc,level, level, MATRIX_HEIGHT, MATRIX_WIDTH);

        // Sort the row data
        _hc->context->CSSetUnorderedAccessViews(0, 1, pc.SortBufferUav.GetAddressOf(), nullptr);
        _hc->context->CSSetShader(_rc->BitonicSort->computeShader.Get(), nullptr, 0);
        _hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
    }

    // Then sort the rows and columns for the levels > than the block size
    // Transpose. Sort the Columns. Transpose. Sort the Rows.
    for (UINT level = (BITONIC_BLOCK_SIZE * 2); level <= NUM_ELEMENTS; level = level * 2)
    {
        SetConstants(pc,(level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT);

        // Transpose the data from buffer 1 into buffer 2
        ID3D11ShaderResourceView* pViewnullptr = nullptr;
        _hc->context->CSSetShaderResources(0, 1, &pViewnullptr);
        _hc->context->CSSetUnorderedAccessViews(0, 1, pc.TmpGpuBufferUav.GetAddressOf(), nullptr);
        _hc->context->CSSetShaderResources(0, 1, pc.SortBufferSrv.GetAddressOf());
        _hc->context->CSSetShader(_rc->MatrixTranspose->computeShader.Get(), nullptr, 0);
        _hc->context->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

        //std::cout << MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE << " " << MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE << std::endl;
        // Sort the transposed column data
        _hc->context->CSSetShader(_rc->BitonicSort->computeShader.Get(), nullptr, 0);
        _hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

        //std::cout << NUM_ELEMENTS / BITONIC_BLOCK_SIZE << std::endl;

        SetConstants(pc,BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT,MATRIX_WIDTH);

        // Transpose the data from buffer 2 back into buffer 1
        _hc->context->CSSetShaderResources(0, 1, &pViewnullptr);
        _hc->context->CSSetUnorderedAccessViews(0, 1, pc.SortBufferUav.GetAddressOf(), nullptr);
        _hc->context->CSSetShaderResources(0, 1, pc.TmpGpuBufferSrv.GetAddressOf());
        _hc->context->CSSetShader(_rc->MatrixTranspose->computeShader.Get(), nullptr, 0);
        _hc->context->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);
        //std::cout << MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE<<" "<< MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE << std::endl;

        // Sort the row data
        _hc->context->CSSetShader(_rc->BitonicSort->computeShader.Get(), nullptr, 0);
        _hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

        //std::cout << NUM_ELEMENTS / BITONIC_BLOCK_SIZE << std::endl;
    }
}

void ParticleRenderSystem::NullSortList(ParticleComponent& pc)
{
    std::generate(pc.data.begin(), pc.data.end(), [&] { return SortListParticle(0, 0); });
   _hc->context->UpdateSubresource(pc.SortListBuffer->buffer.Get(), 0, nullptr, &pc.data[0], 0, 0);
}

void ParticleRenderSystem::SetConstants(ParticleComponent& pc,UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight)
{

    CB cb = { iLevel, iLevelMask, iWidth, iHeight };
    _hc->context->UpdateSubresource(pc.SortGpuConstBuffer->buffer.Get(), 0, nullptr, &cb, 0, 0);
    _hc->context->CSSetConstantBuffers(0, 1, pc.SortGpuConstBuffer->buffer.GetAddressOf());
}