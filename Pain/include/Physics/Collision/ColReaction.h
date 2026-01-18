/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{
namespace ColReaction
{

// Collection of function for the collision Reaction between to shapes
// OBB = Oriented Bounding Box

// TODO: After some time I saw that every solid collision is basically the same,
// however I decided to wait to make capsules and polygons just to be sure
// before create a single, unique function
void solidCollisionDynamic(                           //
    glm::vec2 &center1,                               //
    glm::vec2 &vel1,                                  //
    glm::vec2 &center2,                               //
    glm::vec2 &vel2,                                  //
    const glm::vec2 &collisionNormal,                 //
    const float penetration,                          //
    const float mass1 = 1.f, const float mass2 = 1.f, //
    const float restitution = 1.f                     //
);
void solidCollisionStatic(            //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

} // namespace ColReaction
} // namespace pain
