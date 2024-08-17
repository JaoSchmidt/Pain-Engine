#include "initialMap.h"

MainMap::MainMap(float spriteWidth, float spriteHeight)
    : m_spriteSize({spriteWidth, spriteHeight})
{
  m_spriteSheet.reset(
      new pain::Texture("resources/textures/kenney_roguelike/Spritesheet/"
                        "roguelikeSheet_transparent.png"));
  // clang-format off
  // lake
  m_texturesIds.push_back(createVecFromCoord(0, 6)); // 0. transparent alpha
  m_texturesIds.push_back(createVecFromCoord(45, 19)); // 1. gold coins (for testing)
  m_texturesIds.push_back(createVecFromCoord(2, 30)); // 2. lake northwest corner
  m_texturesIds.push_back(createVecFromCoord(3, 30)); // 3. lake north corner
  m_texturesIds.push_back(createVecFromCoord(4, 30)); // 4. lake northeast corner
  m_texturesIds.push_back(createVecFromCoord(2, 29)); // 5. lake west corner
  m_texturesIds.push_back(createVecFromCoord(4, 29)); // 6. lake east corner
  m_texturesIds.push_back(createVecFromCoord(2, 28)); // 7. lake southwest corner
  m_texturesIds.push_back(createVecFromCoord(3, 28)); // 8. lake south corner
  m_texturesIds.push_back(createVecFromCoord(4, 28)); // 9. lake southeast corner

  // terrain
  m_texturesIds.push_back(createVecFromCoord(5, 30)); // 10. grass 1
  m_texturesIds.push_back(createVecFromCoord(5, 29)); // 11. grass 2

  // paths
  m_texturesIds.push_back(createVecFromCoord(9, 23));  // 12. path vertical
  m_texturesIds.push_back(createVecFromCoord(9, 22));  // 13. path horizontal
  m_texturesIds.push_back(createVecFromCoord(7, 23));  // 14. path 90 degrees (1)
  m_texturesIds.push_back(createVecFromCoord(7, 22));  // 15. path 90 degrees (2)
  m_texturesIds.push_back(createVecFromCoord(8, 23));  // 16. path 90 degrees (3)
  m_texturesIds.push_back(createVecFromCoord(8, 22));  // 17. path 90 degrees (4)
  m_texturesIds.push_back(createVecFromCoord(5, 23));  // 18. path T-junction (1)
  m_texturesIds.push_back(createVecFromCoord(5, 22));  // 19. path T-junction (2)
  m_texturesIds.push_back(createVecFromCoord(6, 23));  // 20. path T-junction (3)
  m_texturesIds.push_back(createVecFromCoord(6, 22));  // 21. path T-junction (4)
  m_texturesIds.push_back(createVecFromCoord(11, 23)); // 22. path cross (X)
  // lake open corners
  m_texturesIds.push_back(createVecFromCoord(0, 29)); // 23. lake southeast open corner
  m_texturesIds.push_back(createVecFromCoord(1, 29)); // 24. lake southwest open corner
  m_texturesIds.push_back(createVecFromCoord(0, 28)); // 25. lake northwest open corner
  m_texturesIds.push_back(createVecFromCoord(1, 28)); // 26. lake northeast open corner
  // tree
  m_texturesIds.push_back(createVecFromCoord(16, 20)); // 27. pine tree top
  m_texturesIds.push_back(createVecFromCoord(16, 19)); // 28. pine tree bottom

  // lake
  m_texturesIds.push_back(createVecFromCoord(0, 30)); // 29. lake 1
  m_texturesIds.push_back(createVecFromCoord(1, 30)); // 30 lake 2
  m_texturesIds.push_back(createVecFromCoord(55, 7)); // 31 lake rock
  m_defaultMap = {
      {10, 10, 10, 10, 11, 10, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 10, 11, 10, 11, 10, 11},
      {10, 10, 10, 11, 10, 11, 02, 03, 03, 03, 03, 03, 03, 04, 11},
      {10, 10, 10, 11, 10, 11, 05, 30, 29, 30, 29, 30, 29, 06, 11},
      {10, 10, 10, 11, 10, 11, 05, 29, 30, 29, 31, 29, 29, 06, 11},
      {10, 10, 10, 11, 10, 11, 05, 30, 29, 30, 29, 30, 29, 06, 11},
      {10, 10, 10, 11, 10, 11, 07,  8,  8, 24, 30, 29, 23,  9, 11},
      {10, 10, 10, 11, 10, 11, 10, 11, 11, 07,  8,  8,  9, 10, 11},
  };
  m_sceneryMap = {
      {00, 27, 00, 27, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {27, 28, 27, 28, 27, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {28, 27, 28, 27, 28, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {27, 28, 27, 28, 27, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {28, 00, 28, 00, 28, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {13, 13, 13, 13, 13, 20, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 19, 13, 13, 13, 13, 13, 13, 13, 13, 13},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
      {00, 00, 00, 00, 00, 12, 00, 00, 00, 00, 00, 00, 00, 00, 00},
  };
  // clang-format on
}
std::array<glm::vec2, 4> MainMap::createVecFromCoord(int x, int y)
{
  float m_spriteMargin = 1.0f;
  glm::vec2 min = {
      x * (m_spriteSize.x + m_spriteMargin) / m_spriteSheet->getWidth(),
      y * (m_spriteSize.y + m_spriteMargin) / m_spriteSheet->getHeight()};
  glm::vec2 max = {((x + 1) * m_spriteSize.x + x * m_spriteMargin) /
                       m_spriteSheet->getWidth(),
                   ((y + 1) * m_spriteSize.y + y * m_spriteMargin) /
                       m_spriteSheet->getHeight()};
  return {
      glm::vec2(min.x, min.y),
      glm::vec2(max.x, min.y),
      glm::vec2(max.x, max.y),
      glm::vec2(min.x, max.y),
  };
}

const std::vector<std::vector<int>> &MainMap::getDefaultMap() const
{
  return m_defaultMap;
}
const std::vector<std::vector<int>> &MainMap::getSceneryMap() const
{
  return m_sceneryMap;
}
