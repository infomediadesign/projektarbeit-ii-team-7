#include "player.h"

#define PLY_ANIM_DELAY    0.1
#define PLY_SPRITE_WIDTH  4.0f
#define PLY_SPRITE_HEIGHT 7.0f
#define PLY_ANIM_ROW(name, n)                                                  \
  Vector2 *name##_frames = new Vector2[4];                                     \
  name##_frames[0]       = { 0.0f, n / PLY_SPRITE_HEIGHT };                    \
  name##_frames[1]       = { 1.0f / PLY_SPRITE_WIDTH, n / PLY_SPRITE_HEIGHT }; \
  name##_frames[2]       = { 2.0f / PLY_SPRITE_WIDTH, n / PLY_SPRITE_HEIGHT }; \
  name##_frames[3]       = { 3.0f / PLY_SPRITE_WIDTH, n / PLY_SPRITE_HEIGHT };

std::shared_ptr<Player> Player::make(std::shared_ptr<Entity> base) {
  std::shared_ptr<Player> ply = std::make_shared<Player>(base);

  ply->get_base()->set_texture_path("assets/animated/main_character.png");
  ply->get_base()->set_uv_size({ 1.0f / PLY_SPRITE_WIDTH, 1.0f / PLY_SPRITE_HEIGHT });

  PLY_ANIM_ROW(idle_front, 0.0f)
  PLY_ANIM_ROW(idle_back, 1.0f)
  PLY_ANIM_ROW(idle_left, 2.0f)
  PLY_ANIM_ROW(idle_right, 3.0f)
  PLY_ANIM_ROW(walk_front, 4.0f)
  PLY_ANIM_ROW(walk_back, 5.0f)
  PLY_ANIM_ROW(walk_left, 6.0f)
  PLY_ANIM_ROW(walk_right, 6.0f)

  Animation *anims = new Animation[8];

  anims[PLY_ANIM_IDLE_FRONT] = Animation::make(idle_front_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_IDLE_BACK]  = Animation::make(idle_back_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_IDLE_LEFT]  = Animation::make(idle_left_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_IDLE_RIGHT] = Animation::make(idle_right_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_WALK_FRONT] = Animation::make(walk_front_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_WALK_BACK]  = Animation::make(walk_back_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_WALK_LEFT]  = Animation::make(walk_left_frames, 4, PLY_ANIM_DELAY, true);
  anims[PLY_ANIM_WALK_RIGHT] = Animation::make(walk_right_frames, 4, PLY_ANIM_DELAY, true);

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
