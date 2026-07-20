#pragma once
/** @brief Aggregated rendering statistics for a batch type.*/
/* Fields:
 * - name: batch name
 * - count: number of objects
 * - indices: total index count
 * - vertices: total vertex count
 * - draws: draw calls issued
 */
struct Stats {
  const char *name;      ///< Human-readable batch name
  uint32_t count = 0;    ///< Number of objects in the batch
  uint32_t indices = 0;  ///< Total index count submitted
  uint32_t vertices = 0; ///< Total vertex count submitted
  uint32_t draws = 0;    ///< Number of draw calls issued

  Stats &operator+=(const Stats &s)
  {
    count += s.count;
    indices += s.indices;
    vertices += s.vertices;
    draws += s.draws;
    return *this;
  }
  Stats operator+(const Stats &s) const
  {
    Stats result = *this;
    result += s;
    return result;
  }
};
