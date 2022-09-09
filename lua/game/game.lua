require 'bit'

local MOUSE_MOVE = 14
local active_commands = {}

-- function GAME:post_init()
--   for i = 1, 8192 do
--     local e = ent.create()
--     e:set_pos({i * 0.1, 0.1, 0})
--     e:set_texture_path('assets/debug/wall_32x32.png')
--     e:set_active(true)
--   end
-- end

function GAME:get_active_commands()
  return active_commands
end

function GAME:create_bindings()
  game.bind(bit.bor(KEY_MOUSE1, KEY_PRESS), MOUSE_MOVE)
  game.bind(bit.bor(KEY_MOUSE1, KEY_RELEASE), -MOUSE_MOVE)
end

function GAME:process_input(cmds, input_state)
  for _, v in ipairs(cmds) do
    active_commands[math.abs(v)] = v > 0
  end

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

    if active_commands[MOUSE_MOVE] then
      local ww = window.getwidth()
      local wh = window.getheight()

      ply:set_velocity_x((input_state.mouse.x - ww * 0.5) / ww)
      ply:set_velocity_y((input_state.mouse.y - wh * 0.5) / wh)
    elseif table.hasvaluei(cmds, -MOUSE_MOVE) then
      ply:set_velocity_x(0)
      ply:set_velocity_y(0)
    end
  end
end
