#ifndef __GAME_CONTROLLERS_DUNGEON_CONTROLLER_H
#define __GAME_CONTROLLERS_DUNGEON_CONTROLLER_H

#include "../level.h"
#include "controller.h"

class DungeonController {
private:
  Controller base;
  Level *level;

public:
  CONTROLLER_METHOD_DEFINITIONS
  void changelevel(const std::string level);
};

#endif
