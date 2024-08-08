#include <pain.h>

class PlayerController : pain::ScriptableEntity
{
public:
  void onUpdate(double deltaTimeSec);
};

class Player : public pain::GameObject
{
public:
  Player(pain::Scene *scene);
};
