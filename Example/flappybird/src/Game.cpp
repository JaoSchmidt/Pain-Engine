#include "Game.h"
#include <cstdlib>

void Game::onRender(double currentTime) {}
void Game::onEvent(const SDL_Event &event) {}

void Game::onCreate()
{
  const int w = 800;
  const int h = 600;
  std::shared_ptr<pain::OrthoCameraEntity> camera =
      std::make_shared<pain::OrthoCameraEntity>(this, (float)w / h, 1.0f);
  pain::Renderer2d::init(camera);
  camera->addComponent<pain::NativeScriptComponent>()
      .bind<pain::OrthoCameraController>();

  auto texture =
      std::make_shared<pain::Texture>("resources/textures/Player.png");
  m_pplayer = std::make_unique<Player>(this, texture);
  m_pplayer->addComponent<pain::NativeScriptComponent>()
      .bind<PlayerController>();

  for (char i = 0; i < m_numberOfObstacles; i++) {
    m_obstacles.push_back(std::make_unique<Obstacles>(this));
    auto &o = m_obstacles[i]->addComponent<pain::NativeScriptComponent>();
    o.bind<ObstaclesController>();
    initializeScripts(o, *m_obstacles[i]);
  };

  pain::Application::Get().addImGuiInstance((ImGuiInstance *)this);
}

void Game::onUpdate(double deltaTime)
{
  if (m_isRunning) {
    // Overall game
    // 1. if obstacle is outside camera, call onDestroy
    // 2. check if player hits obstacles
    // 3. if hits, remove one life
    // 4. if 0 lifes, score menu

    m_waveColor = fmod(m_waveColor + m_colorInterval * deltaTime, 360.f);

    const auto waveColorRadians = glm::radians(m_waveColor);

    // spawn obstacles
    m_obstaclesInterval -= m_intervalTime * deltaTime;
    if (m_obstaclesInterval <= 0) {
      m_obstaclesInterval = m_maxInterval;
      const float randAngle = ((float)rand() / RAND_MAX) * M_PI * 2;

      m_index = (m_index + 1) % m_numberOfObstacles;
      reviveObstacle(m_index, randAngle, true);
      m_index = (m_index + 1) % m_numberOfObstacles;
      reviveObstacle(m_index, randAngle, false);
    }

    // change obstacle color
    glm::vec3 color(0.2f + sin(waveColorRadians) * 0.6f,               // red
                    0.3f + sin(waveColorRadians + M_PI / 4) * 0.6f,    // green
                    0.4f + sin(waveColorRadians + M_PI * 3 / 4) * 0.6f // blue
    );
    for (char i = 0; i < m_numberOfObstacles; i++) {
      Obstacles *obstacle = (Obstacles *)m_obstacles.at(i).get();
      auto *inst = (ObstaclesController *)obstacle
                       ->getComponent<pain::NativeScriptComponent>()
                       .instance;
      inst->changeColor(color);
      // LOG_I("intersect {}", checkIntersection(*m_pplayer, *obstacle, i));
      // m_collision = checkIntersection(*m_pplayer, *obstacle, i);
    }
  }
}

void Game::removeLife()
{
  m_lifes -= 1;
  if (!m_lifes)
    m_isRunning = false;
}

void Game::reviveObstacle(int index, float randomAngle, bool upsideDown)
{
  const float height = upsideDown
                           ? sin(randomAngle) * 0.7 + 0.75f + m_obstaclesSpacing
                           : sin(randomAngle) * 0.7 - 1.25f;
  ((ObstaclesController *)m_obstacles.at(index)
       ->getComponent<pain::NativeScriptComponent>()
       .instance)
      ->revive(m_defaultObstacleSpeed, height, upsideDown);
}

