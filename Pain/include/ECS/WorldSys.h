#include "CoreRender/RenderSys.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Scene.h"
#include "GUI/ImGuiSys.h"
#include "Physics/Collision/CollisionNaiveSys.h"
#include "Physics/KinematicsSys.h"
#include "Scripting/LuaScriptSys.h"
#include "Scripting/NativeSystem.h"

namespace pain
{
class WorldSystems
{
public:
  void updateSystems(double deltaTime);
  void updateSystems(const SDL_Event &event);
  void renderSystems(Renderer2d &renderer, bool isMinimized,
                     double currentTime);
  WorldSystems(reg::ArcheRegistry<ComponentManager> &m_registry,
               float collisionGridSize, void *context, SDL_Window *window);

private:
  reg::ArcheRegistry<ComponentManager> &m_registry;
  Systems::Render m_renderSystem = {m_registry};
  Systems::Kinematics m_kinematicsSystem = {m_registry};
  Systems::NativeScript m_nativeScriptSystem = {m_registry};
  Systems::ImGuiSys m_imGuiSystem;
  Systems::LuaScript m_luaSystem = {m_registry};
  Systems::CollisionSystem m_collisionSystem;
};

} // namespace pain
