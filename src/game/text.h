#ifndef __GAME_TEXT_H
#define __GAME_TEXT_H

#include "vector.h"

#include <engine/types/vector.h>
#include <string>

class Text {
public:
  std::string text;
  Vector4 pos;
  Vector4 color;
  Vector2 scale;
  bool active;

  Text() {
    this->text   = "";
    this->pos    = { 0.0f, 0.0f, 0.0f, 1.0f };
    this->color  = { 1.0f, 1.0f, 1.0f, 1.0f };
    this->scale  = { 1.0f, 1.0f };
    this->active = false;
  }

  Text(const std::string text, const Vector4 pos, const Vector4 color, const Vector2 scale) {
    this->text   = text;
    this->pos    = pos;
    this->color  = color;
    this->scale  = scale;
    this->active = true;
  }
};

#endif
