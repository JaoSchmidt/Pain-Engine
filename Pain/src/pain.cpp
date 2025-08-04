#include "pain.h"
#include <glad/gl.h>

namespace pain
{

void Pain::initiate()
{
  pain::logWrapper::Init();
  PLOG_T("Initialized Log!");
}

class SettingsScene : public pain::Scene
{
  SettingsScene(sol::state &luaState) : pain::Scene(luaState) {}
  void init(pain::Application &app, Scene *scene, float aspectRatio, float zoom)
  {
    m_orthocamera = new pain::OrthoCamera(scene, aspectRatio, zoom);
    pain::Renderer2d::init(*m_orthocamera);
  }
};

void Pain::runApplicationWithSettings(Application *app)
{
  LOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 400;
  const int height = 400;
  Application *settingsApp = new Application(title, width, height);

  Scene *scene = new SettingsScene(app->getLuaState());
  ((SettingsScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

  settingsApp->pushScene("settingsMain", scene);
  settingsApp->attachScene("settingsMain");

  settingsApp->run();
  delete settingsApp;

  Application *application = app;
  application->run();
  delete application;
}

void Pain::runApplication(Application *app)
{
  Application *application = app;
  application->run();
  delete application;
}
} // namespace pain
