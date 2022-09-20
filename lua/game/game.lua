require 'bit'
include 'tileset_ent'
include 'player'
include 'battle'

local MOUSE_MOVE = 14
local REFRESH_CMD = 15
local DEBUG_CHANGELEVEL_CMD = 16
local active_commands = ACTIVE_COMMANDS or {}
ACTIVE_COMMANDS = active_commands

CAN_MOVE = true

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

function math.round(n)
  return math.floor(n * 1000) * 0.001
end

local function get_collision_ent(ply)
  for _, v in ipairs(ent.find_by_class(ENTCLASS_CLIP)) do
    if ply:collides_with(v) then
      return v
    end
  end
end

local stuck_since = nil

local function unstuck_player(ply)
  local vel = ply:get_velocity()

  if vel[1] ~= 0 or vel[2] ~= 0 then return false end

  local e = get_collision_ent(ply)

  if not e then
    stuck_since = nil

    return false
  end

  if not stuck_since then
    stuck_since = platform.time()

    return false
  elseif platform.time() - stuck_since < 0.1 then
    return false
  end

  local orig_pos = ply:get_pos()
  local ent_pos = e:get_pos()
  local diff = { -(ent_pos[1] - orig_pos[1]), -(ent_pos[2] - orig_pos[2]), ent_pos[3] - orig_pos[3] }

  for i = 1, 50 do
    ply:set_pos(
      util.lerp(0.05 * i, orig_pos[1], orig_pos[1] + diff[1]),
      util.lerp(0.05 * i, orig_pos[2], orig_pos[2] + diff[2]),
      orig_pos[3]
    )

    if not ply:collides_with(e) then
      return
    end
  end

  ply:set_pos(orig_pos)

  return true
end

local function aabb_to_absolute(ent)
  local pos = ent:get_pos()
  local scale = ent:get_scale()
  local target_aabb_min = ent:get_aabb_min()
  local target_aabb_max = ent:get_aabb_max()

  return { pos[1] + target_aabb_min[1] * scale[1], pos[2] + target_aabb_min[2] * scale[2], target_aabb_min[3] },
         { pos[1] + target_aabb_max[1] * scale[1], pos[2] + target_aabb_max[2] * scale[2], target_aabb_max[3] }
end

local function collides_with(aabb_min, aabb_max, ent)
  local target_min, target_max = aabb_to_absolute(ent)

  if math.round(aabb_min[1]) - math.round(target_max[1]) > 0.001 or math.round(aabb_max[1]) - math.round(target_min[1]) < 0.001 then
    return false
  end

  if math.round(aabb_min[2]) - math.round(target_max[2]) > 0.001 or math.round(aabb_max[2]) - math.round(target_min[2]) < 0.001 then
    return false
  end

  return true
end

local function vec_distance_sqr(v1, v2)
  return (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2])
end

local clip_ents = {}

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
    ply:set_velocity({0, 0, 0})

    if active_commands[CMD_FORWARD] then
      ply:set_velocity_y(-0.5)
    end

    if active_commands[CMD_BACK] then
      ply:set_velocity_y(0.5)
    end

    if active_commands[CMD_LEFT] then
      ply:set_velocity_x(-0.5)
    end

    if active_commands[CMD_RIGHT] then
      ply:set_velocity_x(0.5)
    end

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

    local vel = ply:get_velocity()
    local pos = ply:get_pos()
    local aabb_min, aabb_max = aabb_to_absolute(ply)
    local aabb_min_orig, aabb_max_orig = aabb_to_absolute(ply)

    aabb_min[1] = aabb_min[1] + vel[1] * 0.01
    aabb_min[2] = aabb_min[2] + vel[2] * 0.01
    aabb_max[1] = aabb_max[1] + vel[1] * 0.01
    aabb_max[2] = aabb_max[2] + vel[2] * 0.01

    for _, v in ipairs(clip_ents) do
      if collides_with(aabb_min, aabb_max, v) then
        local clip_pos = v:get_pos()
        local diff = { clip_pos[1] - pos[1], clip_pos[2] - pos[2], clip_pos[3] - pos[3] }

        if math.abs(diff[1]) < math.abs(diff[2]) then
          ply:set_velocity_y(0)

          aabb_min[2] = aabb_min_orig[2]
          aabb_max[2] = aabb_max_orig[2]
        else
          ply:set_velocity_x(0)

          aabb_min[1] = aabb_min_orig[1]
          aabb_max[1] = aabb_max_orig[1]
        end
      end
    end
  end

  return false
end

function GAME:post_set_stage()
  CAN_MOVE = game.getstage() == GS_OVERWORLD or game.getstage() == GS_DUNGEON
end

function GAME:update()
  local ply = game.player()

  if valid(ply) then
    unstuck_player(ply)
  end
end

function GAME:update_lazy()
  local ply = game.player()

  if not valid(ply) then return end

  clip_ents = {}

  local ply_pos = ply:get_pos()

  for k, v in ipairs(ent.find_by_class(ENTCLASS_CLIP)) do
    if vec_distance_sqr(ply_pos, v:get_pos()) < 1 then
      table.insert(clip_ents, v)
    end
  end

  table.sort(clip_ents, function(a, b)
    return vec_distance_sqr(ply_pos, a:get_pos()) < vec_distance_sqr(ply_pos, b:get_pos())
  end)
end
