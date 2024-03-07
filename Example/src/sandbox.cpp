#include <pain.h>

class MainLayer : public pain::Layer
{
public:
  MainLayer() : Layer("main") {}
  void onUpdate() override { LOG_I("Update on the client"); }
  void onEvent() override { LOG_I("Event on the client"); }
  void onDetach() override { LOG_I("Layer attached to the stack"); }
  void onAttach() override { LOG_I("Layer dettached from the stack"); }
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pushLayer(new MainLayer());
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
