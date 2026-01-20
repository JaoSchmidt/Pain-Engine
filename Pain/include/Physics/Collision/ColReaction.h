/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "glm/ext/vector_float2.hpp"

/**
 * Collision reaction utilities used to resolve detected intersections.
 *
 * This module applies positional correction and impulse-based velocity
 * resolution after a collision has been detected by the collision
 * detection layer.
 *
 * The functions here do not perform collision detection. They assume a
 * valid collision normal and penetration depth are already provided.
 *
 * OBB = Oriented Bounding Box.
 */
namespace pain::ColReaction
{

/**
 * Resolves a solid collision between two dynamic bodies.
 *
 * Applies positional correction using penetration depth and resolves
 * velocities using impulse-based collision response.
 *
 * Mass and restitution parameters control momentum exchange and
 * elasticity of the collision.
 *
 * @param center1 Center position of the first body (modified).
 * @param vel1 Velocity of the first body (modified).
 * @param center2 Center position of the second body (modified).
 * @param vel2 Velocity of the second body (modified).
 * @param collisionNormal Collision normal between the two bodies.
 * @param penetration Penetration depth of the intersection.
 * @param mass1 Mass of the first body.
 * @param mass2 Mass of the second body.
 * @param restitution Elasticity coefficient in range [0, 1].
 */
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

/**
 * Resolves a solid collision between a dynamic body and a static body.
 *
 * Applies positional correction and impulse-based velocity resolution
 * only to the dynamic object.
 *
 * The static body is assumed to have infinite mass and is not modified.
 *
 * @param center1 Center position of the dynamic body (modified).
 * @param vel1 Velocity of the dynamic body (modified).
 * @param center2 Center position of the static body.
 * @param collisionNormal Collision normal pointing from dynamic toward static.
 * @param penetration Penetration depth of the intersection.
 * @param restitution Elasticity coefficient in range [0, 1].
 */
void solidCollisionStatic(            //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

} // namespace pain::ColReaction
