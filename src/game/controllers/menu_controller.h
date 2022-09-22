#ifndef __GAME_CONTROLLERS_MENU_CONTROLLER_H
#define __GAME_CONTROLLERS_MENU_CONTROLLER_H

#include "../lua/entity.h"
#include "../lua/helpers.h"
#include "controller.h"

class MenuController {
private:
  Controller base;
  Entity *background;

public:
  CONTROLLER_METHOD_DEFINITIONS
};

#endif
