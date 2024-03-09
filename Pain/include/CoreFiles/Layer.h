#pragma once
#include "pch.gch"

#include "Core.h"

namespace pain
{
class EXPORT Layer
{
public:
  Layer(const std::string id);
  // virtual because it is supposed to be defined inside a game
  virtual ~Layer();

  virtual void onAttach() = 0;
  virtual void onDetach() = 0;
  virtual void onUpdate() = 0;
  virtual void onEvent() = 0;

  inline const std::string &GetId() const { return m_debugId; }

protected:
  std::string m_debugId;
};

} // namespace pain
