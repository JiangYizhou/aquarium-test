// dear imgui: Renderer for Dawn
// This needs to be used along with a Platform Binding (e.g. GLFW)

#include "imgui_impl_dawn.h"
#include "imgui.h"

#include "ProgramDawn.h"
#include "utils/ComboRenderPipelineDescriptor.h"

// Dawn data
dawn::Device mDevice(nullptr);
dawn::RenderPipeline mPipeline(nullptr);
dawn::BindGroup mBindGroup(nullptr);
dawn::TextureFormat mFormat(dawn::TextureFormat::RGBA8Unorm);
dawn::ShaderModule vsModule(nullptr);
dawn::ShaderModule fsModule(nullptr);

dawn::Buffer mIndexBuffer(nullptr);
dawn::Buffer mVertexBuffer(nullptr);
dawn::Buffer mConstantBuffer(nullptr);
dawn::Buffer mStagingBuffer(nullptr);
dawn::Texture mTexture(nullptr);
dawn::TextureView mTextureView;
dawn::Sampler mSampler(nullptr);

ProgramDawn *programDawn(nullptr);
ContextDawn *contextDawn(nullptr);

int mIndexBufferSize  = 0;
int mVertexBufferSize = 0;
ImDrawVert vertexData[3000];
ImDrawIdx indexData[3000];

struct VERTEX_CONSTANT_BUFFER
{
    float mvp[4][4];
};

static void ImGui_ImplDawn_SetupRenderState(ImDrawData *draw_data,
                                            const dawn::RenderPassEncoder &pass)
{
    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to
    // draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    VERTEX_CONSTANT_BUFFER vertex_constant_buffer;
    {
        float L         = draw_data->DisplayPos.x;
        float R         = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T         = draw_data->DisplayPos.y;
        float B         = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        float mvp[4][4] = {
            {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
            {0.0f, 0.0f, 0.5f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f},
        };
        memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
    }
    mConstantBuffer.SetSubData(0, sizeof(VERTEX_CONSTANT_BUFFER), &vertex_constant_buffer.mvp);

    // TODO(yizhou): setting viewport isn't supported in dawn yet.
    // Setup viewport
    // pass.SetViewport(0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y, 0.0f, 1.0f);

    uint64_t vertexBufferOffsets[1] = {0};

    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mBindGroup, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &mVertexBuffer, vertexBufferOffsets);
    pass.SetIndexBuffer(mIndexBuffer, 0);
}

