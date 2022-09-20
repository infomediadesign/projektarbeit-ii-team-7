#ifndef __GAME_CONTROLLERS_MENU_CONTROLLER_H
#define __GAME_CONTROLLERS_MENU_CONTROLLER_H

#include "controller.h"
#include "../lua/entity.h"
#include "../lua/helpers.h"

class MenuController {
private:
  Controller base;
  Entity* background;

public:
  CONTROLLER_METHOD_DEFINITIONS
};

#endif
