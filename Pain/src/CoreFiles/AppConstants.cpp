#include "CoreFiles/AppConstants.h"

namespace pain
{

unsigned AppConstants::getProcessorCount()
{
  return std::thread::hardware_concurrency();
}

} // namespace pain
