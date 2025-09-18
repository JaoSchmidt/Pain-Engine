#pragma once
#include "ECS/Components/NativeScript.h"

/* What is this?
 * Remember the idea that pure ECS would just be parallels? that is one
 * exception to this rule, altough this can be used later with a tag system
 * TODO: The excercie today is: can I assume that this exception will only arise
 * in the Script system?
 *
 * If I can: then perhaps a tagged/marker system is
 * best. Or even a simple switch case for each. i.e. add something like tags
 * (UIOnly, Hidden, etc).
 *
 * If I cannot: I will then need to think more.
 */
namespace pain
{
struct ImGuiComponent : NativeScriptComponent {
};
} // namespace pain
