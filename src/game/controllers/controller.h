#ifndef __GAME_CONTROLLERS_CONTROLLER_H
#define __GAME_CONTROLLERS_CONTROLLER_H

#include <engine/input/input.h>
#include <engine/platform.h>
#include <engine/render/renderable.h>
#include <engine/state/state.h>
#include <engine/types/numeric.h>
#include <lua.hpp>
#include <memory>
#include <vector>

#define CONTROLLER_METHOD_DEFINITIONS \
  void init();                        \
  void update();                      \
  void update_lazy();                 \
  void update_renderables();

class Controller {
public:
  lua_State *lua;
};

#endif
