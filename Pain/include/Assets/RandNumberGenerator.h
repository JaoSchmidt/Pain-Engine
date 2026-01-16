/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include <random>
#include <type_traits>

namespace pain
{

class RNG
{
public:
  // Constructor initializes the generator with a seed
  RNG() : generator(std::random_device{}()), m_mean(0.0), m_stddev(1.0) {}

  // Constructor allowing specific mean and stddev values
  RNG(double mean, double stddev)
      : generator(std::random_device{}()), m_mean(mean), m_stddev(stddev)
  {
  }

  // Generate a random number with a Gaussian distribution
  template <typename T> const T gaussian() const
  {
    std::normal_distribution<T> distribution(m_mean, m_stddev);
    return distribution(generator);
  }

  // Generate a random number with a Gaussian distribution with specified mean
  // and stddev
  template <typename T> const T gaussian(T mean, T stddev) const
  {
    std::normal_distribution<T> distribution(mean, stddev);
    return distribution(generator);
  }

  // Generate a random floating-point number with a uniform distribution between
  // 0.0 and 1.0
  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, T> uniform() const
  {
    std::uniform_real_distribution<T> distribution(0.0, 1.0);
    return distribution(generator);
  }

  // Generate a random integer number with a uniform distribution in the
  // specified range
  template <typename T>
  std::enable_if_t<std::is_integral_v<T>, T> uniform(T min, T max) const
  {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
  }

  // Generate a random floating-point number with a uniform distribution in the
  // specified range
  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, T> uniform(T min, T max) const
  {
    std::uniform_real_distribution<T> distribution(min, max);
    return distribution(generator);
  }

private:
  mutable std::mt19937 generator; // Mersenne Twister random number generator
  double m_mean;                  // Mean of the Gaussian distribution
  double m_stddev; // Standard deviation of the Gaussian distribution
};

} // namespace pain
