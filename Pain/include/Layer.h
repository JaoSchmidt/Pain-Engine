#pragma once

#include "CoreFiles/Core.h"

namespace pain {
class EXPORT Layer {
public:
  Layer(const int id = 0);
  virtual ~Layer();

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void OnUpdate() {}
  virtual void OnEvent() {}

  inline const int &GetId() const { return m_DebugId; }

protected:
  int m_DebugId;
};

} // namespace pain
