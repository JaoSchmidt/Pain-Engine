/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "Core.h"
#include <filesystem>

namespace pain
{

/**
 * @namespace FileManager
 * @brief Centralized resource and filesystem management API.
 *
 * Provides a unified interface for accessing files, resolving paths,
 * loading Lua scripts, and handling default resource locations.
 * This layer helps avoid duplicated filesystem logic and reduces the
 * risk of fatal IO errors across the engine.
 */
namespace FileManager
{

// =================================================================
// Assure default redundancy
// =================================================================

/**
 * @brief Loads and caches the default Lua script.
 *
 * Ensures that a valid default Lua script is available in memory
 * before any script execution occurs.
 */
void initiateDefaultScript();

/**
 * @brief Returns the current default resource base path.
 *
 * This path is used as the base directory when resolving relative paths.
 *
 * @return Reference to the internal default resource path string.
 */
const std::string &getDefaultPath();

/**
 * @brief Sets the default resource base path.
 *
 * The path is normalized and canonicalized. If the provided path ends
 * with a "resources" directory, the parent directory is used instead.
 *
 * @param path Null-terminated string representing the base path.
 */
void setDefaultPath(const char *path);

/**
 * @brief Resolves a relative or absolute path into a normalized full path.
 *
 * If the path is relative, it is resolved against the default resource path.
 *
 * @param relativeOrAbsolute Relative or absolute path.
 * @return Normalized absolute path string.
 */
std::string getFullPath(const char *relativeOrAbsolute);

/**
 * @brief Returns the current working directory.
 *
 * Equivalent to calling getcwd().
 *
 * @return Current working directory path.
 */
std::string getCurrentWorkingDir();

/**
 * @brief Returns the current working directory with an appended path.
 *
 * Equivalent to getcwd() followed by appending a relative path.
 *
 * @param append Path segment to append.
 * @return Combined path string.
 */
std::string getCurrentWorkingDir(std::string append);

// =================================================================
// Lua Related Stuff
// =================================================================

/**
 * @brief Loads and returns the contents of a Lua script file.
 *
 * The file is cached after the first load to avoid repeated disk IO.
 * If the file cannot be found, the default Lua script is returned instead.
 *
 * @param filepath Path to the Lua script file.
 * @return Reference to the cached script source string.
 */
const std::string &getLuaScriptSource(const char *filepath);

/**
 * @brief Returns the path to the default Lua script file.
 *
 * @return Null-terminated string containing the default Lua script path.
 */
const char *getDefaultLuaFile();

/**
 * @brief Clears all cached Lua script sources.
 *
 * Forces subsequent calls to reload scripts from disk.
 */
void clearScript();

// =================================================================
// FileSystem wrappers
// =================================================================

/**
 * @brief Checks if a file or directory exists (C-string overload).
 *
 * @param name Path to check.
 * @return True if the path exists, false otherwise.
 */
bool existsFile(const char *name);

/**
 * @brief Checks if a file or directory exists (std::string overload).
 *
 * @param name Path to check.
 * @return True if the path exists, false otherwise.
 */
bool existsFile(const std::string &name);

/**
 * @brief Checks if a file or directory exists (std::string_view overload).
 *
 * @param name Path to check.
 * @return True if the path exists, false otherwise.
 */
bool existsFile(const std::string_view &name);

/**
 * @brief Checks whether a path refers to a regular file.
 *
 * @param name Path to check.
 * @return True if the path is a file.
 */
bool isFile(const std::string &name);

/**
 * @brief Checks whether a path refers to a directory.
 *
 * @param name Path to check.
 * @return True if the path is a directory.
 */
bool isDir(const std::string &name);

/**
 * @brief Creates an empty file on disk.
 *
 * @param filename Path of the file to create.
 * @return True if the file was successfully created, false otherwise.
 */
bool createFile(const std::string &filename);

/**
 * @brief Joins multiple path segments into a single filesystem path.
 *
 * Uses std::filesystem::path concatenation rules to safely assemble paths.
 *
 * @tparam P Variadic path segment types.
 * @param parts Path segments to join.
 * @return Joined path as a string.
 */
template <typename... P> std::string pathJoin(const P &...parts)
{
  std::filesystem::path result;
  (result /= ... /= parts);
  return result.string();
}

} // namespace FileManager

} // namespace pain
