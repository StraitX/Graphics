#include "core/os/memory.hpp"
#include "core/assert.hpp"
#include "core/log.hpp"
#include "core/push_array.hpp"
#include "graphics/api/vulkan/graphics_api_impl.hpp"
#include "graphics/api/vulkan/gpu_impl.hpp"

namespace Vk{

GPUType VkTypeToGPUType(u32 type){
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:      return GPUType::Discrete;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:    return GPUType::Integrated;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:       return GPUType::Virtual;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:             return GPUType::Unknown;
    default:
        return GPUType::Unknown;
    }
}

int GetPriority(GPUType type){
    switch (type) {
    case GPUType::Unknown: return 0;
    case GPUType::Virtual: return 1;
    case GPUType::Integrated: return 2;
    case GPUType::Discrete: return 3;
    }
}

VkPhysicalDevice PickBestPhysicalDevice(VkInstance instance){
    u32 count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    auto *devices = SX_STACK_ARRAY_ALLOC(VkPhysicalDevice, count);

    vkEnumeratePhysicalDevices(instance, &count, devices);

    VkPhysicalDevice best = devices[0];
    int best_score = 0;
    
    for(u32 i = 0; i<count; i++){
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);

        u32 families_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &families_count, nullptr);

        int score = GetPriority(VkTypeToGPUType(props.deviceType))*10 + families_count * 5; 

        if(score > best_score){
            best = devices[i];
            best_score = score;
        }
    }

    return best;
}

GPUImpl GPUImpl::s_Instance;

Result GPUImpl::Initialize(){
    m_PhysicalHandle = PickBestPhysicalDevice(GraphicsAPIBackendImpl::s_Instance.Instance());

    m_QueueProperties = QueueProperties::Get(m_PhysicalHandle);

    if(m_QueueProperties.Family[QueueFamily::Graphics].Index == InvalidIndex){
        LogError("Vk: GPU: this gpu does not support graphics queue");
        return Result::Unsupported;
    }
    
    PushArray<VkDeviceQueueCreateInfo, QueueFamily::FamilyCount> queues;

    float priority = 1.0;

    for(int i = 0; i<QueueFamily::FamilyCount; i++){
        if(m_QueueProperties.Family[i].Index != InvalidIndex){
            VkDeviceQueueCreateInfo qinfo;
            qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qinfo.pNext = 0;
            qinfo.flags = 0;
            qinfo.queueFamilyIndex = m_QueueProperties.Family[i].Index;
            qinfo.queueCount = 1;
            qinfo.pQueuePriorities = &priority;

            queues.Push(qinfo);
        }
    }

    const char *ext[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceFeatures features; /// XXX
    vkGetPhysicalDeviceFeatures(m_PhysicalHandle, &features);

    VkDeviceCreateInfo info;
    info.sType                  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext                  = nullptr;
    info.flags                  = 0;
    info.enabledExtensionCount  = lengthof(ext);
    info.ppEnabledExtensionNames= ext;
    info.enabledLayerCount      = 0;
    info.ppEnabledLayerNames    = nullptr;
    info.pEnabledFeatures       = &features;
    info.queueCreateInfoCount   = queues.Size();
    info.pQueueCreateInfos      = queues.Data();

    LogInfo("Vk: GPU: Creating device with % queues", info.queueCreateInfoCount);

    if(vkCreateDevice(m_PhysicalHandle, &info, nullptr, &m_Handle) != VK_SUCCESS)
        return Result::NullObject;

    for(int i = 0; i<QueueFamily::FamilyCount; i++){
        if(m_QueueProperties.Family[i].Index != InvalidIndex)
            vkGetDeviceQueue(m_Handle, m_QueueProperties.Family[i].Index, 0, &m_Queues[i]);
    }

    for(int i = 1; i<QueueFamily::FamilyCount; i++)
        if(m_Queues[i] == VK_NULL_HANDLE)
            m_Queues[i] = m_Queues[i - 1];
    
    SX_ASSERT(m_Queues[QueueFamily::Graphics] != VK_NULL_HANDLE);
    SX_ASSERT(m_Queues[QueueFamily::Compute] != VK_NULL_HANDLE);
    SX_ASSERT(m_Queues[QueueFamily::Transfer] != VK_NULL_HANDLE);

    return Result::Success;
}

void GPUImpl::Finalize(){
    vkDeviceWaitIdle(m_Handle);
    
    vkDestroyDevice(m_Handle,nullptr);
}


}//namespace Vk::