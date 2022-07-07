#include "player.h"

std::shared_ptr<Player> Player::make(std::shared_ptr<Entity> base) {
  std::shared_ptr<Player> ply = std::make_shared<Player>(base);

  ply->get_base()->set_texture_path("assets/animated/main_character.png");
  ply->get_base()->set_uv_size({ 1.0f / 6.0f, 1.0f / 5.0f });

  Vector2 *idle_front_frames = new Vector2[4];
  idle_front_frames[0]       = { 0.0f, 0.0f };
  idle_front_frames[1]       = { 1.0f / 6.0f, 0.0f };
  idle_front_frames[2]       = { 2.0f / 6.0f, 0.0f };
  idle_front_frames[3]       = { 3.0f / 6.0f, 0.0f };

  Vector2 *idle_back_frames = new Vector2[4];
  idle_back_frames[0]       = { 4.0f / 6.0f, 0.0f };
  idle_back_frames[1]       = { 5.0f / 6.0f, 0.0f };
  idle_back_frames[2]       = { 0.0f / 6.0f, 1.0f / 5.0f };
  idle_back_frames[3]       = { 1.0f / 6.0f, 1.0f / 5.0f };

  Vector2 *idle_left_frames = new Vector2[4];
  idle_left_frames[0]       = { 0.0f / 6.0f, 3.0f / 5.0f };
  idle_left_frames[1]       = { 1.0f / 6.0f, 3.0f / 5.0f };
  idle_left_frames[2]       = { 2.0f / 6.0f, 3.0f / 5.0f };
  idle_left_frames[3]       = { 3.0f / 6.0f, 3.0f / 5.0f };

  Vector2 *idle_right_frames = new Vector2[4];
  idle_right_frames[0]       = { 2.0f / 6.0f, 2.0f / 5.0f };
  idle_right_frames[1]       = { 3.0f / 6.0f, 2.0f / 5.0f };
  idle_right_frames[2]       = { 4.0f / 6.0f, 2.0f / 5.0f };
  idle_right_frames[3]       = { 5.0f / 6.0f, 2.0f / 5.0f };

  Vector2 *walk_front_frames = new Vector2[3];
  walk_front_frames[0]       = { 2.0f / 6.0f, 1.0f / 5.0f };
  walk_front_frames[1]       = { 3.0f / 6.0f, 1.0f / 5.0f };
  walk_front_frames[2]       = { 4.0f / 6.0f, 1.0f / 5.0f };

  Vector2 *walk_back_frames = new Vector2[3];
  walk_back_frames[0]       = { 5.0f / 6.0f, 1.0f / 5.0f };
  walk_back_frames[1]       = { 0.0f / 6.0f, 2.0f / 5.0f };
  walk_back_frames[2]       = { 1.0f / 6.0f, 2.0f / 5.0f };

  Vector2 *walk_left_frames = new Vector2[3];
  walk_left_frames[0]       = { 1.0f / 6.0f, 4.0f / 5.0f };
  walk_left_frames[1]       = { 2.0f / 6.0f, 4.0f / 5.0f };
  walk_left_frames[2]       = { 3.0f / 6.0f, 4.0f / 5.0f };

  Vector2 *walk_right_frames = new Vector2[3];
  walk_right_frames[0]       = { 4.0f / 6.0f, 3.0f / 5.0f };
  walk_right_frames[1]       = { 5.0f / 6.0f, 3.0f / 5.0f };
  walk_right_frames[2]       = { 0.0f / 6.0f, 4.0f / 5.0f };

  Animation *anims = new Animation[8];

  anims[PLY_ANIM_IDLE_FRONT] = Animation::make(idle_front_frames, 4, 0.2, true);
  anims[PLY_ANIM_IDLE_BACK]  = Animation::make(idle_back_frames, 4, 0.2, true);
  anims[PLY_ANIM_IDLE_LEFT]  = Animation::make(idle_left_frames, 4, 0.2, true);
  anims[PLY_ANIM_IDLE_RIGHT] = Animation::make(idle_right_frames, 4, 0.2, true);
  anims[PLY_ANIM_WALK_FRONT] = Animation::make(walk_front_frames, 3, 0.2, true);
  anims[PLY_ANIM_WALK_BACK]  = Animation::make(walk_back_frames, 3, 0.2, true);
  anims[PLY_ANIM_WALK_LEFT]  = Animation::make(walk_left_frames, 3, 0.2, true);
  anims[PLY_ANIM_WALK_RIGHT] = Animation::make(walk_right_frames, 3, 0.2, true);

  ply->get_base()->set_anims(anims);
  ply->get_base()->set_animated(true);
  ply->get_base()->set_current_anim(PLY_ANIM_IDLE_FRONT);

  return ply;
}

void Player::update_anim() {
  const Vector3 vel = this->base->get_velocity();

  if (vel.x > 0.1f)
    this->base->set_current_anim(PLY_ANIM_WALK_RIGHT);
  else if (vel.x < -0.1f)
    this->base->set_current_anim(PLY_ANIM_WALK_LEFT);
  else if (vel.y < -0.1f)
    this->base->set_current_anim(PLY_ANIM_WALK_BACK);
  else if (vel.y > 0.1f)
    this->base->set_current_anim(PLY_ANIM_WALK_FRONT);

  if (this->base->get_current_anim() >= PLY_ANIM_WALK_FRONT && vector_length3(vel) <= 0.1f)
    this->base->set_current_anim(this->base->get_current_anim() - PLY_ANIM_WALK_FRONT);
}