// Render function
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call
// this directly from your main loop)
void ImGui_ImplDawn_RenderDrawData(ImDrawData *draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow vertex/index buffers if needed
    if (mVertexBuffer.Get() == nullptr || mVertexBufferSize < draw_data->TotalVtxCount)
    {
        mVertexBufferSize = draw_data->TotalVtxCount + 5000;
        mVertexBufferSize = mVertexBufferSize % 4 == 0
                                ? mVertexBufferSize
                                : mVertexBufferSize + 4 - mVertexBufferSize % 4;

        dawn::BufferDescriptor descriptor;
        descriptor.size  = mVertexBufferSize * sizeof(ImDrawVert);
        descriptor.usage = dawn::BufferUsageBit::Vertex | dawn::BufferUsageBit::CopyDst;

        mVertexBuffer = mDevice.CreateBuffer(&descriptor);
    }

    if (mIndexBuffer.Get() == nullptr || mIndexBufferSize < draw_data->TotalIdxCount)
    {
        mIndexBufferSize = draw_data->TotalIdxCount + 10000;
        mIndexBufferSize = mIndexBufferSize % 4 == 0 ? mIndexBufferSize
                                                     : mIndexBufferSize + 4 - mIndexBufferSize % 4;

        dawn::BufferDescriptor descriptor;
        descriptor.size  = mIndexBufferSize * sizeof(ImDrawIdx);
        descriptor.usage = dawn::BufferUsageBit::Index | dawn::BufferUsageBit::CopyDst;

        mIndexBuffer = mDevice.CreateBuffer(&descriptor);
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    uint32_t vtx_dst = 0;
    uint32_t idx_dst = 0;
    ImDrawVert *pVertex = vertexData;
    ImDrawIdx *pIndex   = indexData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        memcpy(pVertex, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(pIndex, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        pVertex += cmd_list->VtxBuffer.Size;
        pIndex += cmd_list->IdxBuffer.Size;
        vtx_dst += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        idx_dst += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
    }
    vtx_dst = vtx_dst % 4 == 0 ? vtx_dst : vtx_dst + 4 - vtx_dst % 4;
    idx_dst = idx_dst % 4 == 0 ? idx_dst : idx_dst + 4 - idx_dst % 4;

    contextDawn->setBufferData(mVertexBuffer, 0, vtx_dst, vertexData);
    contextDawn->setBufferData(mIndexBuffer, 0, idx_dst, indexData);

    const dawn::RenderPassEncoder& pass = contextDawn->getRenderPass();

    // Setup desired Dawn state
    ImGui_ImplDawn_SetupRenderState(draw_data, pass);

    // Render pass
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off       = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to
                // request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDawn_SetupRenderState(draw_data, pass);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Apply Scissor, Bind texture, Draw
                pass.SetScissorRect(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y,
                                    pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                pass.DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset,
                                 pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

static void ImGui_ImplDawn_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO &io = ImGui::GetIO();
    unsigned char *pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        dawn::TextureDescriptor descriptor;
        descriptor.dimension       = dawn::TextureDimension::e2D;
        descriptor.size.width      = width;
        descriptor.size.height     = height;
        descriptor.size.depth      = 1;
        descriptor.arrayLayerCount = 1;
        descriptor.sampleCount     = 1;
        descriptor.format          = mFormat;
        descriptor.mipLevelCount   = 1;
        descriptor.usage = dawn::TextureUsageBit::CopyDst | dawn::TextureUsageBit::Sampled;
        mTexture         = contextDawn->createTexture(descriptor);

        mStagingBuffer = contextDawn->createBufferFromData(
            pixels, width * height * 4, dawn::BufferUsageBit::CopySrc);
        dawn::BufferCopyView bufferCopyView =
            contextDawn->createBufferCopyView(mStagingBuffer, 0, width * 4, height);
        dawn::TextureCopyView textureCopyView =
            contextDawn->createTextureCopyView(mTexture, 0, 0, {0, 0, 0});
        dawn::Extent3D copySize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        dawn::CommandBuffer copyCommand =
            contextDawn->copyBufferToTexture(bufferCopyView, textureCopyView, copySize);
        contextDawn->queue.Submit(1, &copyCommand);

        // Create texture view
        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain     = nullptr;
        viewDescriptor.dimension       = dawn::TextureViewDimension::e2D;
        viewDescriptor.format          = mFormat;
        viewDescriptor.baseMipLevel    = 0;
        viewDescriptor.mipLevelCount   = 1;
        viewDescriptor.baseArrayLayer  = 0;
        viewDescriptor.arrayLayerCount = 1;

        mTextureView = mTexture.CreateView(&viewDescriptor);

        dawn::SamplerDescriptor samplerDesc;
        samplerDesc.addressModeU    = dawn::AddressMode::Repeat;
        samplerDesc.addressModeV    = dawn::AddressMode::Repeat;
        samplerDesc.addressModeW    = dawn::AddressMode::Repeat;
        samplerDesc.minFilter       = dawn::FilterMode::Linear;
        samplerDesc.magFilter       = dawn::FilterMode::Linear;
        samplerDesc.lodMinClamp     = 0.0f;
        samplerDesc.lodMaxClamp     = 0.0f;
        samplerDesc.compare = dawn::CompareFunction::Always;
        samplerDesc.mipmapFilter    = dawn::FilterMode::Linear;

        mSampler = contextDawn->createSampler(samplerDesc);
    }

    io.Fonts->TexID = (ImTextureID)mTextureView.Get();
}

bool ImGui_ImplDawn_CreateDeviceObjects()
{
    if (!mDevice)
        return false;

    utils::ComboVertexInputDescriptor vertexInputDescriptor;
    vertexInputDescriptor.cBuffers[0].attributeCount    = 3;
    vertexInputDescriptor.cBuffers[0].stride            = sizeof(ImDrawVert);
    vertexInputDescriptor.cAttributes[0].format         = dawn::VertexFormat::Float2;
    vertexInputDescriptor.cAttributes[0].shaderLocation = 0;
    vertexInputDescriptor.cAttributes[0].offset         = 0;
    vertexInputDescriptor.cAttributes[1].format         = dawn::VertexFormat::Float2;
    vertexInputDescriptor.cAttributes[1].shaderLocation = 1;
    vertexInputDescriptor.cAttributes[1].offset         = IM_OFFSETOF(ImDrawVert, uv);
    vertexInputDescriptor.cAttributes[2].format         = dawn::VertexFormat::UChar4Norm;
    vertexInputDescriptor.cAttributes[2].shaderLocation = 2;
    vertexInputDescriptor.cAttributes[2].offset         = IM_OFFSETOF(ImDrawVert, col);

    vertexInputDescriptor.cBuffers[0].attributes        = &vertexInputDescriptor.cAttributes[0];
    vertexInputDescriptor.bufferCount                   = 1;
    vertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    // Create bind group layout
    dawn::BindGroupLayout layout = contextDawn->MakeBindGroupLayout(
        {{0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
         {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
         {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture}});

    dawn::PipelineLayout pipelineLayout = contextDawn->MakeBasicPipelineLayout({layout});

    const ResourceHelper *resourceHelper = contextDawn->getResourceHelper();
    const std::string &programPath       = resourceHelper->getProgramPath();
    programDawn = new ProgramDawn(contextDawn, programPath + "imguiVertexShader",
                                  programPath + "imguiFragmentShader");
    programDawn->loadProgram();

    const dawn::ShaderModule &vsModule = programDawn->getVSModule();
    const dawn::ShaderModule &fsModule = programDawn->getFSModule();

    dawn::PipelineStageDescriptor cVertexStage;
    cVertexStage.entryPoint = "main";
    cVertexStage.module     = vsModule;

    dawn::PipelineStageDescriptor cFragmentStage;
    cFragmentStage.entryPoint = "main";
    cFragmentStage.module     = fsModule;

    dawn::BlendDescriptor blendDescriptor;
    blendDescriptor.operation = dawn::BlendOperation::Add;
    blendDescriptor.srcFactor = dawn::BlendFactor::SrcAlpha;
    blendDescriptor.dstFactor = dawn::BlendFactor::OneMinusSrcAlpha;
    dawn::BlendDescriptor alphaDescriptor;
    alphaDescriptor.operation = dawn::BlendOperation::Add;
    alphaDescriptor.srcFactor = dawn::BlendFactor::OneMinusSrcAlpha;
    alphaDescriptor.dstFactor = dawn::BlendFactor::Zero;

    dawn::ColorStateDescriptor ColorStateDescriptor;
    ColorStateDescriptor.colorBlend = blendDescriptor;
    ColorStateDescriptor.alphaBlend = alphaDescriptor;
    ColorStateDescriptor.writeMask  = dawn::ColorWriteMask::All;

    dawn::RasterizationStateDescriptor rasterizationState;
    rasterizationState.nextInChain         = nullptr;
    rasterizationState.frontFace           = dawn::FrontFace::CCW;
    rasterizationState.cullMode            = dawn::CullMode::None;
    rasterizationState.depthBias           = 0;
    rasterizationState.depthBiasSlopeScale = 0.0;
    rasterizationState.depthBiasClamp      = 0.0;

    // create graphics pipeline
    utils::ComboRenderPipelineDescriptor pipelineDescriptor(mDevice);
    pipelineDescriptor.layout                    = pipelineLayout;
    pipelineDescriptor.cVertexStage.module       = vsModule;
    pipelineDescriptor.cFragmentStage.module     = fsModule;
    pipelineDescriptor.vertexInput               = &vertexInputDescriptor;
    pipelineDescriptor.depthStencilState         = &pipelineDescriptor.cDepthStencilState;
    pipelineDescriptor.cDepthStencilState.format = dawn::TextureFormat::Depth24PlusStencil8;
    pipelineDescriptor.cColorStates[0]           = &ColorStateDescriptor;
    pipelineDescriptor.cColorStates[0]->format   = mFormat;
    pipelineDescriptor.cDepthStencilState.depthWriteEnabled = false;
    pipelineDescriptor.cDepthStencilState.depthCompare      = dawn::CompareFunction::Always;
    pipelineDescriptor.primitiveTopology                    = dawn::PrimitiveTopology::TriangleList;
    pipelineDescriptor.sampleCount                          = 1;
    pipelineDescriptor.rasterizationState                   = &rasterizationState;

    mPipeline = mDevice.CreateRenderPipeline(&pipelineDescriptor);

    ImGui_ImplDawn_CreateFontsTexture();

    // Create uniform buffer
    dawn::BufferDescriptor descriptor;
    descriptor.size  = sizeof(VERTEX_CONSTANT_BUFFER);
    descriptor.usage = dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform |
                       dawn::BufferUsageBit::Uniform;

    mConstantBuffer = mDevice.CreateBuffer(&descriptor);

    mBindGroup =
        contextDawn->makeBindGroup(layout, {{0, mConstantBuffer, 0, sizeof(VERTEX_CONSTANT_BUFFER)},
                                            {1, mSampler},
                                            {2, mTextureView}});

    return true;
}

bool ImGui_ImplDawn_Init(ContextDawn *context, dawn::TextureFormat rtv_format)
{
    // Setup back-end capabilities flags
    ImGuiIO &io            = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_Dawn";
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field,
                                                 // allowing for large meshes.

    mDevice     = context->getDevice();
    mFormat     = rtv_format;
    contextDawn = context;

    mIndexBuffer      = NULL;
    mVertexBuffer     = NULL;
    mIndexBufferSize  = 10000;
    mVertexBufferSize = 5000;

    return true;
}

void ImGui_ImplDawn_Shutdown()
{
    delete programDawn;

    mPipeline  = nullptr;
    mBindGroup = nullptr;
    vsModule   = nullptr;
    fsModule   = nullptr;

    mIndexBuffer  = nullptr;
    mVertexBuffer = nullptr;
    mStagingBuffer   = nullptr;
    mTexture      = nullptr;
    mSampler      = nullptr;
    mConstantBuffer = nullptr;
}

void ImGui_ImplDawn_NewFrame()
{
    if (!mPipeline.Get())
        ImGui_ImplDawn_CreateDeviceObjects();
}
