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
  if val and not battle_ui.cmds[cmd] then
    battle_ui.cmds[cmd] = val

    return true
  elseif not val then
    battle_ui.cmds[cmd] = val
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
end

cmd_register(CMD_FORWARD, function()

end)
