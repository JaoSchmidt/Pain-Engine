#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Entity.h"

namespace reg
{

template <typename T> struct Iterator {
  // https://internalpointers.com/post/writing-custom-iterators-modern-cpp
  using iterator_category = std::forward_iterator_tag;

  Iterator(std::vector<std::vector<T> *> vectors, size_t outerIndex,
           size_t innerIndex)
      : m_vectors(vectors), m_outerIndex(outerIndex),
        m_innerIndex(innerIndex) {};

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
  Iterator &operator+=(std::ptrdiff_t n)
  {
    while (n > 0) {
      ++(*this);
      --n;
    }
    return *this;
  }

  bool operator==(const Iterator &o)
  {
    return m_outerIndex == o.m_outerIndex && m_innerIndex == o.m_innerIndex;
  };
  bool operator!=(const Iterator &o) { return !(*this == o); };

  friend Iterator operator+(Iterator it, std::ptrdiff_t n)
  {
    it += n;
    return it;
  }

protected:
  std::vector<std::vector<T> *> m_vectors;
  size_t m_outerIndex;
  size_t m_innerIndex;
};

template <typename T> struct IteratorWithEntities : public Iterator<T> {
  IteratorWithEntities(std::vector<std::vector<T> *> vectors, int outerIndex,
                       int innerIndex,
                       std::vector<std::vector<reg::Entity> *> entities)
      : Iterator<T>(vectors, outerIndex, innerIndex), m_entities(entities) {};

  inline reg::Entity getEntity() const
  {
    P_ASSERT(m_entities.size() == this->m_vectors.size(),
             "Number of entities is different from the number of components, "
             "they are the same archetypes");
    P_ASSERT(m_entities.at(this->m_outerIndex)->size() ==
                 this->m_vectors.at(this->m_outerIndex)->size(),
             "Number of entities in an archetype should be the same as their "
             "components");
    return m_entities.at(this->m_outerIndex)->at(this->m_innerIndex);
  }

private:
  std::vector<std::vector<reg::Entity> *> m_entities;
};

} // namespace reg
