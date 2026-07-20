/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Scripting/Lua/EngineBind.h"
#include "Assets/ManagerMaterial.h"
#include "Assets/ManagerShader.h"
#include "Assets/ManagerTexture.h"

namespace pain
{
MaterialCreationInfo parseMaterialInfo(sol::table t, pain::ShaderManager &sm,
                                       pain::MaterialManager &mm)
{
  Shader *shader = nullptr;

  sol::object shaderObj = t["shader"];

  if (!shaderObj.valid()) {
    PLOG_E("Material requires a shader!");
    shader = &sm.getDefaultShader(DefaultShader::Texture);
  } else if (shaderObj.is<DefaultShader>()) {
    shader = &sm.getDefaultShader(shaderObj.as<DefaultShader>());
  } else if (shaderObj.is<std::string>()) {
    shader = &sm.getShader(shaderObj.as<std::string>());
  }

  // --- defaults ---
  Color color{255, 255, 255, 255};

  if (auto c = t["color"]; c.valid())
    color = c.get<Color>();

  float tiling = t["tiling"].get_or(1.F);

  Texture *tex = &TextureManager::getDefaultTexture(
      TextureManager::DefaultTexture::Blank, false);

  sol::object texObj = t["texture"];

  if (texObj.is<TextureManager::DefaultTexture>()) {
    tex = &TextureManager::getDefaultTexture(
        texObj.as<TextureManager::DefaultTexture>(), false);
  } else if (texObj.is<std::string>()) {
    tex = &TextureManager::createTexture(texObj.as<std::string>().c_str());
  }

  // --- params (optional, simplified for now) ---
  std::variant<ParamPBR, ParamPhong, std::monostate> params;

  return MaterialCreationInfo{
      .color = color,
      .tilingFactor = tiling,
      .params = params,
      .shader = *shader,
      .texture = *tex,
  };
}

sol::table luabinder::bindEngine(sol::state &lua)
{
  sol::table engineTbl = lua.create_table();
  lua["Engine"] = engineTbl;
  return engineTbl;
}

void luabinder::bindEngineMM(sol::state &lua, ShaderManager &sm,
                              MaterialManager &mm)
{
  // ------------------------------------------------------------
  // Engine table (reuse if exists)
  // ------------------------------------------------------------
  sol::table engineTbl = lua["Engine"];

  // ------------------------------------------------------------
  // Material subtable
  // ------------------------------------------------------------
  sol::table matTbl = lua.create_table();

  // ============================================================
  // Shader API
  // ============================================================

  matTbl["load_shader_file"] =
      [&sm](const std::string &name,
            const std::string &path) -> pain::Shader & {
    return sm.loadShaderFromFile(name, path.c_str());
  };

  matTbl["load_shader_source"] =
      [&sm](const std::string &name, const std::string &vert,
            const std::string &frag) -> pain::Shader & {
    return sm.loadShaderFromStrings(name, vert, frag);
  };

  matTbl["get_shader"] = [&sm](const std::string &name) -> pain::Shader & {
    return sm.getShader(name);
  };

  matTbl["get_default_shader"] =
      [&sm](pain::DefaultShader type) -> pain::Shader & {
    return sm.getDefaultShader(type);
  };

  // ============================================================
  // Material API
  // ============================================================

  matTbl["create"] = [&sm, &mm](const std::string &name,
                                sol::table t) -> pain::Material & {
    MaterialCreationInfo info = parseMaterialInfo(std::move(t), sm, mm);
    return mm.createMaterial(name, info);
  };

  matTbl["get"] = [&mm](const std::string &name) -> pain::Material & {
    return mm.getMaterial(name);
  };

  matTbl["get_default"] = [&mm]() -> pain::Material & {
    return mm.getDefaultMaterial();
  };

  // ------------------------------------------------------------
  // Attach to Engine
  // ------------------------------------------------------------
  engineTbl["Material"] = matTbl;

  // ------------------------------------------------------------
  // DefaultShader enum
  // ------------------------------------------------------------
  lua.new_enum(                                               //
      "DefaultShader",                                        //
      "Texture", pain::DefaultShader::Texture,                //
      "Instancing", pain::DefaultShader::Instancing,          //
      "TexturePhong", pain::DefaultShader::TexturePhong,      //
      "Circles", pain::DefaultShader::Circles,                //
      "Grid", pain::DefaultShader::Grid,                      //
      "Text", pain::DefaultShader::Text,                      //
      "SimpleTriangles", pain::DefaultShader::SimpleTriangles //
  );
  lua.new_enum(                                           //
      "DefaultTexture",                                   //
      "General", TextureManager::DefaultTexture::General, //
      "Error", TextureManager::DefaultTexture::Error,     //
      "Blank", TextureManager::DefaultTexture::Blank      //
  );

  lua["Engine"] = engineTbl;
}

void luabinder::bindMaterial(sol::state &lua)
{
  lua.new_usertype<pain::Material>(
      "Material", sol::no_constructor,           //
      "color", &pain::Material::m_color,         //
      "tiling", &pain::Material::m_tilingFactor, //
      "set_texture",
      sol::overload(static_cast<void (pain::Material::*)(pain::Texture &)>(
                        &pain::Material::setTexture),
                    static_cast<void (pain::Material::*)(const char *)>(
                        &pain::Material::setTexture)),
      "set_texture_sheet", &pain::Material::setTextureSheet);
}

} // namespace pain
