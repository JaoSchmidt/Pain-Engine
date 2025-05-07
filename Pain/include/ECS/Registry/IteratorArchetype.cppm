module;
#include "CoreFiles/LogWrapper.h"
#include <iterator>
#include <vector>
export module pain.IteratorArchetype;
import pain.Entity;

export namespace reg
{

template <typename T> struct Iterator {
  // https://internalpointers.com/post/writing-custom-iterators-modern-cpp
  using iterator_category = std::forward_iterator_tag;

  Iterator(std::vector<std::vector<T> *> vectors, int outerIndex,
           int innerIndex, std::vector<std::vector<pain::Entity> *> entities)
      : m_entities(entities), m_vectors(vectors), m_outerIndex(outerIndex),
        m_innerIndex(innerIndex)
  {
  }

  const T &operator*() const
  {
    return static_cast<const std::vector<T> *>(m_vectors.at(m_outerIndex))
        ->at(m_innerIndex);
  }
  T &operator*() { return m_vectors.at(m_outerIndex)->at(m_innerIndex); }
  const T *operator->() const
  {
    return &static_cast<const std::vector<T>>(m_vectors.at(m_outerIndex))
                ->at(m_innerIndex);
  }
  T *operator->() { return &m_vectors.at(m_outerIndex)->at(m_innerIndex); }

  // Prefix increment i.e.: ++it
  Iterator &operator++()
  {
    if (++m_innerIndex >= m_vectors.at(m_outerIndex)->size()) {
      m_innerIndex = 0;
      ++m_outerIndex;
    }
    return *this;
  }

  bool operator==(const Iterator &o)
  {
    return m_outerIndex == o.m_outerIndex && m_innerIndex == o.m_innerIndex;
  };
  bool operator!=(const Iterator &o) { return !(*this == o); };

  // get the entity index inside the archetype
  inline pain::Entity getEntity() const
  {
    P_ASSERT(m_entities.size() == m_vectors.size(),
             "Number of entities is different from the number of components, "
             "they are the same archetypes");
    P_ASSERT(m_entities.at(m_outerIndex)->size() ==
                 m_vectors.at(m_outerIndex)->size(),
             "Number of entities in an archetype should be the same as their "
             "components");
    return m_entities.at(m_outerIndex)->at(m_innerIndex);
  }

private:
  std::vector<std::vector<pain::Entity> *> m_entities;
  std::vector<std::vector<T> *> m_vectors;
  size_t m_outerIndex;
  size_t m_innerIndex;
};

} // namespace reg
