#include "game.h"

#include "binds.h"

#include <iostream>

void Game::update(GameState *state, mutex_t *lock) {
  if (this->input_state != nullptr && this->input_state->command_count > 0) {
    this->process_input(state);
    input_flush(this->input_state);
  }
}

void Game::update_lazy(GameState *state, mutex_t *lock) {}

void Game::update_paused(GameState *state, mutex_t *lock) {}

void Game::update_renderables(GameState *state, mutex_t *lock, Renderable *renderables, const u32 renderables_count) {}

void Game::create_bindings(GameState *state, mutex_t *lock, InputState *input_state) {
  this->input_state = input_state;

  input_bind(input_state, MF_KEY_W | MF_KEY_PRESS, Cmd::FORWARD);
  input_bind(input_state, MF_KEY_A | MF_KEY_PRESS, Cmd::LEFT);
  input_bind(input_state, MF_KEY_S | MF_KEY_PRESS, Cmd::BACK);
  input_bind(input_state, MF_KEY_D | MF_KEY_PRESS, Cmd::RIGHT);
  input_bind(input_state, MF_KEY_UP | MF_KEY_PRESS, Cmd::FORWARD);
  input_bind(input_state, MF_KEY_LEFT | MF_KEY_PRESS, Cmd::LEFT);
  input_bind(input_state, MF_KEY_DOWN | MF_KEY_PRESS, Cmd::BACK);
  input_bind(input_state, MF_KEY_RIGHT | MF_KEY_PRESS, Cmd::RIGHT);
  input_bind(input_state, MF_KEY_W | MF_KEY_RELEASE, -Cmd::FORWARD);
  input_bind(input_state, MF_KEY_A | MF_KEY_RELEASE, -Cmd::LEFT);
  input_bind(input_state, MF_KEY_S | MF_KEY_RELEASE, -Cmd::BACK);
  input_bind(input_state, MF_KEY_D | MF_KEY_RELEASE, -Cmd::RIGHT);
  input_bind(input_state, MF_KEY_UP | MF_KEY_RELEASE, -Cmd::FORWARD);
  input_bind(input_state, MF_KEY_LEFT | MF_KEY_RELEASE, -Cmd::LEFT);
  input_bind(input_state, MF_KEY_DOWN | MF_KEY_RELEASE, -Cmd::BACK);
  input_bind(input_state, MF_KEY_RIGHT | MF_KEY_RELEASE, -Cmd::RIGHT);
  input_bind(input_state, MF_KEY_SPACE | MF_KEY_PRESS, Cmd::FIRE);
  input_bind(input_state, MF_KEY_ENTER | MF_KEY_PRESS, Cmd::FIRE);
}

void Game::process_input(GameState *state) {
  for (u32 i = 0; i < this->input_state->command_count; i++) {
    if (this->player != nullptr) {
      switch (this->input_state->commands[i]) {
      case Cmd::FORWARD: this->player->set_velocity_x(0.5f); break;
      case -Cmd::FORWARD: this->player->set_velocity_x(0.0f); break;
      case Cmd::BACK: this->player->set_velocity_x(-0.5f); break;
      case -Cmd::BACK: this->player->set_velocity_x(0.0f); break;
      default: break;
      }
    }
  }
}
