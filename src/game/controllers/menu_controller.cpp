#include "menu_controller.h"

#include "../binds.h"

static const Vector3 center_vec3 = { 0.0f, -0.4375f, 0.0f };

void MenuController::init(GameState *state) {
	game_remove_flag(state, GS_PAUSED);

	this->background = this->base.ent_manager->ent_create();
	this->background->set_ent_class(EntClass::BACKGROUND);
	this->background->set_texture_path("assets/ui/startmenu.png");
	this->background->set_pos(this->base.ent_manager->get_player_ent()->get_pos());
	this->background->set_scale({ 20.0f, 11.25f });
	this->background->set_active(true);

	this->base.ent_manager->get_player_ent()->set_pos({ -100.0f, -100.0f });

}

void MenuController::update(GameState *state, mutex_t *lock) {}

void MenuController::update_lazy(GameState *state, mutex_t *lock) {}

void MenuController::update_paused(GameState *state, mutex_t *lock) {}

void MenuController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {}

void MenuController::destroy(GameState *state) {
	this->base.ent_manager->clear_entities();
}

void MenuController::process_input(GameState* state, const f64 update_time) {}
