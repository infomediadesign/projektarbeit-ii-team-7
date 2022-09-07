#include "overworld_controller.h"

#include "../binds.h"
#include "../vector.h"

#define PLAYER_ENT this->base.ent_manager->get_player_ent()

void OverworldController::init(GameState* state) {
	this->level = new Level(this->base.lua, this->base.ent_manager);
	this->level->load_json("levels/dev.json");
	this->level->init();

	PLAYER_ENT->set_pos({ 0.2f, 0.2f, 0.0f });
}

void OverworldController::update(GameState* state, mutex_t* lock) {
	this->check_collision(PLAYER_ENT);

	this->base.ent_manager->get_player()->update_anim();
}

void OverworldController::update_lazy(GameState* state, mutex_t* lock) {}

void OverworldController::update_paused(GameState* state, mutex_t* lock) {}

void OverworldController::update_renderables(
	GameState* state, mutex_t* lock, RenderState* render_state, Renderable** renderables, const u32 renderables_count
) {
	if (this->base.ent_manager->player == nullptr)
		return;

	const std::shared_ptr<Entity> ply = this->base.ent_manager->player->get_base();

	if (this->base.ent_manager->is_valid(ply)) {
		const f32 delta = (f32)(platform_time_f64() - ply->get_updated_at());

#ifndef _WIN32
		const f32 lerp_factor = 0.05f;
#else
		const f32 lerp_factor = 0.1f;
#endif

		render_state->camera_transform.w[0] = util_lerp_f32(
			lerp_factor,
			render_state->camera_transform.w[0],
			-util_lerp_f32(delta, ply->get_pos().x, ply->get_pos().x + ply->get_velocity().x) * render_state->render_scale
		);

		render_state->camera_transform.w[1] = util_lerp_f32(
			lerp_factor,
			render_state->camera_transform.w[1],
			-util_lerp_f32(delta, ply->get_pos().y, ply->get_pos().y + ply->get_velocity().y) * (768.0f / 432.0f) *
			render_state->render_scale
		);
	}
}

void OverworldController::destroy(GameState* state) {
	this->base.ent_manager->clear_entities();
	delete this->level;
}

void OverworldController::process_input(GameState* state, const f64 update_time) {
	for (u32 i = 0; i < this->base.input_state->command_count; i++) {
		if (PLAYER_ENT != nullptr) {
			switch (this->base.input_state->commands[i]) {
			case Cmd::FORWARD: PLAYER_ENT->set_velocity_y(-0.5f); break;
			case -Cmd::FORWARD:
				if (PLAYER_ENT->get_velocity().y < 0.0f)
					PLAYER_ENT->set_velocity_y(0.0f);
				break;
			case Cmd::BACK: PLAYER_ENT->set_velocity_y(0.5f); break;
			case -Cmd::BACK:
				if (PLAYER_ENT->get_velocity().y > 0.0f)
					PLAYER_ENT->set_velocity_y(0.0f);
				break;
			case Cmd::LEFT: PLAYER_ENT->set_velocity_x(-0.5f); break;
			case -Cmd::LEFT:
				if (PLAYER_ENT->get_velocity().x < 0.0f)
					PLAYER_ENT->set_velocity_x(0.0f);
				break;
			case Cmd::RIGHT: PLAYER_ENT->set_velocity_x(0.5f); break;
			case -Cmd::RIGHT:
				if (PLAYER_ENT->get_velocity().x > 0.0f)
					PLAYER_ENT->set_velocity_x(0.0f);
				break;

			default: break;
			}
		}
	}
}

void OverworldController::check_collision(std::shared_ptr<Entity> ent) {
	if (!this->base.ent_manager->is_valid(ent))
		return;

	for (std::shared_ptr<Entity> target : this->base.ent_manager->entities) {
		/*if (!this->base.ent_manager->is_valid(target))
			continue;*/

		if (target->get_id() == ent->get_id())
			continue;

		if (ent->collides_with(target)) {
			if (target->get_ent_class() == EntClass::CLIP) {

				if ((target->get_pos().y - 0.05) - (ent->get_pos().y + 0.05) <= 0) {
					
					f64 l = 0.1;

					if (ent->get_pos().x < target->get_pos().x)
						l = (ent->get_pos().x + 0.05) - (target->get_pos().x - 0.05);
					if (ent->get_pos().x > target->get_pos().x)
						l = (target->get_pos().x + 0.05) - (ent->get_pos().x - 0.05);
					if (ent->get_pos().x == target->get_pos().x)
						l = 0.1;

					if (l >= (ent->get_pos().y + 0.05) - (target->get_pos().y - 0.05)) {
						ent->set_velocity_y(0.0f);
						ent->set_pos(vector_make3(ent->get_pos().x, target->get_pos().y - 0.1, ent->get_pos().z));
					}
				}

				if ((ent->get_pos().y - 0.05) - (target->get_pos().y + 0.05) <= 0) {
					
					f64 l = 0.1;

					if (ent->get_pos().x < target->get_pos().x)
						l = (ent->get_pos().x + 0.05) - (target->get_pos().x - 0.05);
					if (ent->get_pos().x > target->get_pos().x)
						l = (target->get_pos().x + 0.05) - (ent->get_pos().x - 0.05);
					if (ent->get_pos().x == target->get_pos().x)
						l = 0.1;

					if (l >= (target->get_pos().y + 0.05) - (ent->get_pos().y - 0.05)) {
						ent->set_velocity_y(0.0f);
						ent->set_pos(vector_make3(ent->get_pos().x, target->get_pos().y + 0.1, ent->get_pos().z));
					}
				}

				if ((ent->get_pos().x - 0.05) - (target->get_pos().x + 0.05) <= 0) {
					
					f64 l = 0.1;

					if (ent->get_pos().y < target->get_pos().y)
						l = (ent->get_pos().y + 0.05) - (target->get_pos().y - 0.05);
					if (ent->get_pos().y > target->get_pos().y)
						l = (target->get_pos().y + 0.05) - (ent->get_pos().y - 0.05);
					if (ent->get_pos().y == target->get_pos().y)
						l = 0.1;

					if (l >= (target->get_pos().x + 0.05) - (ent->get_pos().x - 0.05)) {
						ent->set_velocity_x(0.0f);
						ent->set_pos(vector_make3(target->get_pos().x + 0.1, ent->get_pos().y, ent->get_pos().z));
					}
				}

				if ((target->get_pos().x - 0.05) - (ent->get_pos().x + 0.05) <= 0) {
					
					f64 l = 0.1;

					if (ent->get_pos().y < target->get_pos().y)
						l = (ent->get_pos().y + 0.05) - (target->get_pos().y - 0.05);
					if (ent->get_pos().y > target->get_pos().y)
						l = (target->get_pos().y + 0.05) - (ent->get_pos().y - 0.05);
					if (ent->get_pos().y == target->get_pos().y)
						l = 0.1;

					if (l >= (ent->get_pos().x + 0.05) - (target->get_pos().x - 0.05)) {
						ent->set_velocity_x(0.0f);
						ent->set_pos(vector_make3(target->get_pos().x - 0.1, ent->get_pos().y, ent->get_pos().z));
					}
				}
			}
		}
	}
}
