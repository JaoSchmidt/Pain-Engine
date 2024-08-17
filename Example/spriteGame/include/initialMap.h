#include <pain.h>
#include <vector>

struct SheetCoord {
  int x;
  int y;
};

class MainMap
{
public:
  MainMap(float spriteWidth, float spriteHeight);

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  const std::array<glm::vec2, 4> getTexCoord(int id) const
  {
    return m_texturesIds.at(id);
  };
  const float getSpriteWidth() const { return m_spriteSize.x; }
  const float getSpriteHeight() const { return m_spriteSize.y; }
  const std::shared_ptr<pain::Texture> getTexture() const
  {
    return m_spriteSheet;
  }

private:
  std::array<glm::vec2, 4> createVecFromCoord(int x, int y);
  std::vector<std::vector<int>> m_defaultMap;
  std::vector<std::vector<int>> m_sceneryMap;
  void createVecFromMap();
  glm::vec2 m_spriteSize = {0.0f, 0.0f};
  std::shared_ptr<pain::Texture> m_spriteSheet;
  std::vector<std::array<glm::vec2, 4>> m_texturesIds = {};
  std::vector<std::vector<glm::vec2[4]>> m_texturesCoord = {};
};
