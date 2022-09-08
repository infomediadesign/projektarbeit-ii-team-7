require 'bit'

local CUSTOM_LUA_COMMAND = 14

-- function GAME:post_init()
--   for i = 1, 8192 do
--     local e = ent.create()
--     e:set_pos({i * 0.1, 0.1, 0})
--     e:set_texture_path('assets/debug/wall_32x32.png')
--     e:set_active(true)
--   end
-- end

function GAME:create_bindings()
  game.bind(bit.bor(GAMEPAD_A, KEY_PRESS), CUSTOM_LUA_COMMAND)
  game.bind(bit.bor(GAMEPAD_A, KEY_RELEASE), -CUSTOM_LUA_COMMAND)
end

function GAME:process_input(cmds, input_state)
  local ply = game.player()

  if valid(ply) then
    if math.abs(input_state.left_stick.x) > 0.1 then
      ply:set_velocity_x(input_state.left_stick.x * 0.5)
    elseif math.abs(input_state.left_stick_last.x) > 0.1 then
      ply:set_velocity_x(0)
    end

    if math.abs(input_state.left_stick.y) > 0.1 then
      ply:set_velocity_y(input_state.left_stick.y * 0.5)
    elseif math.abs(input_state.left_stick_last.y) > 0.1 then
      ply:set_velocity_y(0)
    end
  end
end