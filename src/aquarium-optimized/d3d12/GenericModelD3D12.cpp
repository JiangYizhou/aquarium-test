#include "GenericModelD3D12.h"

GenericModelD3D12::GenericModelD3D12(Context *context,
                                     Aquarium *aquarium,
                                     MODELGROUP type,
                                     MODELNAME name,
                                     bool blend)
    : Model(type, name, blend), instance(0)
{
    contextD3D12 = static_cast<ContextD3D12 *>(context);

    lightFactorUniforms.shininess      = 50.0f;
    lightFactorUniforms.specularFactor = 1.0f;
}

void GenericModelD3D12::init()
{
    programD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    diffuseTexture    = static_cast<TextureD3D12 *>(textureMap["diffuse"]);
    normalTexture     = static_cast<TextureD3D12 *>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureD3D12 *>(textureMap["reflectionMap"]);
    skyboxTexture     = static_cast<TextureD3D12 *>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferD3D12 *>(bufferMap["position"]);
    normalBuffer   = static_cast<BufferD3D12 *>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferD3D12 *>(bufferMap["texCoord"]);
    tangentBuffer  = static_cast<BufferD3D12 *>(bufferMap["tangent"]);
    binormalBuffer = static_cast<BufferD3D12 *>(bufferMap["binormal"]);
    indicesBuffer  = static_cast<BufferD3D12 *>(bufferMap["indices"]);

    vertexBufferView[0] = positionBuffer->mVertexBufferView;
    vertexBufferView[1] = normalBuffer->mVertexBufferView;
    vertexBufferView[2] = texCoordBuffer->mVertexBufferView;
    vertexBufferView[3] = tangentBuffer->mVertexBufferView;
    vertexBufferView[4] = binormalBuffer->mVertexBufferView;

    // create input layout
    // Generic models use reflection, normal or diffuse shaders, of which groupLayouts are
    // different in texture binding.  MODELGLOBEBASE use diffuse shader though it contains
    // normal and reflection textures.
    if (normalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        inputElementDescs = {
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };
    }
    else
    {
        inputElementDescs = {
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };
    }

    // create constant buffer, desc.
    lightFactorBuffer = contextD3D12->createDefaultBuffer(
        &lightFactorUniforms, contextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        lightFactorUploadBuffer);
    lightFactorView.BufferLocation = lightFactorBuffer->GetGPUVirtualAddress();
    lightFactorView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    contextD3D12->buildCbvDescriptor(lightFactorView, &lightFactorGPUHandle);
    worldBuffer = contextD3D12->createUploadBuffer(
        &worldUniformPer, contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniformPer)));
    worldBufferView.BufferLocation = worldBuffer->GetGPUVirtualAddress();
    worldBufferView.SizeInBytes = contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniformPer));

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[5];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = contextD3D12->rootParameterGeneral;
    rootParameters[1] = contextD3D12->rootParameterWorld;

    if (skyboxTexture && reflectionTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        diffuseTexture->createSrvDescriptor();
        normalTexture->createSrvDescriptor();
        reflectionTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    else if (normalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        diffuseTexture->createSrvDescriptor();
        normalTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    else
    {
        diffuseTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               contextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    contextD3D12->createRootSignature(&rootSignatureDesc, m_rootSignature);

    contextD3D12->createGraphicsPipelineState(inputElementDescs, m_rootSignature,
                                              programD3D12->getVSModule(),
                                              programD3D12->getFSModule(), m_pipelineState, mBlend);
}

// Update constant buffer per frame
void GenericModelD3D12::preDraw() const
{
    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    worldBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, &worldUniformPer, sizeof(WorldUniformPer));
}

void GenericModelD3D12::draw()
{
    auto &commandList = contextD3D12->mCommandList;

    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, contextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, contextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, lightFactorGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, diffuseTexture->getTextureGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(4, worldBufferView.BufferLocation);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // diffuseShader doesn't have to input tangent buffer or binormal buffer.
    if (tangentBuffer && binormalBuffer && mName != MODELNAME::MODELGLOBEBASE)
    {
        commandList->IASetVertexBuffers(0, 5, vertexBufferView);
    } else
    {
        commandList->IASetVertexBuffers(0, 3, vertexBufferView);
    }
    commandList->IASetIndexBuffer(&indicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);

    instance = 0;
}

void GenericModelD3D12::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    worldUniformPer.WorldUniforms[instance] = *worldUniforms;

    instance++;
}
