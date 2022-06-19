#ifndef __GAME_CONTROLLERS_BATTLE_CONTROLLER_H
#define __GAME_CONTROLLERS_BATTLE_CONTROLLER_H

#include "controller.h"

class BattleController {
private:
  Controller base;
  std::shared_ptr<Entity> background;
  std::shared_ptr<Entity> opponent;
  std::vector<std::shared_ptr<Entity>> buttons;

public:
  CONTROLLER_METHOD_DEFINITIONS
};

#endif
