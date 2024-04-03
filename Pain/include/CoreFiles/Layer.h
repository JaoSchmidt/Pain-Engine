#pragma once
#include "pch.h"

#include "Core.h"
#include "DeltaTime.h"

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
  virtual void onUpdate(DeltaTime deltaTime) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

  inline const std::string &GetId() const { return m_debugId; }

protected:
  std::string m_debugId;
};

} // namespace pain
