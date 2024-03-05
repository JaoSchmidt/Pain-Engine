#include <pain.h>

class MainLayer : public pain::Layer {
  MainLayer() : Layer() {}
  void OnUpdate() override { LOG_I("Update on the client"); }
  void OnEvent() override { LOG_I("Event on the client"); }
};

class Sandbox : public pain::Application {
public:
  Sandbox() {}

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication() {
  LOG_I("Creating app");
  return new Sandbox();
}
