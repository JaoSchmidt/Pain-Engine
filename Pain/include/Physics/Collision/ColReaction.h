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

// ==========================================================
// AABB = Axis Aligned Bounding Box
// ----------------------------------------------------------

// Solid collision between two AABB shapes
void solidCollisionAABB(                              //
    glm::vec2 &center1,                               //
    glm::vec2 &vel1,                                  //
    glm::vec2 &center2,                               //
    glm::vec2 &vel2,                                  //
    const glm::vec2 &collisionNormal,                 //
    const float penetration,                          //
    const float mass1 = 1.f, const float mass2 = 1.f, //
    const float restitution = 1.f                     //
);
// Solid collision between AABB shape, and other AABB static shape
void solidCollisionAABBxStaticAABB(   //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// ==========================================================
// OBB = Oriented Bounding Box
// ----------------------------------------------------------

// Solid collision between two OBB shapes
void solidCollisionOBB(                               //
    glm::vec2 &center1,                               //
    glm::vec2 &vel1,                                  //
    glm::vec2 &center2,                               //
    glm::vec2 &vel2,                                  //
    const glm::vec2 &collisionNormal,                 //
    const float penetration,                          //
    const float mass1 = 1.f, const float mass2 = 1.f, //
    const float restitution = 1.f                     //
);
// Solid collision between a static OBB and a OBB shapes
void solidCollisionStaticOBBxOBB(     //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// ==========================================================
// CIRCLE = a circle, duh
// ----------------------------------------------------------

// Solid collision between two circle shapes
void solidCollisionCIRCLE(                            //
    glm::vec2 &center1,                               //
    glm::vec2 &vel1,                                  //
    glm::vec2 &center2,                               //
    glm::vec2 &vel2,                                  //
    const glm::vec2 &collisionNormal,                 //
    const float penetration,                          //
    const float mass1 = 1.f, const float mass2 = 1.f, //
    const float restitution = 1.f                     //
);
// Solid collision between a static circle and a circle shapes
void solidCollisionStaticCIRCLExCIRCLE( //
    glm::vec2 &center1,                 //
    glm::vec2 &vel1,                    //
    const glm::vec2 &center2,           //
    const glm::vec2 &collisionNormal,   //
    const float penetration,            //
    const float restitution = 1.f       //
);

// ==========================================================
// Mixed stuff, but only squared
// ----------------------------------------------------------

// Solid collision between OBB and AABB shapes
void solidCollisionAABBxOBB(                          //
    glm::vec2 &center1,                               //
    glm::vec2 &vel1,                                  //
    glm::vec2 &center2,                               //
    glm::vec2 &vel2,                                  //
    const glm::vec2 &collisionNormal,                 //
    const float penetration,                          //
    const float mass1 = 1.f, const float mass2 = 1.f, //
    const float restitution = 1.f                     //
);

// Solid collision between OBB and AABB shapes
void solidCollisionStaticAABBxOBB(    //
    const glm::vec2 &center1,         //
    glm::vec2 &center2,               //
    glm::vec2 &vel2,                  //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// Solid collision between OBB and AABB shapes
void solidCollisionAABBxStaticOBB(    //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// ==========================================================
// Mixed stuff, but involves a circle
// ----------------------------------------------------------

// Solid collision between OBB and Circle shape
void solidCollisionOBBxCIRCLE(            //
    glm::vec2 &center1,                   //
    glm::vec2 &vel1,                      //
    glm::vec2 &center2,                   //
    glm::vec2 &vel2,                      //
    const glm::vec2 &collisionNormal,     //
    const float penetration,              //
    float mass1 = 1.f, float mass2 = 1.f, //
    const float restitution = 1.f         //
);

// Solid collision between AABB and Circle shape
void solidCollisionStaticAABBxCIRCLE( //
    const glm::vec2 &center1,         //
    glm::vec2 &center2,               //
    glm::vec2 &vel2,                  //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// Solid collision between a OBB and static Circle shape
void solidCollisionOBBxStaticCIRCLE(  //
    glm::vec2 &center1,               //
    glm::vec2 &vel1,                  //
    const glm::vec2 &center2,         //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

// Solid collision between a static OBB and Circle shape
void solidCollisionStaticOBBxCIRCLE(  //
    const glm::vec2 &center1,         //
    glm::vec2 &center2,               //
    glm::vec2 &vel2,                  //
    const glm::vec2 &collisionNormal, //
    const float penetration,          //
    const float restitution = 1.f     //
);

} // namespace ColReaction
} // namespace pain
