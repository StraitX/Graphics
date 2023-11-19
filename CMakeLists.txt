cmake_minimum_required(VERSION 3.10)

project(
    StraitXGraphics
    DESCRIPTION "StraitXGraphics package"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)

include(config/cmake/Config.cmake)

set(SX_GRAPHICS_SOURCES_DIR ${PROJECT_SOURCE_DIR}/sources)
set(SX_GRAPHICS_THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

set(SX_GRAPHICS_SOURCES_GRAPHICS_API
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/graphics_api.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/graphics_resource.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/gpu.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/texture.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/semaphore.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/swapchain.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/fence.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/command_buffer.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/render_pass.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/framebuffer.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/shader.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/graphics_pipeline.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/compute_pipeline.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/buffer.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/sampler.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/descriptor_set.cpp
)

set(SX_GRAPHICS_SOURCES_GRAPHICS_API_VULKAN
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/debug.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/graphics_api_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/gpu_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/queue.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/semaphore_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/swapchain_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/fence_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/command_buffer_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/texture_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/render_pass_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/framebuffer_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/shader_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/pipeline.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/graphics_pipeline_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/compute_pipeline_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/memory.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/memory_allocator.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/buffer_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/immediate.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/sampler_impl.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/api/vulkan/descriptor_set_impl.cpp
)

set(SX_GRAPHICS_SOURCES_GRAPHICS
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/color.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/image_loader.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/image.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/render_window.cpp
    ${SX_GRAPHICS_SOURCES_DIR}/graphics/render_texture.cpp
)

if(STRAITX_PLATFORM_LINUX)

    set(SX_GRAPHICS_SOURCES_PLATFORM
        ${SX_GRAPHICS_SOURCES_DIR}/platform/linux/glloader_impl.cpp
        ${SX_GRAPHICS_SOURCES_DIR}/platform/linux/opengl_context_impl.cpp
        ${SX_GRAPHICS_SOURCES_DIR}/platform/linux/vulkan_linux.cpp
        ${SX_GRAPHICS_SOURCES_DIR}/platform/linux/vulkan_surface_impl.cpp
    )

    set(SX_GRAPHICS_LIBS_PLATFORM
        GLX
        GL
        vulkan
    )

    set(SX_GRAPHICS_LIBS_DIRS_PLATFORM
        ${SX_GRAPHICS_THIRDPARTY_DIR}/vulkan-sdk/linux/lib
    )

    set(SX_GRAPHICS_LINKER_FLAGS_PLATFORM
    )

elseif(STRAITX_PLATFORM_WINDOWS)
    set(SX_GRAPHICS_SOURCES_PLATFORM
       ${SX_GRAPHICS_SOURCES_DIR}/platform/windows/glloader_impl.cpp
       ${SX_GRAPHICS_SOURCES_DIR}/platform/windows/wgl.cpp
       ${SX_GRAPHICS_SOURCES_DIR}/platform/windows/opengl_context_impl.cpp
       ${SX_GRAPHICS_SOURCES_DIR}/platform/windows/vulkan_windows.cpp
       ${SX_GRAPHICS_SOURCES_DIR}/platform/windows/vulkan_surface_impl.cpp
    )

    set(SX_GRAPHICS_LIBS_PLATFORM
        opengl32
        vulkan-1
    )

    set(SX_GRAPHICS_LIBS_DIRS_PLATFORM
        ${SX_GRAPHICS_THIRDPARTY_DIR}/vulkan-sdk/windows/lib
    )

    set(SX_GRAPHICS_LINKER_FLAGS_PLATFORM

    )

elseif(STRAITX_PLATFORM_MACOS)
    set(SX_GRAPHICS_SOURCES_PLATFORM
        ${SX_GRAPHICS_SOURCES_DIR}/platform/macos/glloader_impl.mm
        ${SX_GRAPHICS_SOURCES_DIR}/platform/macos/opengl_context_impl.mm
        ${SX_GRAPHICS_SOURCES_DIR}/platform/macos/vulkan_macos.cpp
        ${SX_GRAPHICS_SOURCES_DIR}/platform/macos/vulkan_surface_impl.mm
    )

    set(SX_GRAPHICS_LIBS_PLATFORM
        "-framework Cocoa"
        "-framework AppKit"
        "-framework CoreData"
        "-framework Foundation"
        "-framework Metal"
        "-framework IOKit"
        "-framework IOSurface"
        "-framework QuartzCore"
        MoltenVK
    )

    set(SX_GRAPHICS_LIBS_DIRS_PLATFORM
        ${SX_GRAPHICS_THIRDPARTY_DIR}/vulkan-sdk/apple/lib
    )

    set(SX_GRAPHICS_LINKER_FLAGS_PLATFORM

    )

else()
    message(FATAL_ERROR "Your platform is not supported yet")
endif()


add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/stb_image)

if(STRAITX_SUPPORT_VULKAN)
	add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/glsl2spv)
endif()


set(SX_GRAPHICS_INCLUDES
    ${SX_GRAPHICS_SOURCES_DIR}
    ${SX_GRAPHICS_THIRDPARTY_DIR}/glad/include
    ${SX_GRAPHICS_THIRDPARTY_DIR}/vulkan-sdk/include
    ${SX_GRAPHICS_THIRDPARTY_DIR}/stb_image
    ${SX_GRAPHICS_THIRDPARTY_DIR}/glsl2spv
)

set(SX_GRAPHICS_LIBS
    glad
    stb_image
    ${SX_GRAPHICS_LIBS_PLATFORM}
)
if(STRAITX_SUPPORT_VULKAN)
	list(APPEND SX_GRAPHICS_LIBS glsl2spv)
endif()

add_library(StraitXGraphics STATIC 
    ${SX_GRAPHICS_SOURCES_PLATFORM} 
    ${SX_GRAPHICS_SOURCES_GRAPHICS}
    ${SX_GRAPHICS_SOURCES_GRAPHICS_API}
    ${SX_GRAPHICS_SOURCES_GRAPHICS_API_VULKAN}
)

target_link_options(StraitXGraphics
    PUBLIC ${SX_GRAPHICS_LINKER_FLAGS_PLATFORM}
)

target_include_directories(StraitXGraphics
    PUBLIC ${SX_GRAPHICS_INCLUDES}
)
target_link_libraries(StraitXGraphics
    PRIVATE ${SX_GRAPHICS_LIBS}
    PRIVATE ${CMAKE_DL_LIBS}
    PUBLIC StraitXCore
)
target_link_directories(StraitXGraphics
    PUBLIC ${SX_GRAPHICS_LIBS_DIRS_PLATFORM}
)