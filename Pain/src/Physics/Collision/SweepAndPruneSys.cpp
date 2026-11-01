/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// SweepAndPruneSys.cpp
#include "Physics/Collision/SweepAndPruneSys.h"

#include "ECS/Archetype/ArcheRegistry.h"
#include "Misc/Events.h"
#include "Physics/Collision/ColDetection.h"
#include "Physics/Collision/ColReaction.h"
#include "Physics/Collision/Collider.h"
#include "Physics/MovementComponent.h"

#include <unordered_map>

namespace pain
{
namespace Systems
{
namespace detail
{

void sortSAP(std::vector<EndPoint> &vec, std::vector<EndPointKey> &keyVec,
             bool isX)
{
  std::sort(vec.begin(), vec.end(), [](const EndPoint &a, const EndPoint &b) {
    if (a.valueOnAxis != b.valueOnAxis)
      return a.valueOnAxis < b.valueOnAxis;
    // NOTE: remember true > false in c++
    return a.isMin > b.isMin;
  });

  // fill the EndPointKey with the correct indexes
  for (size_t i = 0; i < vec.size(); ++i) {
    if (isX && vec[i].isMin) {
      keyVec[vec[i].key].index_minX = i;
    } else if (isX && !vec[i].isMin) {
      keyVec[vec[i].key].index_maxX = i;
    } else if (!isX && vec[i].isMin) {
      keyVec[vec[i].key].index_minY = i;
    } else {
      keyVec[vec[i].key].index_maxY = i;
    }
  }
};

void insertionSortEndpoint(std::vector<EndPoint> &endpoints,
                           std::vector<EndPointKey> &keys, size_t index,
                           bool isXAxis)
{
  const size_t n = endpoints.size();
  if (n < 2 || index >= n)
    return;

  const float value = endpoints[index].valueOnAxis;
  const bool isMin = endpoints[index].isMin;

  // Move left if value decreased
  while (index > 0 && (endpoints[index - 1].valueOnAxis > value ||
                       (endpoints[index - 1].valueOnAxis == value &&
                        endpoints[index - 1].isMin > isMin))) {

    std::swap(endpoints[index], endpoints[index - 1]);

    // Update the key indices for both swapped elements
    size_t key1 = endpoints[index].key;
    size_t key2 = endpoints[index - 1].key;

    if (isXAxis) {
      if (endpoints[index].isMin)
        keys[key1].index_minX = index;
      else
        keys[key1].index_maxX = index;

      if (endpoints[index - 1].isMin)
        keys[key2].index_minX = index - 1;
      else
        keys[key2].index_maxX = index - 1;
    } else {
      if (endpoints[index].isMin)
        keys[key1].index_minY = index;
      else
        keys[key1].index_maxY = index;

      if (endpoints[index - 1].isMin)
        keys[key2].index_minY = index - 1;
      else
        keys[key2].index_maxY = index - 1;
    }

    index--;
  }

  // Move right if value increased
  while (index < n - 1 && (endpoints[index + 1].valueOnAxis < value ||
                           (endpoints[index + 1].valueOnAxis == value &&
                            endpoints[index + 1].isMin < isMin))) {

    std::swap(endpoints[index], endpoints[index + 1]);

    // Update the key indices for both swapped elements
    size_t key1 = endpoints[index].key;
    size_t key2 = endpoints[index + 1].key;

    if (isXAxis) {
      if (endpoints[index].isMin)
        keys[key1].index_minX = index;
      else
        keys[key1].index_maxX = index;

      if (endpoints[index + 1].isMin)
        keys[key2].index_minX = index + 1;
      else
        keys[key2].index_maxX = index + 1;
    } else {
      if (endpoints[index].isMin)
        keys[key1].index_minY = index;
      else
        keys[key1].index_maxY = index;

      if (endpoints[index + 1].isMin)
        keys[key2].index_minY = index + 1;
      else
        keys[key2].index_maxY = index + 1;
    }

    index++;
  }
}

size_t insertEndPoint(reg::Entity entity, std::vector<EndPoint> &vecX,
                      std::vector<EndPoint> &vecY,
                      std::vector<EndPointKey> &vecKeys,
                      const Transform2dComponent &transComp,
                      SAPCollider &colComp)
{
  glm::vec2 center = transComp.m_position + colComp.m_offset;
  glm::vec2 bottomLeft;
  glm::vec2 topRight;

  // compute AABB
  std::visit(
      [&](auto &&shape1) {
        using T1 = std::decay_t<decltype(shape1)>;
        if constexpr (std::is_same_v<T1, AABBShape>) {
          bottomLeft = center - shape1.halfSize;
          topRight = center + shape1.halfSize;
        } else if constexpr (std::is_same_v<T1, CircleShape>) {
          const float diameter = 2.F * shape1.radius;
          bottomLeft = center - glm::vec2(diameter);
          topRight = center + glm::vec2(diameter);
        } else {
          PLOG_E("ERROR: could not find infer shape of collider");
        }
      },
      colComp.m_shape);

  // Step 2: Create sorted endpoint lists
  const size_t key_index = vecKeys.size();
  vecX.emplace_back(key_index, bottomLeft.x, true);
  vecX.emplace_back(key_index, topRight.x, false);
  vecY.emplace_back(key_index, bottomLeft.y, true);
  vecY.emplace_back(key_index, topRight.y, false);
  // NOTE: the indexes don't matter if you are going to use std::sort
  // They do matter for introducing colliders later dynamically
  vecKeys.emplace_back(entity,          //
                       vecX.size() - 2, //
                       vecX.size() - 1, //
                       vecY.size() - 2, //
                       vecY.size() - 1);
  colComp.m_index = static_cast<int>(key_index);
  return key_index;
}

void sortAfterInsertion(
    std::vector<EndPoint> &endPointsX,      //
    std::vector<EndPoint> &endPointsY,      //
    std::vector<EndPointKey> &endPointKeys, //
    std::vector<EndPoint> &staticEndPointsX,      //
    std::vector<EndPoint> &staticEndPointsY,      //
    std::vector<EndPointKey> &staticEndPointKeys, //
    bool &firstTime)
{
  sortSAP(endPointsX, endPointKeys, true);
  sortSAP(endPointsY, endPointKeys, false);
  sortSAP(staticEndPointsX, staticEndPointKeys, true);
  sortSAP(staticEndPointsY, staticEndPointKeys, false);
  firstTime = false;
}

void _implOnUpdate(
    DeltaTime deltaTime,
    std::vector<reg::ChunkView<Transform2dComponent, SAPCollider,
                               Movement2dComponent>>
        chunksDynamic,
    std::vector<reg::ChunkView<Transform2dComponent, SAPCollider>>
        chunksStatic,
    std::vector<EndPoint> &endPointsX,      //
    std::vector<EndPoint> &endPointsY,      //
    std::vector<EndPointKey> &endPointKeys, //
    std::vector<EndPoint> &staticEndPointsX,      //
    std::vector<EndPoint> &staticEndPointsY,      //
    std::vector<EndPointKey> &staticEndPointKeys, //
    std::vector<long> &activeList,                //
    bool &firstTime,                              //
    reg::EventDispatcher &eventDispatcher)
{
  UNUSED(deltaTime)

  // Lookup tables so the narrow phase can recover components without needing a
  // CM-dependent query.
  std::unordered_map<reg::Entity, std::tuple<Transform2dComponent *, SAPCollider *,
                                             Movement2dComponent *>>
      dynamicLookup;
  for (auto &chunk : chunksDynamic) {
    auto *t = std::get<0>(chunk.arrays);
    auto *c = std::get<1>(chunk.arrays);
    auto *m = std::get<2>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i)
      dynamicLookup.emplace(chunk.entities[i],
                            std::make_tuple(&t[i], &c[i], &m[i]));
  }
  std::unordered_map<reg::Entity,
                     std::pair<Transform2dComponent *, SAPCollider *>>
      staticLookup;
  for (auto &chunk : chunksStatic) {
    auto *t = std::get<0>(chunk.arrays);
    auto *c = std::get<1>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i)
      staticLookup.emplace(chunk.entities[i], std::make_pair(&t[i], &c[i]));
  }

