#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

#include "initialMap.h"

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene(), m_mainMap(16.0f, 16.0f) {}
  void init(pain::OrthoCameraEntity *pCamera)
  {
    m_orthocamera = pCamera;
    pain::Renderer2d::init(m_orthocamera);
    auto &a = m_orthocamera->addComponent<pain::NativeScriptComponent>();
    a.bind<pain::OrthoCameraController>();
    m_texture.reset(new pain::Texture("resources/textures/Checkerboard.png"));
  }

  void onUpdate(double deltaTime) override
  {
    // m_orthocamera->onUpdate(deltaTime);
    const std::vector<std::vector<int>> mdm = m_mainMap.getDefaultMap();
    const std::vector<std::vector<int>> msm = m_mainMap.getSceneryMap();
    pain::Renderer2d::beginScene();

    for (unsigned int i = 0; i < mdm[0].size(); i++) {
      for (unsigned int j = 0; j < mdm.size(); j++) {
        pain::Renderer2d::drawQuad(
            {1.f * i, -1.f * j}, {1.f, 1.f}, m_mainMap.getTexture(), 1.0f,
            {1.0f, 1.0f, 1.0f, 1.0f}, m_mainMap.getTexCoord(mdm[j][i]));
      }
    }
    for (unsigned int i = 0; i < msm[0].size(); i++) {
      for (unsigned int j = 0; j < msm.size(); j++) {
        if (msm[j][i] != 00)
          pain::Renderer2d::drawQuad(
              {1.f * i, -1.f * j}, {1.f, 1.f}, m_mainMap.getTexture(), 1.0f,
              {1.0f, 1.0f, 1.0f, 1.0f}, m_mainMap.getTexCoord(msm[j][i]));
      }
    }
    // pain::Renderer2d::drawQuad({0.0f, -0.8f}, {0.3f, 0.3f},
    //                            {0.9f, 0.3f, 0.2f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f},
    //                            {0.8f, 0.9f, 0.3f, 1.0f});
    // pain::Renderer2d::drawQuad({0.0f, 0.0f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, -0.5f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
    pain::Renderer2d::endScene();
  }
  void onEvent(const SDL_Event &event) override
  {
    // m_orthocamera->onEvent(event);
  }

private:
  std::shared_ptr<pain::Shader> m_texture_shader;
  pain::OrthoCameraEntity *m_orthocamera;
  std::shared_ptr<pain::Texture> m_texture;
  MainMap m_mainMap;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pain::Scene *scene = new MainScene();

    pain::OrthoCameraEntity *pCamera =
        // new pain::OrthoCameraEntity(scene, (float)w / h, 40.0f);
        new pain::OrthoCameraEntity(scene, (float)w / h, 10.0f);
    ((MainScene *)scene)->init(pCamera);
    pushScene("main", scene);
    attachScene("main");
  }

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain";
  const int width = 800;
  const int height = 600;
  return new Sandbox(title, width, height);
}

#ifdef _WIN64
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   INT nCmdShow)
{
  pain::Pain::initiate();
  pain::Pain::runApplication(pain::CreateApplication());
  return 0;
}
#else
int main(int argc, char *argv[])
{
  pain::Pain::initiate();
  pain::Pain::runApplication(pain::CreateApplication());
  return 0;
}
#endif
