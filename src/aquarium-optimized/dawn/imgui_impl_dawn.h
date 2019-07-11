// dear imgui: Renderer for Dawn
// This needs to be used along with a Platform Binding (e.g. GLFW)

#pragma once

#include "imgui.h"

#include <dawn/dawncpp.h>
#include <dawn_native/DawnNative.h>
#include "ContextDawn.h"
#include "utils/DawnHelpers.h"

// cmd_list is the command list that the implementation will use to render imgui draw lists.
// Before calling the render function, caller must prepare cmd_list by resetting it and setting the
// appropriate render target and descriptor heap that contains
// font_srv_cpu_desc_handle/font_srv_gpu_desc_handle. font_srv_cpu_desc_handle and
// font_srv_gpu_desc_handle are handles to a single SRV descriptor to use for the internal font
// texture.
IMGUI_IMPL_API bool ImGui_ImplDawn_Init(ContextDawn *context, dawn::TextureFormat rtv_format);
IMGUI_IMPL_API void ImGui_ImplDawn_Shutdown();
IMGUI_IMPL_API void ImGui_ImplDawn_NewFrame();
IMGUI_IMPL_API void ImGui_ImplDawn_RenderDrawData(ImDrawData *draw_data);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_IMPL_API void ImGui_ImplDawn_InvalidateDeviceObjects();
IMGUI_IMPL_API bool ImGui_ImplDawn_CreateDeviceObjects();
