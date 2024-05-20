#pragma once

struct transform_system {
  // we'll use a constant time for this demonstration
  float dt = 0.1f;

  void update(Registry &reg)
  {
    for (int e = 1; e <= max_entity; e++) {
      // and each entity with a transform compnent will update
      // their position with respect to their velocity
      if (reg.transforms.contains(e)) {
        reg.transforms[e].pos_x += reg.transforms[e].vel_x * dt;
        reg.transforms[e].pos_y += reg.transforms[e].vel_y * dt;
      }
    }
  }
};
