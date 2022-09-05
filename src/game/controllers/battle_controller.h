#ifndef __GAME_CONTROLLERS_BATTLE_CONTROLLER_H
#define __GAME_CONTROLLERS_BATTLE_CONTROLLER_H

#include "controller.h"

class BattleController {
private:
  Controller base;
  Entity *background;
  Entity *opponent;
  std::vector<Entity *> buttons;

public:
  CONTROLLER_METHOD_DEFINITIONS
};

#endif
