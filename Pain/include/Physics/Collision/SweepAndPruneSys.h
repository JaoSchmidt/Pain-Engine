// SweepAndPruneSys.h
#pragma once

#include "Assets/DeltaTime.h"
#include "ECS/Systems.h"
namespace pain
{

namespace Systems
{
struct EndPoint {
  size_t key;
  float valueOnAxis;
  bool isMin;
  // used in the initial sort
};

struct EndPointKey {
  reg::Entity entity;
  size_t index_minX;
  size_t index_maxX;
  size_t index_minY;
  size_t index_maxY;
};

struct SweepAndPruneSys : public System<ComponentManager> {

  template <typename... Args>
    requires std::constructible_from<System<ComponentManager>, Args...>
  SweepAndPruneSys(Args &&...args) : System(std::forward<Args>(args)...){};

  SweepAndPruneSys() = delete;
  void onUpdate(DeltaTime deltaTime);
  // void deleteStaticEntities(std::span<reg::Entity> entities);
  // void deleteStaticEntity(reg::Entity entitiy);
  // void deleteEntity(reg::Entity entitiy);

  template <typename... Args> void insertColliders(const Args &...blob)
  {
    auto deBlob = [this](const auto &blob) {
      using E = std::remove_const_t<std::remove_cvref_t<decltype(blob)>>;

      if constexpr (std::same_as<E, reg::Entity>) {
        insertCollider(blob);
      } else if constexpr (std::same_as<E, std::vector<reg::Entity>>) {
        insertColliderSpan(blob);
      }
    };
    (deBlob(blob), ...);
    sortAfterInsertion();
  }

  size_t insertCollider(reg::Entity entity);
  void insertColliderSpan(const std::vector<reg::Entity> &entities);

private:
  void sortAfterInsertion();
  std::vector<EndPoint> m_endPointsX = {};
  std::vector<EndPoint> m_endPointsY = {};

  std::vector<EndPoint> m_staticEndPointsX = {};
  std::vector<EndPoint> m_staticEndPointsY = {};

  std::vector<EndPointKey> m_endPointKeys = {};
  std::vector<EndPointKey> m_staticEndPointKeys = {};

  std::vector<long> m_activeList = {};
  std::vector<long> m_activeStaticList = {};

  static inline long staticMSB(size_t x)
  {
    long lx = static_cast<long>(x);
    lx |= (1UL << ((sizeof(long) * 8) - 1));
    return lx;
  }
  static inline size_t clearMSB(long x)
  {
    x &= ~(1UL << ((sizeof(long) * 8) - 1));
    return static_cast<size_t>(x);
  }
};

} // namespace Systems
} // namespace pain
