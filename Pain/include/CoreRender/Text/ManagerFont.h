/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "CoreRender/Text/Font.h"

namespace pain
{

class FontManager
{
public:
  static FontManager create();

  Font &createFont(const std::string_view &name, const char *filepath);
  Font &createFont(const std::string_view &name, const char *filepath,
                   double emSize);
  Font &getFont(const std::string_view &name);
  Font &getDefault();

private:
  FontManager(Font *font);
  std::map<std::string, Font, std::less<>> m_fonts = {};
  Font m_defaultFont;
};

} // namespace pain
