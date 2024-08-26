#pragma once

#include <pain.h>

class ImGuiController : public pain::ImGuiInstance
{
public:
  const void onImGuiUpdate() override
  {
    ImGui::Begin("Game Controller");
    ImGui::Text("Player");
    ImGui::InputFloat2("Position", m_vecPos);
    ImGui::InputFloat2("Size", m_vecSize);
    ImGui::ColorEdit4("Color", m_vecColor);
    ImGui::End();
  }

  float m_vecPos[2] = {0.0f, -0.80f};
  float m_vecSize[2] = {0.3f, 0.3f};
  float m_vecColor[4] = {0.9f, 0.3f, 0.2f, 1.0f};
};
