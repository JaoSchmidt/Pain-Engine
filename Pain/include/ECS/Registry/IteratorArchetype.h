#include "ECS/Entity.h"

namespace reg
{

template <typename T> struct Iterator {
  // https://internalpointers.com/post/writing-custom-iterators-modern-cpp
  using iterator_category = std::forward_iterator_tag;

  Iterator(const std::vector<std::vector<T> *> &vectors, size_t outerIndex,
           size_t innerIndex,
           const std::vector<const std::vector<pain::Entity> *> &entities)
      : m_vectors(vectors), m_outerIndex(outerIndex), m_innerIndex(innerIndex),
        m_entities(entities)
  {
  }

  const T &operator*() const { return m_vectors[m_outerIndex][m_innerIndex]; }
  T &operator*() { return m_vectors[m_outerIndex][m_innerIndex]; }
  const T *operator->() const { return m_vectors[m_outerIndex][m_innerIndex]; }
  T *operator->() { return m_vectors[m_outerIndex][m_innerIndex]; }

  // Prefix increment i.e.: ++it
  Iterator &operator++()
  {
    if (++m_innerIndex >= m_vectors[m_outerIndex].size()) {
      m_innerIndex = 0;
      ++m_outerIndex;
    }
    return *this;
  }

  bool operator==(const Iterator &o)
  {
    return &m_vectors == &o.m_vectors && m_outerIndex == o.m_outerIndex &&
           m_innerIndex == o.m_innerIndex && m_entities == o.m_entities;
  };
  bool operator!=(const Iterator &o) { return !(*this == o); };

  // get the entity index inside the archetype
  inline const pain::Entity getEntity() const
  {
    P_ASSERT(m_entities.size() == m_vectors.size(),
             "Number of entities is different from the number of components, "
             "they are the same archetypes");
    P_ASSERT(m_entities[m_outerIndex]->size() == m_vectors[m_outerIndex].size(),
             "Number of entities in an archetype should be the same as their "
             "components");
    return (*m_entities[m_outerIndex])[m_innerIndex];
  }

private:
  const std::vector<const std::vector<pain::Entity> *> &m_entities;
  const std::vector<std::vector<T> *> &m_vectors;
  size_t m_outerIndex;
  size_t m_innerIndex;
};

} // namespace reg
