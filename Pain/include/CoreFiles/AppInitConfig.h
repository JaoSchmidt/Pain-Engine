#pragma once

/**
 * @struct AppContext
 * @brief Configuration structure used to initialize the Application.
 *
 * Provides window parameters and configuration file paths used during startup.
 */
struct AppInit {
  /** Default external configuration file name. */
  static constexpr const char *configIniFile = "config.ini";
  /** Default internal configuration file name. */
  static constexpr const char *internalConfigFile = "internalConfig.ini";
  /** Window title displayed in the OS window. */
  const char *title = "Unnamed Game";
  /** Initial window width in pixels. */
  int defaultWidth = 800;
  /** Initial window height in pixels. */
  int defaultHeight = 600;
  /** Initial camera zoom, assuming a 2d camera */
  float defaultZoom2d = 1.f;
};
