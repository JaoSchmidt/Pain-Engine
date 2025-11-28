#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{
namespace ColReaction
{
// Collection of function for the collision Reaction between to shapes
// OBB = Oriented Bounding Box

// ==========================================================
// AABB = Axis Aligned Bounding Box
// ----------------------------------------------------------

// Solid collision between two AABB shapes
void solidCollisionAABB(const glm::vec2 &center1,   //
                        glm::vec2 &vel1,            //
                        const glm::vec2 &halfSize1, //
                        const glm::vec2 &center2,   //
                        glm::vec2 &vel2,            //
                        const glm::vec2 &halfSize2, //
                        float mass1 = 1.f, float mass2 = 1.f);
// Solid collision between AABB shape, and other AABB static shape
void solidCollisionAABBxStaticAABB(glm::vec2 &center1, //
                                   glm::vec2 &vel1,    //
                                   const glm::vec2 &center2);

// ==========================================================
// OBB = Oriented Bounding Box
// ----------------------------------------------------------

// Solid collision between two OBB shapes
void solidCollisionOBB(glm::vec2 &vel1,                  //
                       glm::vec2 &vel2,                  //
                       const glm::vec2 &collisionNormal, //
                       float mass1 = 1.f, float mass2 = 1.f);
// Solid collision between a static OBB and a OBB shapes
void solidCollisionStaticOBBxOBB(glm::vec2 &vel1,
                                 const glm::vec2 &collisionNormal);

// ==========================================================
// CIRCLE = a circle, duh
// ----------------------------------------------------------

// Solid collision between two circle shapes
void solidCollisionCIRCLE(const glm::vec2 &center1, //
                          glm::vec2 &vel1,          //
                          const glm::vec2 &center2, //
                          glm::vec2 &vel2,          //
                          float mass1 = 1.f, float mass2 = 1.f);
// Solid collision between a static circle and a circle shapes
void solidCollisionStaticCIRCLExCIRCLE(const glm::vec2 &center1, //
                                       glm::vec2 &vel1,          //
                                       const glm::vec2 &center2);

// ==========================================================
// Mixed stuff, but only squared
// ----------------------------------------------------------

// Solid collision between OBB and AABB shapes
void solidCollisionAABBxOBB(glm::vec2 &vel1,                  //
                            glm::vec2 &vel2,                  //
                            const glm::vec2 &collisionNormal, //
                            float mass1, float mass2);

// Solid collision between OBB and AABB shapes
void solidCollisionStaticAABBxOBB(glm::vec2 &vel2, //
                                  const glm::vec2 &collisionNormal);

// Solid collision between OBB and AABB shapes
void solidCollisionAABBxStaticOBB(glm::vec2 &vel1, //
                                  const glm::vec2 &collisionNormal);

// ==========================================================
// Mixed stuff, but involves a circle
// ----------------------------------------------------------

// Solid collision between OBB and Circle shape
void solidCollisionOBBxCIRCLE(const glm::vec2 &center1,           //
                              glm::vec2 &vel1,                    //
                              const glm::vec2 &halfSize1,         //
                              const glm::mat2 &normalizedRotMat1, //
                              const glm::vec2 &center2,           //
                              glm::vec2 &vel2,                    //
                              float radius2,                      //
                              float mass1 = 1.f, float mass2 = 1.f);

// Solid collision between AABB and Circle shape
void solidCollisionStaticAABBxCIRCLE(glm::vec2 &center1,          //
                                     glm::vec2 &vel1,             //
                                     float radius1,               //
                                     const glm::vec2 &boxcenter2, //
                                     const glm::vec2 &halfSize2);

// Solid collision between a OBB and static Circle shape
void solidCollisionOBBxStaticCIRCLE(const glm::vec2 &center1,           //
                                    glm::vec2 &vel1,                    //
                                    const glm::mat2 &normalizedRotMat1, //
                                    const glm::vec2 &center2,           //
                                    float radius2);

// Solid collision between a static OBB and Circle shape
void solidCollisionStaticOBBxCIRCLE(const glm::vec2 &center1,           //
                                    const glm::mat2 &normalizedRotMat1, //
                                    const glm::vec2 &center2,           //
                                    glm::vec2 &vel2,                    //
                                    float radius2);

} // namespace ColReaction
} // namespace pain
