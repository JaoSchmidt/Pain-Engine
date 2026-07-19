/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Text/ManagerFont.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

FontManager FontManager::create()
{
  // std::optional<Font> font =
  Font *font =
      Font::create("resources/default/fonts/OpenSans-Regular.ttf", 40.0);
  if (!font) {
    PLOG_E("Default font is missing, terminating.");
    std::terminate();
  }
  return FontManager(font);
}

FontManager::FontManager(Font *font) : m_defaultFont(std::move(*font)) {};

Font &FontManager::createFont(const std::string_view &name,
                              const char *filepath)
{
  return createFont(name, filepath, 40.0);
}

Font &FontManager::createFont(const std::string_view &name,
                              const char *filepath, double emSize)
{
  auto it = m_fonts.find(name);
  if (it != m_fonts.end()) {
    PLOG_W("Attention: You are re-creating the font {} which already exists "
           "inside the font manager, perhaps you meant to use "
           "getFont(\"{}\")?",
           name, name);
  }

  auto font = Font::create(filepath, emSize);
  if (font) {
    auto [it, inserted] = m_fonts.emplace(name, std::move(*font));
    return it->second;
  }
  PLOG_E("FontManager::loadFont could not load font \"{}\" on path \"{}\". "
         "Maybe it's missing?",
         name, filepath);
  return getDefault();
}

Font &FontManager::getFont(const std::string_view &name)
{
  auto it = m_fonts.find(name);
  if (it == m_fonts.end()) {
    PLOG_W("Font {} not found, returning default", name);
    return getDefault();
  }

  return it->second;
}

Font &FontManager::getDefault() { return m_defaultFont; }

} // namespace pain
