#ifndef __ENGINE_STATE_STATE_H
#define __ENGINE_STATE_STATE_H

#include "../platform.h"

/**
 * @brief Current state of the game.
 *
 * Contains information about the game, the variables, flags
 * entities and everything else. Is used to communicate the
 * current state of things between various systems of the
 * game engine.
 *
 */
typedef struct GameState {
  uint32_t flags;
  uint64_t tick;
  uint32_t tickrate;
  uint32_t fps_max;
} GameState;

/**
 * @brief Game state flags.
 *
 * Various boolean variables that are used to control the
 * behavior of the engine.
 *
 */
enum GameFlag { GS_PAUSED = 1, GS_DEBUG = 2, GS_EXIT = 4 };

/**
 * @brief Creates a new game state.
 *
 * ```
 * GameState *state = game_new_state();
 * ```
 *
 * @return GameState* Newly created game state.
 */
GameState *game_new_state();

/**
 * @brief Adds a boolean flag to the game flags.
 *
 * ```
 * game_add_flag(state, GameFlag::PAUSED);
 * ```
 *
 * @param state Current game state
 * @param flag The flag to add
 */
void game_add_flag(GameState *state, const uint32_t flag);

/**
 * @brief Checks whether game state is paused or not.
 *
 * ```
 * if (game_is_paused(state)) {
 *   think_while_paused();
 *   return;
 * }
 * ```
 *
 * @param state Current game state
 * @return uint8_t Whether or not the game is paused
 */
uint8_t game_is_paused(GameState *state);

/**
 * @brief Checks whether the game is in debug mode.
 *
 * ```
 * if (game_is_debug(state)) {
 *   draw_debug_gui();
 * }
 * ```
 *
 * @param state Current game state
 * @return uint8_t Whether or not the game is in debug mode
 */
uint8_t game_is_debug(GameState *state);

/**
 * @brief Checks whether or not the game should exit.
 *
 * ```
 * int main() {
 *   GameState *state = (Gamestate *)malloc(sizeof(GameState));
 *
 *   while (!game_should_exit(state)) {
 *     game_logic(state);
 *   }
 *
 *   free(state);
 *
 *   return 0;
 * }
 * ```
 *
 * @param state Current game state
 * @return uint8_t Whether or not the game should exit.
 */
uint8_t game_should_exit(GameState *state);

#endif
