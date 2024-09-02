#include "Game.h"
#include <cstdlib>

void Game::onRender() {}
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
      ((ObstaclesController *)m_obstacles.at(i)
           ->getComponent<pain::NativeScriptComponent>()
           .instance)
          ->changeColor(color);
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
  ImGui::End();
}
