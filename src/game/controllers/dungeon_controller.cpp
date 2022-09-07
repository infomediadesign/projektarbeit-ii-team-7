#include "dungeon_controller.h"

void DungeonController::init(GameState *state) {}

void DungeonController::update(GameState *state, mutex_t *lock) {}

void DungeonController::update_lazy(GameState *state, mutex_t *lock) {}

void DungeonController::update_paused(GameState *state, mutex_t *lock) {}

void DungeonController::update_renderables(
  GameState *state, mutex_t *lock, RenderState *render_state, Renderable **renderables, const u32 renderables_count
) {}

void DungeonController::destroy(GameState *state) {}

void DungeonController::process_input(GameState *state, const f64 update_time) {}

void DungeonController::check_collision(std::shared_ptr<Entity> ent)
{
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
