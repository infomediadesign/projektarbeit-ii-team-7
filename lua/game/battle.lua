local battle_gui = BATTLE_GUI or {cmds = {}}
BATTLE_GUI = battle_gui

local center = { 0, -0.4375, 0 }
local ui_width = 2
local ui_height = 1.125
local ui_left = -1
local ui_right = 1
local ui_top = -0.5625
local ui_bottom = 0.5625
local opponents = {
  test = function()
    local e = ent.create()
    e:set_ent_class(ENTCLASS_ENEMY)
    e:set_texture_path('assets/debug/foxy_64x64.png')
    e:set_pos({0, -0.5, 0})
    e:set_scale({4, 4})
    e:set_active(true)

    CURRENT_OPPONENT = {
      ent = e,
      limbs = {
        {
          name = "Body",
          health = 5
        }
      }
    }
  end
}
local commands = {}

local function px_to_pos(x, y)
  return {x / 1280 * ui_width - 1, y / 720 * ui_height - 1, 0}
end

local function react_to(cmd, val)
  if val and not battle_gui.cmds[cmd] then
    battle_gui.cmds[cmd] = val

    return true
  elseif not val then
    battle_gui.cmds[cmd] = val
  end

  return false
end

local function cmd_register(cmd, cb)
  commands[cmd] = cb
end

function GAME:battle_setup_opponents()
  if not NEXT_OPPONENT or type(opponents[NEXT_OPPONENT]) ~= "function" then
    game.setstage(GS_OVERWORLD)
    return false
  end

  opponents[NEXT_OPPONENT]()
  
  NEXT_OPPONENT = nil
end

function GAME:battle_update()
  if not CURRENT_OPPONENT or not valid(CURRENT_OPPONENT.ent) or CURRENT_OPPONENT.ent:get_ent_class() ~= ENTCLASS_ENEMY then
    return
  end

  local active_cmds = self:get_active_commands()

  for cmd, val in pairs(active_cmds) do
    if react_to(cmd, val) and commands[cmd] then
      commands[cmd]()
    end
  end
end

function GAME:battle_setup_gui()
  local hp = player.get_data('health')
  local max_hp = player.get_data('max_health')
  local ap = player.get_data('ap')
  local max_ap = player.get_data('max_ap')

  battle_gui.main = ent.create()
  battle_gui.main:set_ent_class(ENTCLASS_GUI)
  battle_gui.main:set_texture_path('assets/ui/battleSTD_GUI.png')
  battle_gui.main:set_scale({ 20, 11.25 })
  battle_gui.main:set_pos(center)
  battle_gui.main:set_active(true)

  battle_gui.go = ent.create()
  battle_gui.go:set_ent_class(ENTCLASS_GUI)
  battle_gui.go:set_texture_path('assets/ui/GO_button_disabled.png')
  battle_gui.go:set_scale({ 2.5, 2.5 })
  battle_gui.go:set_pos({ 0.5, -0.07, 0 })
  battle_gui.go:set_active(true)

  battle_gui.hp_back_first = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
  battle_gui.hp_back_first:set_index(4, 0)
  battle_gui.hp_back_first.ent:set_pos(px_to_pos(48, 680))

  battle_gui.hp_back_last = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
  battle_gui.hp_back_last:set_index(2, 0)
  battle_gui.hp_back_last.ent:set_pos(px_to_pos(48, 680 - 32 * max_hp))

  battle_gui.hp_bars = {}
  battle_gui.hp_bars_back = {}

  for i = 1, max_hp - 1 do
    battle_gui.hp_bars_back[i] = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
    battle_gui.hp_bars_back[i]:set_index(3, 0)
    battle_gui.hp_bars_back[i].ent:set_pos(px_to_pos(48, 680 - 32 * i))
  end

  if hp > 0 then
    for i = 1, hp do
      battle_gui.hp_bars[i] = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
      battle_gui.hp_bars[i].ent:set_pos(px_to_pos(48, 680 - 32 * i))
    end
  end

  battle_gui.ap_back_first = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
  battle_gui.ap_back_first:set_index(4, 1)
  battle_gui.ap_back_first.ent:set_pos(px_to_pos(128, 680))

  battle_gui.ap_back_last = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
  battle_gui.ap_back_last:set_index(2, 1)
  battle_gui.ap_back_last.ent:set_pos(px_to_pos(128, 680 - 32 * max_ap))

  battle_gui.ap_bars = {}
  battle_gui.ap_bars_back = {}

  for i = 1, max_ap - 1 do
    battle_gui.ap_bars_back[i] = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
    battle_gui.ap_bars_back[i]:set_index(3, 1)
    battle_gui.ap_bars_back[i].ent:set_pos(px_to_pos(128, 680 - 32 * i))
  end

  if ap > 0 then
    for i = 1, ap do
      battle_gui.ap_bars[i] = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
      battle_gui.ap_bars[i]:set_index(0, 1)
      battle_gui.ap_bars[i].ent:set_pos(px_to_pos(128, 680 - 32 * i))
    end
  end

  battle_gui.current_pos = 1
  battle_gui.current_limb = 1
end

cmd_register(CMD_USE, function()
  if battle_gui.current_pos == 1 then
    battle_gui.current_pos = 0
  elseif battle_gui.current_pos == 0 then
    -- attack code here
  elseif battle_gui.current_pos == 2 then
    -- inventory code here
  end
end)

cmd_register(CMD_FORWARD, function()
  if battle_gui.current_pos == 2 then
    battle_gui.current_pos = 1
  end
end)

cmd_register(CMD_BACK, function()
  if battle_gui.current_pos == 1 then
    battle_gui.current_pos = 2
  end
end)

cmd_register(CMD_RIGHT, function()
  if battle_gui.current_pos == 1 then
    local total_limbs = table.length(CURRENT_OPPONENT.limbs)

    if total_limbs == 1 or battle_gui.current_limb >= total_limbs then
      battle_gui.current_pos = 0
    elseif battle_gui.current_limb < total_limbs then
      battle_gui.current_limb = battle_gui.current_limb + 1
    end
  else
    battle_gui.current_pos = 0
  end
end)

cmd_register(CMD_LEFT, function()
  if battle_gui.current_pos == 0 then
    battle_gui.current_pos = 1
  elseif battle_gui.current_pos == 1 then
    if battle_gui.current_limb > 1 then
      battle_gui.current_limb = battle_gui.current_limb - 1
    end
  end
end)
