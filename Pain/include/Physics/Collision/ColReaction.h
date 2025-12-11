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
