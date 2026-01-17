/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
// RandNumberGenerator.h
#pragma once

#include <random>
#include <type_traits>

namespace pain
{

/**
 * @class RNG
 * @brief Random number generator utility providing Gaussian and uniform
 * sampling.
 *
 * Internally uses a Mersenne Twister engine (`std::mt19937`) seeded from
 * `std::random_device`.
 *
 * The generator supports:
 * - Gaussian (normal) distributions
 * - Uniform distributions for both floating-point and integral types
 *
 * The class is lightweight and intended for gameplay, simulation, and tools.
 */
class RNG
{
public:
  /**
   * @brief Constructs the RNG with a random seed and default Gaussian
   * parameters.
   *
   * Default Gaussian distribution:
   * - Mean = 0.0
   * - Standard deviation = 1.0
   */
  RNG() : generator(std::random_device{}()), m_mean(0.0), m_stddev(1.0) {}

  /**
   * @brief Constructs the RNG with a random seed and custom Gaussian
   * parameters.
   *
   * @param mean    Mean of the Gaussian distribution.
   * @param stddev  Standard deviation of the Gaussian distribution.
   */
  RNG(double mean, double stddev)
      : generator(std::random_device{}()), m_mean(mean), m_stddev(stddev) {};

  /**
   * @brief Generates a random number using the internally configured Gaussian
   * distribution.
   *
   * @tparam T Numeric type to generate (typically float or double).
   * @return Random value sampled from a normal distribution.
   */
  template <typename T> const T gaussian() const
  {
    std::normal_distribution<T> distribution(m_mean, m_stddev);
    return distribution(generator);
  }

  /**
   * @brief Generates a random number using a custom Gaussian distribution.
   *
   * @tparam T Numeric type to generate (typically float or double).
   * @param mean    Mean of the distribution.
   * @param stddev  Standard deviation of the distribution.
   * @return Random value sampled from a normal distribution.
   */
  template <typename T> const T gaussian(T mean, T stddev) const
  {
    std::normal_distribution<T> distribution(mean, stddev);
    return distribution(generator);
  }

  /**
   * @brief Generates a floating-point random value in the range [0.0, 1.0].
   *
   * Enabled only for floating-point types.
   *
   * @tparam T Floating-point type.
   * @return Random value in the range [0.0, 1.0].
   */
  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, T> uniform() const
  {
    std::uniform_real_distribution<T> distribution(0.0, 1.0);
    return distribution(generator);
  }

  /**
   * @brief Generates a uniformly distributed integer in a given range.
   *
   * Enabled only for integral types.
   *
   * @tparam T Integral type.
   * @param min  Inclusive minimum value.
   * @param max  Inclusive maximum value.
   * @return Random integer in the specified range.
   */
  template <typename T>
  std::enable_if_t<std::is_integral_v<T>, T> uniform(T min, T max) const
  {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
  }

  /**
   * @brief Generates a uniformly distributed floating-point value in a given
   * range.
   *
   * Enabled only for floating-point types.
   *
   * @tparam T Floating-point type.
   * @param min  Minimum value.
   * @param max  Maximum value.
   * @return Random value in the specified range.
   */
  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, T> uniform(T min, T max) const
  {
    std::uniform_real_distribution<T> distribution(min, max);
    return distribution(generator);
  }

private:
  /** Mersenne Twister random number generator engine. */
  mutable std::mt19937 generator;
  /** Mean of the Gaussian distribution. */
  double m_mean;
  /** Standard deviation of the Gaussian distribution. */
  double m_stddev;
};

} // namespace pain
