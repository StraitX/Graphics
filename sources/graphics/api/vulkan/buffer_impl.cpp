#include "core/os/memory.hpp"
#include "graphics/api/vulkan/buffer_impl.hpp"
#include "graphics/api/vulkan/memory_allocator.hpp"
#include "graphics/api/vulkan/debug.hpp"
#include "graphics/api/vulkan/immediate.hpp"

namespace Vk{

BufferImpl::BufferImpl(size_t size, BufferMemoryType mem_type, BufferUsage usage):
    m_Size(size),
    m_AbstractMemoryType(mem_type),
    m_BackingMemoryType(GPUImpl::Get().BackingMemoryType(MemoryType::Type(mem_type))),
    m_Usage(usage)
{
    VkBufferCreateInfo info;
    info.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext                  = nullptr;
    info.flags                  = 0;
    info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount  = 0;
    info.pQueueFamilyIndices    = nullptr;
    info.size                   = size;
    info.usage                  = usage;

    SX_VK_ASSERT(vkCreateBuffer(GPUImpl::Get(), &info, nullptr, &m_Handle), "Vk: GPUBufferImpl: Can't create buffer");

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(GPUImpl::Get(), m_Handle, &req);

    m_Memory = MemoryAllocator::Alloc(req.size, MemoryType::Type(mem_type));

    SX_VK_ASSERT(vkBindBufferMemory(GPUImpl::Get(), m_Handle, m_Memory, 0), "GPUBuffer: can't bind buffer's memory");
}

BufferImpl::~BufferImpl(){
    if(IsMapped())
        Unmap();

    MemoryAllocator::Free(m_Memory);

    vkDestroyBuffer(GPUImpl::Get(), m_Handle, nullptr);
}

void BufferImpl::Copy(Buffer *src, size_t size, size_t src_offset, size_t dst_offset){
    if(src->IsMappable() && this->IsMappable()){
        u8* src_ptr =    src->Map<u8>() + src_offset;
        u8* dst_ptr = Buffer::Map<u8>() + dst_offset;

        Memory::Copy(src_ptr, dst_ptr, size);
    }else{
        Immediate::Copy(src, this, size, src_offset, dst_offset);
    }
}

void BufferImpl::Copy(const void *data, size_t size, size_t offset){
    if(IsMappable()){
        u8 *dst_ptr = Buffer::Map<u8>() + offset;

        Memory::Copy(data, dst_ptr, size);
    }else{
        // NOTE: Ram should map anyway, if it is not, there is a bug
        BufferImpl tmp(size, BufferMemoryType::RAM, BufferUsageBits::TransferDestination | BufferUsageBits::TransferSource);
        tmp.Copy(data, size, 0);

        Immediate::Copy(&tmp, this, size, 0, offset);
    }
}

bool BufferImpl::IsMappable()const{
    return m_BackingMemoryType == Vk::MemoryType::DynamicVRAM
        || m_BackingMemoryType == Vk::MemoryType::RAM
        || m_BackingMemoryType == Vk::MemoryType::UncachedRAM;
}

bool BufferImpl::IsMapped()const{
    return m_Pointer != nullptr;
}

void *BufferImpl::Map(){
    if(!IsMapped())
        vkMapMemory(GPUImpl::Get(), m_Memory, 0, m_Size, 0, &m_Pointer);
    return m_Pointer;
}

void BufferImpl::Unmap(){
    if(IsMapped()){
        vkUnmapMemory(GPUImpl::Get(), m_Memory);
        m_Pointer = nullptr;
    }
}

void BufferImpl::Realloc(size_t new_size){
    if(IsMapped())
        Unmap();

    BufferUsage usage = m_Usage;
    BufferMemoryType mem_type = m_AbstractMemoryType;

    //XXX: Dirty hack
    this->~BufferImpl();
    new(this)BufferImpl(new_size, mem_type, usage);
}

size_t BufferImpl::Size()const{
    return m_Size;
}

BufferMemoryType BufferImpl::MemoryType()const{
    return m_AbstractMemoryType;
}

BufferUsage BufferImpl::Usage()const{
    return m_Usage;
}

}//namespace Vk::
