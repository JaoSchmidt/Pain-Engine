#pragma once

#include "Core.h"
#include <pain.h>

class MainMap;

namespace Chunk
{
reg::Entity create(pain::Scene &scene, glm::ivec2 offset, int chunkSize,
                   MainMap &mainMap);

class Script : public pain::WorldObject
{
public:
  Script(reg::Entity entity, pain::Scene &scene, glm::ivec2 offSet,
         int chunkSize, MainMap &mainMap);
  // void onCreate();
  // void onUpdate(pain::DeltaTime deltaTime);
  void onRender(pain::Renderer2d &renderer2d, bool isMinimized,
                pain::DeltaTime currentTime);
  bool isOutsideRadius(glm::ivec2 &chunkAt, int radius);
  void setData(std::vector<int> &&data) { m_data = std::move(data); }
  // TODO: Implement chunk file saving
  // - checkChunkOnFile
  // - loadChunkOnFile
  // - saveChunkOnFIle
  int m_offsetX = 0;
  int m_offsetY = 0;

  ~Script() { PLOG_T("Deleting chunk in ({},{})", m_offsetX, m_offsetY); };

private:
  std::vector<int> m_data = {};
  int m_chunkSize = 0;
  pain::TextureSheet &m_spriteSheet;
};

}; // namespace Chunk
