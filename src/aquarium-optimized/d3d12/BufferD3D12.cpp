#include "BufferD3D12.h"
#include "ContextD3D12.h"

BufferD3D12::BufferD3D12(ContextD3D12 *context,
                         int totalComponents,
                         int numComponents,
                         const std::vector<float> &buffer,
                         bool isIndex)
    : mIsIndex(isIndex), mTotoalComponents(totalComponents), mStride(0), mOffset(nullptr)
{
    mSize   = totalComponents * sizeof(float);
    mBuffer = context->createDefaultBuffer(buffer.data(), mSize, mUploadBuffer);

    // Initialize the vertex buffer view.
    mVertexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes  = numComponents * sizeof(float);
    mVertexBufferView.SizeInBytes    = mSize;
}

BufferD3D12::BufferD3D12(ContextD3D12 *context,
                         int totalComponents,
                         int numComponents,
                         const std::vector<unsigned short> &buffer,
                         bool isIndex)
    : mIsIndex(isIndex), mTotoalComponents(totalComponents), mStride(0), mOffset(nullptr)
{
    mSize   = totalComponents * sizeof(unsigned short);
    mBuffer = context->createDefaultBuffer(buffer.data(), mSize, mUploadBuffer);

    // Initialize the vertex buffer view.
    mIndexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
    mIndexBufferView.SizeInBytes    = mSize;
    mIndexBufferView.Format         = DXGI_FORMAT_R16_UINT;
}