template <std::size_t T>
glm::vec2 Game::projection(const std::array<glm::vec2, T> &shape,
                           const glm::vec2 &axis)
{
  float min = glm::dot(shape[0], axis);
  float max = min;
  for (size_t i = 1; i < shape.size(); i++) {
    float projection = glm::dot(shape[i], axis);
    min = std::min(min, projection);
    max = std::max(max, projection);
  }
  return {min, max};
}

bool Game::checkIntersection(const Player &player, const Obstacles &obstacle,
                             int index)
{
  auto &ptc = player.getComponent<pain::TransformComponent>();
  auto &prc = player.getComponent<pain::RotationComponent>();
  auto &psc = player.getComponent<pain::SpriteComponent>();
  auto &otc = obstacle.getComponent<pain::TransformComponent>();
  auto &otgc = obstacle.getComponent<pain::TrianguleComponent>();

  // get quad vertices
  constexpr glm::vec4 quadVertexPositions[4] = {
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, 0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
  };
  const glm::mat4 transform = pain::Renderer2d::getTransform(
      ptc.m_position, psc.m_size, prc.m_rotationAngle);

  std::array<glm::vec2, 4> qVertices = {
      transform * quadVertexPositions[0],
      transform * quadVertexPositions[1],
      transform * quadVertexPositions[2],
      transform * quadVertexPositions[3],
  };

  // triangle
  constexpr glm::vec4 triVertexPositions[3] = {
      glm::vec4(0.0f, 0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
  };
  const glm::mat4 transformTri =
      pain::Renderer2d::getTransform(otc.m_position, otgc.m_height);
  const std::array<glm::vec2, 3> tVertices = {
      transformTri * triVertexPositions[0], //
      transformTri * triVertexPositions[1], //
      transformTri * triVertexPositions[2], //
  };

  std::vector<glm::vec2> axes;
  for (size_t i = 0; i < 4; i++) {
    glm::vec2 edge = qVertices[(i + 1) % 4] - qVertices[i];
    glm::vec2 axis(-edge.y, edge.x); // Perpendicular to the edge
    axis = glm::normalize(axis);
    axes.push_back(axis);
  }
  for (size_t i = 0; i < 3; i++) {
    glm::vec2 edge = tVertices[(i + 1) % 3] - tVertices[i];
    glm::vec2 axis(-edge.y, edge.x);
    axis = glm::normalize(axis);
    axes.push_back(axis);
  }
  // Perform SAT check on all axes
  for (const glm::vec2 &axis : axes) {
    auto boundA = projection(qVertices, axis);
    auto boundB = projection(tVertices, axis);

    // Check for overlap
    if (boundA.y < boundB.x || boundB.y < boundA.x) {
      return false; // No collision
    }
  }

  return true;
}

const void Game::onImGuiUpdate()
{
  ImGui::Begin("Player Controller");
  ImGui::Text("Parameters Settings");
  ImGui::InputInt("Number of Obstacles", &m_numberOfObstacles);
  ImGui::InputFloat("Obstacles Spacing", &m_obstaclesSpacing, 0.01f, 1.0f,
                    "%.3f");
  ImGui::InputFloat("Max Interval", &m_maxInterval, 0.1f, 1.0f, "%.3f");
  ImGui::InputFloat("Interval Time", &m_intervalTime, 0.1f, 1.0f, "%.3f");
  ImGui::InputFloat("Obstacle Speed", &m_defaultObstacleSpeed, 0.01f, 1.0f,
                    "%.3f");
  ImGui::InputFloat("Color Interval", &m_colorInterval, 0.1f, 1.0f, "%.3f");
  ImGui::InputFloat("Height Interval", &m_heightInterval, 0.1f, 1.0f, "%.3f");
  ImGui::SeparatorText("Info");
  ImGui::Text("Obstacle Spawn counter: %.2f seconds", m_obstaclesInterval);
  ImGui::Text("Last Obstacle index: %.2d", m_index);
  // ImGui::Text("Collision: %s", m_collision ? "true" : "false");
  ImGui::End();
}
