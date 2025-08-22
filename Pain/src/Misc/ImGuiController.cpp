#include "CoreFiles/ImGuiController.h"

#include "CoreFiles/LogWrapper.h"
#include "SDL_video.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

namespace pain
{

ImGuiController::ImGuiController(void *context, SDL_Window *window,
                                 ImGuiConfigFlags flags)
{
  P_ASSERT(m_io != nullptr,
           "Trying to create a new ImGui context will undermine the singleton "
           "approach, read \"HACK\" inside ImGuiController class");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  m_io = &ImGui::GetIO();
  (void)m_io;
  m_io->ConfigFlags |= flags;
  ImGui::StyleColorsDark();

  ImGuiStyle &style = ImGui::GetStyle();
  if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 430"); // OpenGL 3.0 and above

  m_showAnotherWindow = false;
  m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

// NOTE: Consider the possibility of enabling a "preupdate" and "posupadte"
// function for each specific system in the ECS, this could enable you to easily
// make this a simple game object instead of a extraneous insertion inside the
// onUpdate loop as it is coded right now
void ImGuiController::onUpdate(bool isMinimized)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  if (!isMinimized) {
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    for (ImGuiInstance *i : m_imguiInstances) {
      i->onImGuiUpdate();
    }
  }
}

void ImGuiController::onEvent(const SDL_Event &event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);
}
void ImGuiController::onRender()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
  }
}

ImGuiController::~ImGuiController()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  m_io = nullptr;
}

} // namespace pain