  // Step 0: In case its the first time, use faster sorter
  if (firstTime) {
    for (auto &chunk : chunksDynamic) {
      auto *t = std::get<0>(chunk.arrays);
      auto *c = std::get<1>(chunk.arrays);
      auto &e = chunk.entities;
      for (size_t i = 0; i < chunk.count; ++i) {
        insertEndPoint(e[i], endPointsX, endPointsY, endPointKeys, t[i], c[i]);
      }
    }
    for (auto &chunk : chunksStatic) {
      auto *t = std::get<0>(chunk.arrays);
      auto *c = std::get<1>(chunk.arrays);
      auto &e = chunk.entities;
      for (size_t i = 0; i < chunk.count; ++i) {
        insertEndPoint(e[i], staticEndPointsX, staticEndPointsY,
                       staticEndPointKeys, t[i], c[i]);
      }
    }
    sortSAP(endPointsX, endPointKeys, true);
    sortSAP(endPointsY, endPointKeys, false);
    sortSAP(staticEndPointsX, staticEndPointKeys, true);
    sortSAP(staticEndPointsY, staticEndPointKeys, false);
    firstTime = false;
  }

  // Step 1: Update all moving endpoints with new data from the components
  for (auto &chunk : chunksDynamic) {
    auto *t = std::get<0>(chunk.arrays);
    auto *c = std::get<1>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {

      SAPCollider &collider = c[i];
      EndPointKey &proxy =
          collider.m_index >= 0
              ? endPointKeys[static_cast<unsigned>(collider.m_index)]
              : endPointKeys[insertEndPoint(chunk.entities[i], endPointsX,
                                            endPointsY, endPointKeys, t[i],
                                            c[i])];

      glm::vec2 center = t[i].m_position + collider.m_offset;
      glm::vec2 bottomLeft;
      glm::vec2 topRight;

      std::visit(
          [&](auto &&shape) {
            using T = std::decay_t<decltype(shape)>;

            if constexpr (std::is_same_v<T, AABBShape>) {
              bottomLeft = center - shape.halfSize;
              topRight = center + shape.halfSize;
            } else if constexpr (std::is_same_v<T, CircleShape>) {
              float d = 2.F * shape.radius;
              bottomLeft = center - glm::vec2(d);
              topRight = center + glm::vec2(d);
            } else {
              PLOG_E("ERROR: could not infer shape of collider");
            }
          },
          collider.m_shape);

      // ----------------------------------------------------------
      // Update X axis endpoints
      // ----------------------------------------------------------
      endPointsX[proxy.index_minX].valueOnAxis = bottomLeft.x;
      insertionSortEndpoint(endPointsX, endPointKeys, proxy.index_minX, true);

      endPointsX[proxy.index_maxX].valueOnAxis = topRight.x;
      insertionSortEndpoint(endPointsX, endPointKeys, proxy.index_maxX, true);

      // ----------------------------------------------------------
      // Update Y axis endpoints
      // ----------------------------------------------------------
      endPointsY[proxy.index_minY].valueOnAxis = bottomLeft.y;
      insertionSortEndpoint(endPointsY, endPointKeys, proxy.index_minY, false);

      endPointsY[proxy.index_maxY].valueOnAxis = topRight.y;
      insertionSortEndpoint(endPointsY, endPointKeys, proxy.index_maxY, false);
    }
  }
  // --------------------------------------------------------------------------
  // Step 3: Sweep and build active list to find overlapping pairs
  // --------------------------------------------------------------------------
  std::vector<std::pair<size_t, long>> potential_pairs;
  potential_pairs.reserve(endPointKeys.size());

