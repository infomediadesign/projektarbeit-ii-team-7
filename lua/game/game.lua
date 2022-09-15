require 'bit'
require 'battle'

local MOUSE_MOVE = 14
local REFRESH_CMD = 15
local DEBUG_CHANGELEVEL_CMD = 16
local active_commands = ACTIVE_COMMANDS or {}
ACTIVE_COMMANDS = active_commands

CAN_MOVE = true

-- function GAME:post_init()
--   for i = 1, 8192 do
--     local e = ent.create()
--     e:set_pos({i * 0.1, 0.1, 0})
--     e:set_texture_path('assets/debug/wall_32x32.png')
--     e:set_active(true)
--   end
-- end

function GAME:ent_create(e)
  table.insert(ENTS, e)
end

function GAME:ent_removed(e)
  local ent_index = e:get_entity_index()

  for k, v in ipairs(ENTS) do
    if v:get_entity_index() == ent_index then
      table.remove(ENTS, k)
      break
    end
  end
end

function GAME:get_active_commands()
  return active_commands
end

function GAME:create_bindings()
  game.bind(bit.bor(KEY_MOUSE1, KEY_PRESS), MOUSE_MOVE)
  game.bind(bit.bor(KEY_MOUSE1, KEY_RELEASE), -MOUSE_MOVE)
  game.bind(bit.bor(KEY_F5, KEY_PRESS), REFRESH_CMD)
  game.bind(bit.bor(KEY_F10, KEY_PRESS), DEBUG_CHANGELEVEL_CMD)
end

function GAME:process_input(cmds, input_state)
  for _, v in ipairs(cmds) do
    active_commands[math.abs(v)] = v > 0

    if v == REFRESH_CMD then
      for k, v in pairs(package.loaded) do
        if k ~= 'bit' then
          package.loaded[k] = false
        end
      end

      require '../init'

      return
    elseif v == DEBUG_CHANGELEVEL_CMD then
      NEXT_OPPONENT = 'test'
      game.setstage(GS_BATTLE)
    end
  end

  if not CAN_MOVE then return false end

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

function GAME:post_set_stage()
  CAN_MOVE = game.getstage() == GS_OVERWORLD or game.getstage() == GS_DUNGEON
end
