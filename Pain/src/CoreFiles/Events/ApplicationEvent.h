#pragma once

#include "Event.h"
#include <string>

namespace pain {
class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(unsigned int width, unsigned int height)
      : m_width(width), m_height(height) {}
  inline unsigned int GetWidth() const { return m_width; }
  inline unsigned int GetHeight() const { return m_height; }
  std::string ToString() const override {
    return "WindowResizeEvent: " + std::to_string(m_width) + ", " +
           std::to_string(m_height);
  }

private:
  unsigned int m_width, m_height;
};

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent() {}

  EVENT_CLASS_TYPE(WindowClose)
  EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

} // namespace pain