  size_t staticIdx = 0;
  size_t dynIdx = 0;
  const size_t dynIdxMax = endPointsX.size();
  const size_t staticIdxMax = staticEndPointsX.size();
  const EndPoint *staticEndPoint = nullptr;
  const EndPoint *dynEndPoint = nullptr;

  while (dynIdx < dynIdxMax || staticIdx < staticIdxMax) {
    // declare both endpoints
    dynEndPoint = (dynIdx < dynIdxMax) ? &endPointsX[dynIdx] : nullptr;
    staticEndPoint =
        (staticIdx < staticIdxMax) ? &staticEndPointsX[staticIdx] : nullptr;

    bool processDynamic = false; // assume static process is valid (default)
    if (dynEndPoint && staticEndPoint) { // both process can be valid
      processDynamic = (dynEndPoint->valueOnAxis < staticEndPoint->valueOnAxis);
      // ||
      //   (dynEndPoint->valueOnAxis == staticEndPoint->valueOnAxis &&
      //    dynEndPoint->isMin < staticEndPoint->isMin);
    } else if (dynEndPoint) {
      processDynamic = true;
    }

    if (processDynamic) {

      // --- Dynamic processing ---
      if (dynEndPoint->isMin) {
        // add to active list and potential_pairs
        for (long active_object : activeList) {
          // some min x is overlaping with an active overlap, store it
          potential_pairs.emplace_back(dynEndPoint->key, active_object);
        }
        // activate the new overlap as well
        activeList.push_back(static_cast<long>(dynEndPoint->key));

      } else {
        auto it = std::find(activeList.begin(), activeList.end(),
                            dynEndPoint->key);
        if (it != activeList.end()) {
          activeList.erase(it);
        } else {
          PLOG_E("could not delete dynamic {}", dynEndPoint->key);
        }
      }
      dynIdx++;
    } else {
      // --- Static processing ---
      if (staticEndPoint->isMin) {
        // HACK: set MSB to 1, making the value negative. I'm essentially using
        // the MSB to differentiate static and dynamic keys
        activeList.push_back(
            staticMSB(staticEndPoint->key)); // activate the new overlap as well
        // add to active list and potential_pairs
        for (long active_object : activeList) {
          // some min x is overlaping with an active overlap, store it
          if (active_object >= 0) // only store (dyn,static) or (dyn,dyn)
            potential_pairs.emplace_back(active_object,
                                         staticMSB(staticEndPoint->key));
        }
      } else {
        // remove the overlap from the active objects
        // TODO: if necessary, this search can be removed by "simply" caching
        // the active list index inside the dynEndPoint
        auto it = std::find(activeList.begin(), activeList.end(),
                            staticMSB(staticEndPoint->key));
        if (it != activeList.end()) {
          activeList.erase(it);
        } else {
          PLOG_E("could not delete static key {}", staticEndPoint->key);
        }
      }
      staticIdx++;
    }
  }

