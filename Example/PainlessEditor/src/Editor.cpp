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
#include "Misc/Events.h"
#include "imgui_internal.h"
#include "implot.h"

void showStats(const Stats &s)
{
  ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "%s instances: %d", s.name,
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
        ImGuiID dock_id_left, dock_id_center, dock_id_bottom;

        // Splits
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f,
                                    &dock_id_left, &dock_id_center);
        ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Down, 0.5f,
                                    &dock_id_bottom, &dock_id_center);
        // Windows
        ImGui::DockBuilderDockWindow("Stats", dock_id_left);
        ImGui::DockBuilderDockWindow("Viewport", dock_id_center);
        ImGui::DockBuilderDockWindow("Panel", dock_id_bottom);
        ImGui::DockBuilderFinish(dockspace_id);
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
    ImGui::Text("Renderer2D Stats:");
    showStats(renderers.m_renderer2d.getQuadStatistics());
    showStats(renderers.m_renderer2d.getTextStatistics());
    showStats(renderers.m_renderer2d.getSprayStatistics());
    showStats(renderers.m_renderer2d.getTriStatistics());

    ImGui::Text("Renderer3D Stats:");
    showStats(renderers.m_renderer3d.getCubeStatistics());
    showStats(renderers.m_renderer3d.getSphereStatistics());
    m_imGuiDebugMenu.onRender(renderers, isMinimized, dt);
    ImGui::End();

    ImGui::Begin("Viewport");
    uint32_t textureID = m_app.getFrameInfo().colorAttachmentTextureId;
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float splitterThickness = 4.0f;

    if (textureID) {
      if (avail.x != m_avail.x || avail.y != m_avail.y) {
        m_avail = avail;
        renderers.setViewPort(0, 0, avail.x, avail.y);
        getEventDispatcher().enqueue<pain::ImGuiViewportChangeEvent>(
            {glm::vec2(avail.x, avail.y)});
      }
      ImGui::Image((ImTextureID)(uintptr_t)textureID, m_avail, {0, 1}, {1, 0});
    }
    ImGui::End(); // "Viewport"

    // Small demo

    ImGui::Begin("Panel");

    float xs1[1001], ys1[1001];
    double xs2[20], ys2[20];

    for (int i = 0; i < 1001; ++i) {
      xs1[i] = i * 0.001f;
      ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
    }
    for (int i = 0; i < 20; ++i) {
      xs2[i] = i * 1 / 19.0f;
      ys2[i] = xs2[i] * xs2[i];
    }
    avail = ImGui::GetContentRegionAvail();
    if (ImPlot::BeginPlot("Line Plots", avail)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
      ImPlot::PlotLine("g(x)", xs2, ys2, 20,
                       {ImPlotProp_Marker, ImPlotMarker_Circle,
                        ImPlotProp_Flags, ImPlotLineFlags_Segments});
      ImPlot::EndPlot();
    }

    ImGui::End(); // Port

    // ImGui::Begin("Panel");
    //
    // ImGui::End();

    ImGui::End();
  }
}

Editor::Editor(reg::Entity entity, pain::UIScene &scene, pain::Application &app)
    : pain::UIObject(entity, scene), m_app(app), m_imGuiDebugMenu() {};

} // namespace painless
