#pragma once
#include "CoreRender/FrameBuffer.h"
#include <imgui.h>
#include <pain.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class PainlessEditor : public pain::ExtendedEntity
{

public:
  ~PainlessEditor() = default;
  NONCOPYABLE(PainlessEditor);
  NONMOVABLE(PainlessEditor);
  // void init(Application *app) { m_app = app; }
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                pain::DeltaTime currentTime)
  {
    UNUSED(renderer)
    UNUSED(currentTime)
    UNUSED(isMinimized)

    if (!m_app.getFrameInfo().swapChainTarget) {
      static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

      // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
      // window not dockable into, because it would be confusing to have two
      // docking targets within each others.
      ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
      // render our background and handle the pass-thru hole, so we ask Begin()
      // to not render a background.
      if (ImGuiDockNodeFlags_PassthruCentralNode)
        m_windowFlags |= ImGuiWindowFlags_NoBackground;

      // Important: note that we proceed even if Begin() returns false (aka
      // window is collapsed). This is because we want to keep our DockSpace()
      // active. If a DockSpace() is inactive, all active windows docked into it
      // will lose their parent and become undocked. We cannot preserve the
      // docking relationship between an active window and an inactive docking,
      // otherwise any change of dockspace/settings would lead to windows being
      // stuck in limbo and never being visible.
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("DockSpace Demo", &m_dockspaceOpen, m_windowFlags);
      ImGui::PopStyleVar();

      ImGui::PopStyleVar(2);

      // DockSpace
      ImGuiIO &io = ImGui::GetIO();
      if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
      }

      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          // Disabling fullscreen would allow the window to be moved to the
          // front of other windows, which we can't undo at the moment without
          // finer window depth/z control. ImGui::MenuItem("Fullscreen", NULL,
          // &opt_fullscreen_persistant);

          if (ImGui::MenuItem("Exit"))
            m_app.stopLoop();
          ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
      }

      ImGui::Begin("Settings");

      // auto stats = Hazel::Renderer2D::GetStats();
      ImGui::Text("Renderer2D Stats:");
      ImGui::Text("Draw Calls: %d", 0);
      ImGui::Text("Quads: %d", 0);
      ImGui::Text("Vertices: %d", 0);
      ImGui::Text("Indices: %d", 0);
      // ImGui::Text("Draw Calls: %d", stats.DrawCalls);
      // ImGui::Text("Quads: %d", stats.QuadCount);
      // ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
      // ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

      uint32_t textureID = m_app.getFrameInfo().colorAttachmentTextureId;
      if (textureID)
        ImGui::Image(static_cast<ImTextureID>(textureID),
                     ImVec2{static_cast<float>(m_app.getFrameInfo().width),
                            static_cast<float>(m_app.getFrameInfo().height)});

      ImGui::End();

      ImGui::End();
    }
  }

  PainlessEditor(reg::Entity entity, pain::Scene &scene, pain::Application &app)
      : ExtendedEntity(entity, scene), m_app(app) {};
  std::vector<std::string> m_availableResolutions;

private:
  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  bool m_dockspaceOpen = true;
  pain::Application &m_app;
};