  if (activeList.size() > 0) {
    PLOG_W("Warning: Active collision list still has endpoints keys inside");
    activeList.clear();
  }

  // --------------------------------------------------------------------------
  // Step 4: Get the indexes from potential_pairs and check if those entities
  // also check for Y
  // --------------------------------------------------------------------------
  std::vector<std::pair<size_t, long>> validPairs;
  for (const std::pair<size_t, long> &pair : potential_pairs) {
    EndPointKey entityProxy1 = endPointKeys[pair.first];
    const float minY1 = endPointsY[entityProxy1.index_minY].valueOnAxis;
    const float maxY1 = endPointsY[entityProxy1.index_maxY].valueOnAxis;

    float minY2;
    float maxY2;
    // clang-format off
    if (pair.second < 0) { // indicates MSB = 1, meaning it is  static
      EndPointKey &entityProxy2 = staticEndPointKeys[clearMSB(pair.second)];
      minY2 = staticEndPointsY[entityProxy2.index_minY].valueOnAxis;
      maxY2 = staticEndPointsY[entityProxy2.index_maxY].valueOnAxis;
    }else{
      EndPointKey &entityProxy2 = endPointKeys[static_cast<size_t>(pair.second)];
      minY2 = endPointsY[entityProxy2.index_minY].valueOnAxis;
      maxY2 = endPointsY[entityProxy2.index_maxY].valueOnAxis;
    }
    // clang-format on

    if (minY1 < maxY2 && maxY1 > minY2) {
      validPairs.push_back(pair); // still has MSB here
    }
  }
  // --------------------------------------------------------------------------
  // Step 5: Narrow phase from valid pairs
  // --------------------------------------------------------------------------
  for (const std::pair<size_t, long> &pair : validPairs) {

    // Dynamic vs Dynamic narrow collision
    if (pair.second >= 0) {
      const reg::Entity entity1 = endPointKeys[pair.first].entity;
      const reg::Entity entity2 =
          endPointKeys[static_cast<size_t>(pair.second)].entity;
      auto &dyn1 = dynamicLookup.at(entity1);
      auto &dyn2 = dynamicLookup.at(entity2);
      Transform2dComponent &tIt_i = *std::get<0>(dyn1);
      SAPCollider &cIt_i = *std::get<1>(dyn1);
      Movement2dComponent &mIt_i = *std::get<2>(dyn1);
      Transform2dComponent &tIt_j = *std::get<0>(dyn2);
      SAPCollider &cIt_j = *std::get<1>(dyn2);
      Movement2dComponent &mIt_j = *std::get<2>(dyn2);
      glm::vec2 center1 = tIt_i.m_position + cIt_i.m_offset;
      glm::vec2 center2 = tIt_j.m_position + cIt_j.m_offset;
      ColDet::Result collisionHappened = {false};

      // -------------------------------------------------------------
      // Collision detection on movable vs movable objects
      // -------------------------------------------------------------
      std::visit(
          [&](auto &&shape1, auto &&shape2) {
            using T1 = std::decay_t<decltype(shape1)>;
            using T2 = std::decay_t<decltype(shape2)>;
            if constexpr (std::is_same_v<T1, AABBShape> &&
                          std::is_same_v<T2, AABBShape>) {
              collisionHappened = ColDet::checkAABBCollision(
                  center1, shape1.halfSize, center2, shape2.halfSize);
            } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                 std::is_same_v<T2, CircleShape>) {
              collisionHappened = ColDet::checkCircleCollision(
                  center1, shape1.radius, center2, shape2.radius);
            } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                 std::is_same_v<T2, AABBShape>) {
              collisionHappened = ColDet::checkAABBCollisionCircle(
                  center2, shape2.halfSize, center1, shape1.radius);
            } else if constexpr (std::is_same_v<T1, AABBShape> &&
                                 std::is_same_v<T2, CircleShape>) {
              collisionHappened = ColDet::checkAABBCollisionCircle(
                  center1, shape1.halfSize, center2, shape2.radius);
            } else {
              PLOG_E("ERROR: could not find infer shape of collider");
            }
          },
          cIt_i.m_shape, cIt_j.m_shape);

