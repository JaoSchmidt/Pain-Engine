#pragma once

#include <pain.h>

class MainMap;

struct ChunkHash {
  size_t operator()(const std::pair<int, int> &p) const
  {
    return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
  }
};

class ChunkEntity : public pain::GameObject<pain::TransformComponent,
                                            pain::NativeScriptComponent>
{
public:
  ChunkEntity(pain::Scene *scene);
};

class ChunkController
    : public pain::ExtendedScriptableEntity<pain::TransformComponent,
                                            pain::NativeScriptComponent>
{
public:
  void init(glm::ivec2 offSet, int chunkSize, MainMap *mainMap);
  void onRender(double currentTime);
  bool isOutsideRadius(glm::ivec2 &chunkAt, int radius);

  // TODO: Implement chunk file saving
  // - checkChunkOnFile
  // - loadChunkOnFile
  // - saveChunkOnFIle
  int m_offsetX = 0;
  int m_offsetY = 0;

private:
  std::vector<int> m_data = {};
  int m_chunkSize = 0;
  MainMap *m_mainMap = nullptr;
  std::vector<int> generateTerrainMatrix(int offsetX = 0, int offsetY = 0);
};
