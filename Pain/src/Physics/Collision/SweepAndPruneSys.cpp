// SweepAndPruneSys.cpp
#include "Physics/Collision/SweepAndPruneSys.h"

#include "ECS/Components/Movement.h"
#include "Physics/Collision/ColDetection.h"
#include "Physics/Collision/ColReaction.h"
#include "Physics/Collision/Collider.h"

namespace pain
{
namespace Systems
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

void insertEndPoint(std::vector<EndPoint> &vecX, std::vector<EndPoint> &vecY,
                    std::vector<EndPointKey> &vecKeys,
                    const Transform2dComponent &transComp, SAPCollider &colComp)
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
          const float diameter = 2.f * shape1.radius;
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
  vecKeys.emplace_back(colComp.m_entity, 0, 0, 0, 0);
  colComp.m_index = key_index;
}

// insert a non moving entity into the static entities array, then sort the
// array
void SweepAndPruneSys::insertStaticEntity(reg::Entity entity,
                                          const Transform2dComponent &tc,
                                          SAPCollider &sc)
{
  UNUSED(entity)
  insertEndPoint(m_staticEndPointsX, m_staticEndPointsY, m_staticEndPointKeys,
                 tc, sc);
}
void SweepAndPruneSys::insertEntity(reg::Entity entity,
                                    const Transform2dComponent &tc,
                                    SAPCollider &sc)
{
  UNUSED(entity)
  insertEndPoint(m_endPointsX, m_endPointsY, m_endPointKeys, tc, sc);
}
void SweepAndPruneSys::sortAfterInsertion()
{
  sortSAP(m_endPointsX, m_endPointKeys, true);
  sortSAP(m_endPointsY, m_endPointKeys, false);
  sortSAP(m_staticEndPointsX, m_staticEndPointKeys, true);
  sortSAP(m_staticEndPointsY, m_staticEndPointKeys, false);
}

