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
           m_innerIndex == o.m_innerIndex;
  };
  bool operator!=(const Iterator &o) { return !(*this == o); };

  // get the entity index inside the archetype
  inline const size_t getEntityIndex() const { return m_innerIndex; }

private:
  const std::vector<std::vector<T> *> &m_vectors;
  const std::vector<const std::vector<pain::Entity> *> &m_entities;
  size_t m_outerIndex;
  size_t m_innerIndex;
};

} // namespace reg
