/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Editor.h"

#include <imgui.h>
#include <pain.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CoreFiles/Application.h"
#include "CoreRender/Buffers/FrameBuffer.h"
#include "CustomPanel.h"
#include "ImGuiEmplacer.h"
#include "LuaImGuiBinder.h"
#include "LuaImPlotBinder.h"
#include "Misc/Events.h"
#include "imgui_internal.h"

void showStats(const Stats &s)
{
  ImGui::TextColored(ImVec4(1.0F, 0.6F, 0.2F, 1.0F), "%s instances: %d", s.name,
                     s.count);

  // Small indented box for extra stats
  if (ImGui::TreeNodeEx((void *)(&s), // unique ID
                        ImGuiTreeNodeFlags_Framed |
                            ImGuiTreeNodeFlags_SpanAvailWidth,
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

namespace painless
{
void Editor::onRender(pain::Renderers &renderers, bool isMinimized,
                      pain::DeltaTime dt)
{
  UNUSED(isMinimized)
  if (!m_app.getFrameInfo().swapChainTarget) {
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::Begin("DockSpace Demo", &m_dockspaceOpen, m_windowFlags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspaceID, ImVec2(0.0F, 0.0F), dockspaceFlags);
      if (!m_dockspaceInitialized) {
        m_dockspaceInitialized = true;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_CentralNode);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        // Splits
        ImGuiID dockerIDSidebar, dockerIDViewport;
        ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.25F,
                                    &dockerIDSidebar, &dockerIDViewport);

        // Windows
        ImGui::DockBuilderDockWindow("Stats", dockerIDSidebar);
        ImGui::DockBuilderDockWindow("Viewport", dockerIDViewport);
        customPanel::buildDockerWindow(dockerIDSidebar, dockerIDViewport);
        ImGui::DockBuilderFinish(dockspaceID);
      }
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

    ImGui::Begin("Stats");

    // -------------------------------------------------- //

    if (ImGui::TreeNodeEx("Renderer2D Stats:", // unique ID
                          ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth,
                          "Renderer2D Stats:")) {
      showStats(renderers.m_renderer2d.getQuadStatistics());
      showStats(renderers.m_renderer2d.getTextStatistics());
      showStats(renderers.m_renderer2d.getSprayStatistics());
      showStats(renderers.m_renderer2d.getTriStatistics());
      ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Renderer3D Stats:", // unique ID
                          ImGuiTreeNodeFlags_Framed |
                              ImGuiTreeNodeFlags_SpanAvailWidth,
                          "Renderer3D Stats:")) {
      showStats(renderers.m_renderer3d.getCubeStatistics());
      showStats(renderers.m_renderer3d.getSphereStatistics());
      ImGui::TreePop();
    }
    m_imGuiDebugMenu.onRender(renderers, isMinimized, dt);
    ImGui::End();

    ImGui::Begin("Viewport");
    bool isNowFocused = (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) ||
                        ImGui::IsWindowFocused();

    m_app.setFocusedOrHovered(isNowFocused);
    if (!isNowFocused && m_wasFocused) {
      m_app.setFocusedOrHovered(false);
    }

    m_wasFocused = isNowFocused;

    uint32_t textureID = m_app.getFrameInfo().colorAttachmentTextureId;
    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (textureID) {
      if (avail.x > 0 && avail.y > 0 &&
          (avail.x != m_avail.x || avail.y != m_avail.y)) {
        m_avail = avail;
        pain::Renderers::setViewPort(0, 0, avail.x, avail.y);
        getEventDispatcher().enqueue<pain::ImGuiViewportChangeEvent>(
            {glm::vec2(avail.x, avail.y)});
      }
      ImGui::Image((ImTextureID)(uintptr_t)textureID, m_avail, {0, 1}, {1, 0});
    }
    ImGui::End(); // "Viewport"

    customPanel::renderAll();

    ImGui::End();
  }
}

Editor &Editor::create(pain::UIScene &uiScene, pain::Application &app)
{
  logWrapper::s_initEditorLogger();
  Editor &editor =
      painless::emplaceImGuiScript<Editor>(uiScene.getEntity(), uiScene, app);
  return editor;
}

Editor::Editor(reg::Entity entity, pain::UIScene &scene, pain::Application &app)
    : pain::UIObject(entity, scene), m_app(app), m_imGuiDebugMenu()
{
  luabinder::bindImGui(m_app.getLuaState());
  // luabinder::bindImPlot(m_app.getLuaState());
}

Editor::~Editor()
{
  m_imGuiDebugMenu.onDestroy();
  luabinder::unbindImGui(m_app.getLuaState());
  // luabinder::unbindImPlot(m_app.getLuaState());
  painless::customPanel::deleteAll();
}

} // namespace painless
