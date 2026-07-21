/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ImGuiSys.h"

#include "CoreFiles/LogWrapper.h"
#include "ImGuiComponent.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

namespace pain
{
namespace Systems
{
ImGuiSys::ImGuiSys(reg::ArcheRegistry<UIComponents> &archetype,
                   reg::EventDispatcher &eventDispatcher, SDL_GLContext context,
                   SDL_Window *window, ImGuiConfigFlags flags)
    : System(archetype, eventDispatcher)
{
  P_ASSERT(m_io == nullptr,
           "Trying to create a new ImGui context will undermine the singleton "
           "approach, read HACK inside ImGuiSystem.h class");
  ::IMGUI_CHECKVERSION();
  ::ImGui::CreateContext();
  ImPlot::CreateContext();
  m_io = &::ImGui::GetIO();
  (void)m_io;
  m_io->ConfigFlags |= flags;
  ::ImGui::StyleColorsDark();

  ImGuiStyle &style = ::ImGui::GetStyle();
  if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0F;
    style.Colors[ImGuiCol_WindowBg].w = 1.0F;
  }
  // TODO: this need to be moved to the OpenGL backend
  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 430"); // OpenGL 3.0 and above

  m_showAnotherWindow = false;
  m_clearColor = ImVec4(0.45F, 0.55F, 0.60F, 1.00F);

  // fonts
  //   static ImWchar ranges[] = {0x1, static_cast<ImWchar>(0x1FFFF), 0};
  //   static ImFontConfig cfg;
  //   cfg.MergeMode = true;
  //   cfg.FontLoaderFlags |= ImGuiFreeTypeLoaderFlags_LoadColor;
  // #ifdef PLATFORM_IS_WINDOWS
  //   m_io->Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 16.0f,
  //                                   &cfg);
  // #else
  //   m_io->Fonts->AddFontFromFileTTF("resources/default/fonts/NotoColorEmoji.ttf",
  //                                   16.0f);
  // #endif
}

void ImGuiSys::onEvent(const SDL_Event &event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);
  auto chunks = query<painless::ImGuiComponent>();
  for (auto &chunk : chunks) {
    auto *__restrict nsc = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      if (nsc[i].instance && nsc[i].onEventFunction)
        nsc[i].onEventFunction(nsc[i].instance.get(), event);
    }
  }
}
void ImGuiSys::onRender(RenderApi &renderer, DeltaTime currentTime)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ::ImGui::NewFrame();

  // ::ImGui::ShowDemoWindow();
  // ::ImPlot::ShowDemoWindow();
  auto chunks = query<painless::ImGuiComponent>();
  for (auto &chunk : chunks) {
    auto *__restrict nscs = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      auto &nsc = nscs[i];
      if (nsc.instance && nsc.onRenderFunction)
        nsc.onRenderFunction(nsc.instance.get(), renderer, currentTime);
    }
  }

  ::ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
  if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ::ImGui::UpdatePlatformWindows();
    ::ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
  }
}

Systems::ImGuiSys::~ImGuiSys()
{
  ELOG_T("Shut ImGui System");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImPlot::DestroyContext();
  ::ImGui::DestroyContext();
  m_io = nullptr;
  // delete m_io;
}

} // namespace Systems
} // namespace pain