      // --- Perform Collision Reaction on Entities with Collision Scripts
      if (collisionHappened.isDetected) {
        if (cIt_i.m_isTrigger || cIt_j.m_isTrigger) {
          eventDispatcher.enqueue<CollisionEvent>(
              {entity1, entity2, collisionHappened.normal,
               collisionHappened.penetration});
        } else {
          ColReaction::solidCollisionDynamic( //
              tIt_i.m_position,               //
              mIt_i.m_velocity,               //
              tIt_j.m_position,               //
              mIt_j.m_velocity, collisionHappened.normal,
              collisionHappened.penetration);
        }
      }
    } else {
      const reg::Entity entity1 = endPointKeys[pair.first].entity;
      const reg::Entity entity2 =
          staticEndPointKeys[clearMSB(pair.second)].entity;
      auto &dyn1 = dynamicLookup.at(entity1);
      auto &stat2 = staticLookup.at(entity2);
      Transform2dComponent &tIt_i = *std::get<0>(dyn1);
      SAPCollider &cIt_i = *std::get<1>(dyn1);
      Movement2dComponent &mIt_i = *std::get<2>(dyn1);
      Transform2dComponent &tIt_j = *stat2.first;
      SAPCollider &cIt_j = *stat2.second;
      glm::vec2 center1 = tIt_i.m_position + cIt_i.m_offset;
      glm::vec2 center2 = tIt_j.m_position + cIt_j.m_offset;
      ColDet::Result collisionHappened = {false};

      // -------------------------------------------------------------
      // Collision detection on movable vs static objects
      // -------------------------------------------------------------
      std::visit(
          [&](auto &&shape1, auto &&shape2) {
            using T1 = std::decay_t<decltype(shape1)>;
            using T2 = std::decay_t<decltype(shape2)>;
            if constexpr (std::is_same_v<T1, AABBShape> &&
                          std::is_same_v<T2, AABBShape>) {
              collisionHappened = ColDet::checkAABBCollision(
                  center1, shape1.halfSize, center2, shape2.halfSize);
            } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                 std::is_same_v<T2, CircleShape>) {
              collisionHappened = ColDet::checkCircleCollision(
                  center1, shape1.radius, center2, shape2.radius);
            } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                 std::is_same_v<T2, AABBShape>) {
              collisionHappened = ColDet::checkAABBCollisionCircle(
                  center2, shape2.halfSize, center1, shape1.radius);
            } else if constexpr (std::is_same_v<T1, AABBShape> &&
                                 std::is_same_v<T2, CircleShape>) {
              collisionHappened = ColDet::checkAABBCollisionCircle(
                  center1, shape1.halfSize, center2, shape2.radius);
            } else {
              PLOG_E("ERROR: could not find infer shape of collider");
            }
          },
          cIt_i.m_shape, cIt_j.m_shape);

      // --- Perform Collision Reaction on Entities with Collision Scripts
      if (collisionHappened.isDetected) {
        if (cIt_i.m_isTrigger || cIt_j.m_isTrigger) {
          eventDispatcher.enqueue<CollisionEvent>(
              {entity1, entity2, collisionHappened.normal,
               collisionHappened.penetration});
        } else {
          ColReaction::solidCollisionStatic( //
              tIt_i.m_position,              //
              mIt_i.m_velocity,              //
              tIt_j.m_position,              //
              collisionHappened.normal, collisionHappened.penetration);
        }
      }
    }
  }
}

} // namespace detail
} // namespace Systems
} // namespace pain
