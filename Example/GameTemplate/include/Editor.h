/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <imgui.h>
#include <pain.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CoreFiles/Application.h"
#include "CoreRender/FrameBuffer.h"
#include "Misc/Events.h"
#include "imgui_internal.h"
class PainlessEditor : public pain::UIObject {
 public:
  ~PainlessEditor() { m_imGuiDebugMenu.onDestroy(); };
  NONCOPYABLE(PainlessEditor);
  NONMOVABLE(PainlessEditor);
  // void init(Application *app) { m_app = app; }

  void onRender(pain::Renderers &renderers, bool isMinimized,
                pain::DeltaTime dt) {
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
      if constexpr (ImGuiDockNodeFlags_PassthruCentralNode)
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
        static bool dockspace_initialized = false;
        if (!dockspace_initialized) {
          dockspace_initialized = true;
          ImGui::DockBuilderRemoveNode(dockspace_id);
          ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
          ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
          ImGuiID dock_id_left, dock_id_center;

          // Split: 25% left
          ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f,
                                      &dock_id_left, &dock_id_center);
          ImGui::DockBuilderDockWindow("Stats", dock_id_left);
          ImGui::DockBuilderDockWindow("Viewport", dock_id_center);
          ImGui::DockBuilderFinish(dockspace_id);
        }
      }

      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          // Disabling fullscreen would allow the window to be moved to the
          // front of other windows, which we can't undo at the moment without
          // finer window depth/z control. ImGui::MenuItem("Fullscreen", NULL,
          // &opt_fullscreen_persistant);

          if (ImGui::MenuItem("Exit")) m_app.stopLoop();
          ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
      }

      ImGui::Begin("Stats");

      // -------------------------------------------------- //

      ImGui::Text("Renderer2D Stats:");
      showStats(renderers.renderer2d.getStatistics<pain::QuadBatch>());
      showStats(renderers.renderer2d.getStatistics<pain::CircleBatch>());
      showStats(renderers.renderer2d.getStatistics<pain::TextBatch>());
      showStats(renderers.renderer2d.getStatistics<pain::SprayBatch>());
      showStats(renderers.renderer2d.getStatistics<pain::TriBatch>());
      m_imGuiDebugMenu.onRender(renderers, isMinimized, dt);

      ImGui::Text("Renderer3D Stats:");
      showStats(renderers.renderer3d.getStatistics<pain::CubeBatch>());
      m_imGuiDebugMenu.onRender(renderers, isMinimized, dt);
      ImGui::End();

      ImGui::Begin("Viewport");
      uint32_t textureID = m_app.getFrameInfo().colorAttachmentTextureId;
      if (textureID) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x != m_avail.x || avail.y != m_avail.y) {
          m_avail = avail;
          renderers.renderer2d.setViewport(0, 0, avail.x, avail.y);
          getEventDispatcher().enqueue<pain::ImGuiViewportChangeEvent>(
              {glm::vec2(avail.x, avail.y)});
        }

        ImGui::Image((ImTextureID)(uintptr_t)textureID, m_avail, {0, 1},
                     {1, 0});
      }
      ImGui::End();

      ImGui::End();
    }
  }

  PainlessEditor(reg::Entity entity, pain::UIScene &scene,
                 pain::Application &app)
      : pain::UIObject(entity, scene), m_app(app), m_imGuiDebugMenu() {};
  std::vector<std::string> m_availableResolutions;

 private:
  template <typename Stats>
    requires std::same_as<Stats, pain::Renderer2d::Stats> ||
             std::same_as<Stats, pain::Renderer3d::Stats>
  void showStats(const Stats &s) {
    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "%s instances: %d",
                       s.name, s.count);

    // Small indented box for extra stats
    if (ImGui::TreeNodeEx(
            (void *)(&s),  // unique ID
            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth,
            "Details")) {
      ImGui::BeginChild("DetailsBox",
                        ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4),
                        true);

      ImGui::Text("Draw Calls: %d", s.draws);
      ImGui::Text("Vertices:  %d", s.vertices);
      ImGui::Text("Indices:   %d", s.indices);

      ImGui::EndChild();
      ImGui::TreePop();
    }
  };
  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImVec2 m_avail = {200.f, 200.f};
  bool m_dockspaceOpen = true;
  pain::Application &m_app;
  pain::ImGuiDebugMenu::Script m_imGuiDebugMenu;
};
