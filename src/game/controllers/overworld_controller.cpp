#include "overworld_controller.h"

#include "../vector.h"

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void OverworldController::init(GameState *state) {
  for (u8 x = 0; x < 16; x++) {
    for (u8 y = 0; y < 16; y++) {
      std::shared_ptr<Entity> ent = this->base.ent_manager->ent_create();
      ent->set_ent_class(EntClass::BACKGROUND);
      ent->set_texture_path(
        (x != 0 && x != 15 && y != 0 && y != 15) ? "assets/debug/path_32x32.png" : "assets/debug/wall_32x32.png"
      );
      ent->set_pos(center_vec3 + vector_make3(-0.8f + 0.1f * x, -0.8f + 0.1f * y, 0.0f));
      ent->set_active(true);
    }
  }
}

void OverworldController::update(GameState *state, mutex_t *lock) {}

void OverworldController::update_lazy(GameState *state, mutex_t *lock) {}

void OverworldController::update_paused(GameState *state, mutex_t *lock) {}

void OverworldController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable *renderables, const u32 renderables_count
) {}

void OverworldController::destroy(GameState *state) {}
