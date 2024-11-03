#include <random>

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

  // Method to generate a random number with a Gaussian distribution
  const double gaussian() const
  {
    std::normal_distribution<double> distribution(m_mean, m_stddev);
    return distribution(generator);
  }
  const double gaussian(double mean, double stddev) const
  {
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(generator);
  }

private:
  std::mt19937 generator; // Mersenne Twister random number generator
  double m_mean;          // Mean of the Gaussian distribution
  double m_stddev;        // Standard deviation of the Gaussian distribution
};

} // namespace pain
