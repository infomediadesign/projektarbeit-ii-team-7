#ifndef __GAME_CONTROLLERS_OVERWORLD_CONTROLLER_H
#define __GAME_CONTROLLERS_OVERWORLD_CONTROLLER_H

#include "controller.h"

class OverworldController {
private:
  Controller base;

  void check_collision(std::shared_ptr<Entity> ent);

public:
  CONTROLLER_METHOD_DEFINITIONS
};

#endif
