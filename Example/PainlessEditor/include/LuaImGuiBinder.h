#include "CustomPanel.h"
#include <sol/sol.hpp>
namespace painless::luabinder
{
void bindImGui(sol::state &lua, CustomEditor &editor);
void unbindCustomPanels(sol::state &lua);
} // namespace painless::luabinder