void SweepAndPruneSys::onUpdate(DeltaTime deltaTime)
{
  UNUSED(deltaTime)
  // Step 1: Update all moving endpoints with new data from the components
  auto [tIt, cIt, mIt] =
      begin<Transform2dComponent, SAPCollider, Movement2dComponent>();
  auto [tItEnd, cItEnd, mItEnd] =
      end<Transform2dComponent, SAPCollider, Movement2dComponent>();

  // --------------------------------------------------------------------------
  // Step 2: update all `valueOnAxis` from the sorted list with new values from
  // --------------------------------------------------------------------------
  // the components
  for (; tIt != tItEnd; ++tIt, ++cIt, ++mIt) {
    EndPointKey &proxy = m_endPointKeys[cIt->m_index];
    glm::vec2 center = tIt->m_position + cIt->m_offset;
    glm::vec2 bottomLeft;
    glm::vec2 topRight;

    // if (isnan(tIt->m_position.x))
    //   PLOG_E("tIt->m_position.x is Nan, from entity {}, index {}, offset",
    //          cIt->m_entity, cIt->m_index);
    // if (isnan(tIt->m_position.y))
    //   PLOG_E("tIt->m_position = ({}, {})", tIt->m_position.x,
    //          tIt->m_position.y);
    // compute AABB
    std::visit(
        [&](auto &&shape1) {
          using T1 = std::decay_t<decltype(shape1)>;
          if constexpr (std::is_same_v<T1, AABBShape>) {
            bottomLeft = center - shape1.halfSize;
            topRight = center + shape1.halfSize;
          } else if constexpr (std::is_same_v<T1, CircleShape>) {
            const float diameter = 2.f * shape1.radius;
            bottomLeft = center - glm::vec2(diameter);
            topRight = center + glm::vec2(diameter);
          } else {
            PLOG_E("ERROR: could not find infer shape of collider");
          }
        },
        cIt->m_shape);

    m_endPointsX[proxy.index_minX].valueOnAxis = bottomLeft.x;
    insertionSortEndpoint(m_endPointsX, m_endPointKeys, proxy.index_minX, true);

    m_endPointsX[proxy.index_maxX].valueOnAxis = topRight.x;
    insertionSortEndpoint(m_endPointsX, m_endPointKeys, proxy.index_maxX, true);

    // Update Y axis endpoints
    m_endPointsY[proxy.index_minY].valueOnAxis = bottomLeft.y;
    insertionSortEndpoint(m_endPointsY, m_endPointKeys, proxy.index_minY,
                          false);

    m_endPointsY[proxy.index_maxY].valueOnAxis = topRight.y;
    insertionSortEndpoint(m_endPointsY, m_endPointKeys, proxy.index_maxY,
                          false);
  }

  // --------------------------------------------------------------------------
  // Step 3: Sweep and build active list to find overlapping pairs
  // --------------------------------------------------------------------------
  std::vector<std::pair<size_t, long>> potential_pairs;
  potential_pairs.reserve(m_endPointKeys.size());

  size_t staticIdx = 0;
  size_t dynIdx = 0;
  const size_t dynIdxMax = m_endPointsX.size();
  const size_t staticIdxMax = m_staticEndPointsX.size();
  const EndPoint *staticEndPoint = nullptr;
  const EndPoint *dynEndPoint = nullptr;

  while (dynIdx < dynIdxMax || staticIdx < staticIdxMax) {
    // declare both endpoints
    dynEndPoint = (dynIdx < dynIdxMax) ? &m_endPointsX[dynIdx] : nullptr;
    staticEndPoint =
        (staticIdx < staticIdxMax) ? &m_staticEndPointsX[staticIdx] : nullptr;

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
        for (long active_object : m_activeList) {
          // some min x is overlaping with an active overlap, store it
          potential_pairs.emplace_back(dynEndPoint->key, active_object);
        }
        // activate the new overlap as well
        m_activeList.push_back(static_cast<long>(dynEndPoint->key));

      } else {
        // FIX: debug only part, nothing happening here
        // if (dynEndPoint->valueOnAxis > 2.f) {
        //   // check if circle
        //
        //   EndPointKey entityProxy1 = m_endPointKeys[dynEndPoint->key];
        //
        //   const float maxY1 =
        //   m_endPointsY[entityProxy1.index_maxY].valueOnAxis; if (maxY1 > 1.5)
        //   {
        //     PLOG_I("AABB {}, index {}, detected at x = {}",
        //     entityProxy1.entity,
        //            dynEndPoint->key, dynEndPoint->valueOnAxis);
        //   }
        // }

        // remove the overlap from the active objects
        // TODO: if necessary, this search can be removed by "simply" caching
        // the active list index inside the dynEndPoint
        auto it = std::find(m_activeList.begin(), m_activeList.end(),
                            dynEndPoint->key);
        if (it != m_activeList.end()) {
          m_activeList.erase(it);
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
        m_activeList.push_back(
            staticMSB(staticEndPoint->key)); // activate the new overlap as well
        // add to active list and potential_pairs
        for (long active_object : m_activeList) {
          // some min x is overlaping with an active overlap, store it
          if (active_object >= 0) // only store (dyn,static) or (dyn,dyn)
            potential_pairs.emplace_back(active_object,
                                         staticMSB(staticEndPoint->key));
        }
      } else {
        // remove the overlap from the active objects
        // TODO: if necessary, this search can be removed by "simply" caching
        // the active list index inside the dynEndPoint
        auto it = std::find(m_activeList.begin(), m_activeList.end(),
                            staticMSB(staticEndPoint->key));
        if (it != m_activeList.end()) {
          m_activeList.erase(it);
        } else {
          PLOG_E("could not delete static key {}", staticEndPoint->key);
        }
      }
      staticIdx++;
    }
  }

  if (m_activeList.size() > 0) {
    PLOG_W("Warning: Active collision list still has endpoints keys inside");
    m_activeList.clear();
  }

  // --------------------------------------------------------------------------
  // Step 4: Get the indexes from potential_pairs and check if those entities
  // also check for Y
  // --------------------------------------------------------------------------
  std::vector<std::pair<size_t, long>> validPairs;
  for (const std::pair<size_t, long> &pair : potential_pairs) {
    EndPointKey entityProxy1 = m_endPointKeys[pair.first];
    const float minY1 = m_endPointsY[entityProxy1.index_minY].valueOnAxis;
    const float maxY1 = m_endPointsY[entityProxy1.index_maxY].valueOnAxis;

    float minY2;
    float maxY2;
    // clang-format off
    if (pair.second < 0) { // indicates MSB = 1, meaning it is  static
      EndPointKey &entityProxy2 = m_staticEndPointKeys[clearMSB(pair.second)];
      minY2 = m_staticEndPointsY[entityProxy2.index_minY].valueOnAxis;
      maxY2 = m_staticEndPointsY[entityProxy2.index_maxY].valueOnAxis;
    }else{
      EndPointKey &entityProxy2 = m_endPointKeys[static_cast<size_t>(pair.second)];
      minY2 = m_endPointsY[entityProxy2.index_minY].valueOnAxis;
      maxY2 = m_endPointsY[entityProxy2.index_maxY].valueOnAxis;
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
      const reg::Entity entity1 = m_endPointKeys[pair.first].entity;
      const reg::Entity entity2 =
          m_endPointKeys[static_cast<size_t>(pair.second)].entity;
      auto [tIt_i, cIt_i, mIt_i] =
          getComponents<Transform2dComponent, SAPCollider, Movement2dComponent>(
              entity1);
      auto [tIt_j, cIt_j, mIt_j] =
          getComponents<Transform2dComponent, SAPCollider, Movement2dComponent>(
              entity2);
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
          // TODO: perform trigger callback
          PLOG_I("Trigger collision happened despite being WIP");
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
      const reg::Entity entity1 = m_endPointKeys[pair.first].entity;
      const reg::Entity entity2 =
          m_staticEndPointKeys[clearMSB(pair.second)].entity;
      auto [tIt_i, cIt_i, mIt_i] =
          getComponents<Transform2dComponent, SAPCollider, Movement2dComponent>(
              entity1);
      auto [tIt_j, cIt_j] =
          getComponents<Transform2dComponent, SAPCollider>(entity2);
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
          // TODO: perform trigger callback
          PLOG_I("Trigger collision happened despite being WIP");
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

} // namespace Systems
} // namespace pain
