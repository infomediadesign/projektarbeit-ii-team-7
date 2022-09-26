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
      tile = nil,
      name = "FOXY FOXINGTON",
      status = "idle",
      status_messages = {
        idle = "BLOCKS THE WAY",
        attack = "BITES YOU",
        defend = "HIDES BEHIND TAIL",
        pain = "YIPS PAINFULLY",
        near_death = "WHIMPERS",
        death = "IS DEFEATED"
      },
      limbs = {
        {
          name = "Body",
          health = 5
        }
      }
    }
  end,
  bandit = function()
    local e = Tileset.create('assets/animated/battle_enemy_rat.png', 2128, 120, 304, 120)
    e.ent:set_ent_class(ENTCLASS_ENEMY)
    e.ent:set_pos({0, -0.5, 0})
    e.ent:set_scale({12, 4.5})

    CURRENT_OPPONENT = {
      ent = e.ent,
      tile = e,
      animated = true,
      anim_max = 7,
      name = "THE BANDIT",
      status = "idle",
      status_messages = {
        idle = "BLOCKS THE WAY",
        attack = "SLASHES AT YOU",
        weak_attack = "PUNCHES YOU",
        defend = "HIDES BEHIND GREATSHIELD",
        pain = "CURSES!",
        near_death = "SEEMS EXHAUSTED",
        death = "IS DEFEATED"
      },
      limbs = {
        {
          name = "Body",
          health = 10
        },
        {
          name = "Sword",
          health = 2 
        },
        {
          name = "Shield",
          health = 25
        }
      },
      attack = function(self)
        if self.limbs[1].health <= 0 then
          return
        end

        if self.limbs[2].health > 0 then
          -- attack
          self.status = "attack"
          self.attack_damage = 2
          self.defense = 0
        elseif self.limbs[3].health > 0 then
          -- defend or weak attack
          if math.random(1, 2) == 1 then
            self.status = "weak_attack"
            self.attack_damage = 1
            self.defense = 0
          else
            self.status = "defend"
            self.attack_damage = 0
            self.defense = 2
          end
        else
          self.status = "weak_attack"
          self.attack_damage = 1
          self.defense = 0
        end
      end,
      stance = function(self)
        if self.limbs[1].health <= 0 then
          self.status = "death"
        elseif self.limbs[1].health <= 5 then
          self.status = "near_death"
        elseif self.limbs[1].health <= 9 then
          self.status = "pain"
        else
          self.status = "idle"
        end
      end
    }
  end
}
local commands = {}
local col_green = {x = 0.2, y = 0.9, z = 0.2, w = 1.0}
local col_black = {x = 0.1, y = 0.2, z = 0.1, w = 1.0}
local col_red = {x = 0.9, y = 0.2, z = 0.2, w = 1.0}
local col_blue = {x = 0.2, y = 0.3, z = 0.85, w = 1.0}
local won_since = 0

local function px_to_pos(x, y)
  return {x / 1280 * ui_width - 1, y / 720 * ui_height - 1, 0}
end

local function px_to_pos4(x, y)
  return {x = x / 1280 * ui_width - 1, y = y / 720 * ui_height - 1, z = 0, w = 1}
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

local function rebuild_text()
  game.cleartext()

  if battle_gui.current_stage == 1 then
    battle_gui.attack_text  = game.addtext("ATTACK", px_to_pos4(270, 580), col_green, {x = 0.64, y = 0.64})
    battle_gui.defend_text  = game.addtext("DEFEND", px_to_pos4(580, 580), col_green, {x = 0.64, y = 0.64})
    battle_gui.item_text    = game.addtext("ITEM", px_to_pos4(270, 650), col_green, {x = 0.64, y = 0.64})
    battle_gui.retreat_text = game.addtext("RETREAT", px_to_pos4(580, 650), col_green, {x = 0.64, y = 0.64})
  elseif battle_gui.current_stage == 2 then
    local limbs_count = table.length(CURRENT_OPPONENT.limbs)
    local scale = 1 - (limbs_count - 2) * 0.3

    battle_gui.limb_label = game.addtext("CHOOSE BODY PART", px_to_pos4(270, 560), col_green, {x = 0.45, y = 0.55})
    battle_gui.limbs = {}

    for k, v in ipairs(CURRENT_OPPONENT.limbs) do
      local t = game.addtext(v.name, px_to_pos4(270 + (k - 1) * 260 * scale, 620), col_green, {x = 0.5 * scale, y = 0.6 * scale })

      table.insert(battle_gui.limbs, t)
    end
  else
    battle_gui.error_label = game.addtext("ERROR!", px_to_pos4(270, 580), col_red, {x = 0.8, y = 0.8})
    battle_gui.error_text = game.addtext("Please restart the game.", px_to_pos4(270, 630), col_red, {x = 0.35, y = 0.45})
  end

  if battle_gui.chain and table.length(battle_gui.chain) > 0 then
    battle_gui.chain_texts = {}

    for k, v in ipairs(battle_gui.chain) do
      if v[1] == 'attack' then
        game.addtext('ATK '..CURRENT_OPPONENT.limbs[v[2]].name, px_to_pos4(1000, 128 + 48 * (k - 1)), col_red, {x = 0.5, y = 0.75})
      else
        game.addtext('DEF', px_to_pos4(1000, 128 + 48 * (k - 1)), col_blue, {x = 0.5, y = 0.75})
      end
    end
  end

  battle_gui.log_text = game.addtext(CURRENT_OPPONENT.name.." "..CURRENT_OPPONENT.status_messages[CURRENT_OPPONENT.status], px_to_pos4(300, 32), col_green, {x = 0.3, y = 0.3})
end

local function recolor_text()
  if battle_gui.current_stage == 1 then
    local ap = player.get_data('ap')

    battle_gui.attack_text:set_color(col_green)
    battle_gui.defend_text:set_color(col_green)
    battle_gui.item_text:set_color(col_green)
    battle_gui.retreat_text:set_color(col_green)

    if battle_gui.current_pos == 1 then
      battle_gui.attack_text:set_color(col_black)
    elseif battle_gui.current_pos == 2 then
      battle_gui.defend_text:set_color(col_black)
    elseif battle_gui.current_pos == 3 then
      battle_gui.item_text:set_color(col_black)
    elseif battle_gui.current_pos == 4 then
      battle_gui.retreat_text:set_color(col_black)
    end

    if ap <= 0 then
      battle_gui.attack_text:set_color(col_red)
      battle_gui.defend_text:set_color(col_red)
    end
  elseif battle_gui.current_stage == 2 then
    for k, v in ipairs(battle_gui.limbs) do
      if CURRENT_OPPONENT.limbs[k].health <= 0 then
        v:set_color(col_red)
      elseif battle_gui.current_pos == 1 and battle_gui.current_limb == k then
        v:set_color(col_black)
      else
        v:set_color(col_green)
      end
    end
  end
end

local function update_hp_ap()
  local hp = player.get_data('health')
  local max_hp = player.get_data('max_health')
  local ap = player.get_data('ap')
  local max_ap = player.get_data('max_ap')

  for i = 1, max_ap do
    if i > ap then
      battle_gui.ap_bars[i].ent:set_visible(false)
    else
      battle_gui.ap_bars[i].ent:set_visible(true)
    end
  end

  for i = 1, max_hp do
    if i > hp then
      battle_gui.hp_bars[i].ent:set_visible(false)
    else
      battle_gui.hp_bars[i].ent:set_visible(true)
    end
  end
end

function GAME:battle_setup_opponents()
  if not NEXT_OPPONENT or type(opponents[NEXT_OPPONENT]) ~= "function" then
    game.setstage(GS_OVERWORLD)
    return false
  end

  opponents[NEXT_OPPONENT]()

  battle_gui = {cmds = {}}

  player.set_data('health', 5)
  player.set_data('ap', 2)
  
  NEXT_OPPONENT = nil
end

local next_frame_at = 0
local current_frame = 0

function GAME:battle_update()
  if not CURRENT_OPPONENT or not valid(CURRENT_OPPONENT.ent) or CURRENT_OPPONENT.ent:get_ent_class() ~= ENTCLASS_ENEMY then
    return
  end

  if player.get_data('health') <= 0 then
    if won_since == 0 then
      won_since = platform.time()

      game.cleartext()

      local e = ent.create()
      e:set_ent_class(ENTCLASS_GUI)
      e:set_pos(center)
      e:set_scale({ 20, 11.25 })
      e:set_texture_path('assets/ui/background_black.png')
      e:set_active(true)

      e = ent.create()
      e:set_ent_class(ENTCLASS_GUI)
      e:set_pos(center)
      e:set_scale({ 20 * 0.5, 11.25 * 0.5 })
      e:set_texture_path('assets/ui/game_over.png')
      e:set_active(true)
    elseif won_since + 5 < platform.time() then
      game.setstage(GS_OVERWORLD)
    end

    return
  end

  if CURRENT_OPPONENT.status == 'death' then
    if won_since == 0 then
      won_since = platform.time()
    elseif won_since + 3 < platform.time() then
      game.setstage(GS_OVERWORLD)
    end

    return
  end

  local active_cmds = self:get_active_commands()

  for cmd, val in pairs(active_cmds) do
    if react_to(cmd, val) and commands[cmd] then
      commands[cmd]()
    end
  end

  if CURRENT_OPPONENT.animated and next_frame_at <= platform.time() then
    CURRENT_OPPONENT.tile:set_index(current_frame)

    current_frame = current_frame + 1

    if current_frame >= CURRENT_OPPONENT.anim_max then
      current_frame = 0
    end

    next_frame_at = platform.time() + 0.1
  end

  if battle_gui.attack_ok then
    battle_gui.go:set_texture_path('assets/ui/GO_button.png')
  end

  if battle_gui.current_pos == 0 then
    battle_gui.selector:set_visible(true)
    battle_gui.selector:set_scale({ 2.5, 2.5 })
    battle_gui.selector:set_pos({ 0.5, -0.07, 0 })
  elseif battle_gui.current_pos == 100 then
    battle_gui.selector:set_visible(true)
    battle_gui.selector:set_scale({ 1.75, 1.75 })
    battle_gui.selector:set_pos(px_to_pos(1226, 642))  
  else
    battle_gui.selector:set_visible(false)
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

  battle_gui.back_btn = Tileset.create('assets/ui/back_button.png', 128, 32, 32, 32)
  battle_gui.back_btn.ent:set_scale({ 1.75, 1.75 })
  battle_gui.back_btn.ent:set_pos(px_to_pos(1226, 642))
  battle_gui.back_btn.ent:set_visible(false)

  battle_gui.selector = ent.create()
  battle_gui.selector:set_ent_class(ENTCLASS_GUI)
  battle_gui.selector:set_texture_path('assets/ui/cursor_hover.png')
  battle_gui.selector:set_scale({ 2.5, 2.5 })
  battle_gui.selector:set_pos({ 0.5, -0.07, 0 })
  battle_gui.selector:set_visible(false)
  battle_gui.selector:set_active(true)

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
    for i = 1, max_hp do
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
    for i = 1, max_ap do
      battle_gui.ap_bars[i] = Tileset.create('assets/ui/HP_AP_bar.png', 80, 32, 16, 16)
      battle_gui.ap_bars[i]:set_index(0, 1)
      battle_gui.ap_bars[i].ent:set_pos(px_to_pos(128, 680 - 32 * i))
    end
  end

  battle_gui.current_pos = 1
  battle_gui.current_limb = 1
  battle_gui.current_stage = 1
  battle_gui.chain = {}

  rebuild_text()
  recolor_text()
  update_hp_ap()
end

cmd_register(CMD_USE, function()
  local current_ap = player.get_data('ap')

  if battle_gui.current_stage == 1 and current_ap > 0 then
    if battle_gui.current_pos == 1 then
      battle_gui.current_stage = 2
      battle_gui.current_pos = 1
      battle_gui.current_limb = 1

      battle_gui.back_btn.ent:set_visible(true)

      rebuild_text()
      recolor_text()

      return
    elseif battle_gui.current_pos == 2 and current_ap > 0 then
      table.insert(battle_gui.chain, {'defend'})
      player.set_data('ap', 2 - table.length(battle_gui.chain))

      update_hp_ap()
      rebuild_text()
      recolor_text()

      return
    elseif battle_gui.current_pos == 4 then
      game.setstage(GS_OVERWORLD)

      return
    end
  elseif battle_gui.current_stage == 2 then
    if battle_gui.current_pos == 1 and current_ap > 0 then
      battle_gui.attack_ok = true
      battle_gui.current_pos = 0

      table.insert(battle_gui.chain, {'attack', battle_gui.current_limb})

      player.set_data('ap', 2 - table.length(battle_gui.chain))

      update_hp_ap()
      rebuild_text()
      recolor_text()

      return
    elseif battle_gui.current_pos == 2 and current_ap > 0 then

    elseif battle_gui.current_pos == 100 then
      battle_gui.current_stage = 1
      battle_gui.current_pos = 1
      battle_gui.current_limb = 1

      battle_gui.back_btn.ent:set_visible(false)

      rebuild_text()
      recolor_text()

      return
    end
  end

  if battle_gui.current_pos == 0 and table.length(battle_gui.chain) > 0 then
    CURRENT_OPPONENT:attack()

    for k, v in ipairs(battle_gui.chain) do
      if v[1] == 'attack' then
        CURRENT_OPPONENT.limbs[v[2]].health = CURRENT_OPPONENT.limbs[v[2]].health - 3
      elseif v[1] == 'defend' then
        CURRENT_OPPONENT.attack_damage = math.clamp(CURRENT_OPPONENT.attack_damage - 1, 0, 100)
      end
    end

    player.set_data('health', (player.get_data('health') or 5) - (CURRENT_OPPONENT.attack_damage or 0))
    player.set_data('ap', player.get_data('max_ap'))

    battle_gui.chain = {}

    rebuild_text()
    recolor_text()
    update_hp_ap()
  end
end)

cmd_register(CMD_FORWARD, function()
  if battle_gui.current_stage == 1 then
    if battle_gui.current_pos > 2 then
      battle_gui.current_pos = battle_gui.current_pos - 2
    end
  elseif battle_gui.current_stage == 2 then
    if battle_gui.current_pos == 2 then
      battle_gui.current_pos = 1
    end
  end

  CURRENT_OPPONENT:stance()
  rebuild_text()
  recolor_text()
end)

cmd_register(CMD_BACK, function()
  if battle_gui.current_stage == 1 then
    if battle_gui.current_pos <= 2 then
      battle_gui.current_pos = battle_gui.current_pos + 2
    end
  elseif battle_gui.current_stage == 2 then
    if battle_gui.current_pos == 1 then
      battle_gui.current_pos = 2
    end
  end

  CURRENT_OPPONENT:stance()
  rebuild_text()
  recolor_text()
end)

cmd_register(CMD_RIGHT, function()
  if battle_gui.current_stage == 1 then
    if battle_gui.current_pos % 2 == 1 then
      battle_gui.current_pos = battle_gui.current_pos + 1
    elseif battle_gui.current_pos % 2 == 0 then
      battle_gui.current_pos = 0
    end
  elseif battle_gui.current_stage == 2 then
    if battle_gui.current_pos == 1 then
      local total_limbs = table.length(CURRENT_OPPONENT.limbs)

      if total_limbs == 1 or battle_gui.current_limb >= total_limbs then
        battle_gui.current_pos = 0
      elseif battle_gui.current_limb < total_limbs then
        battle_gui.current_limb = battle_gui.current_limb + 1
      end
    elseif battle_gui.current_pos == 0 then
      battle_gui.current_pos = 100
    elseif battle_gui.current_pos ~= 100 then
      battle_gui.current_pos = 0
    end
  end

  CURRENT_OPPONENT:stance()
  rebuild_text()
  recolor_text()
end)

cmd_register(CMD_LEFT, function()
  if battle_gui.current_stage == 1 then
    if battle_gui.current_pos > 0 and battle_gui.current_pos % 2 == 0 then
      battle_gui.current_pos = battle_gui.current_pos - 1
    elseif battle_gui.current_pos == 0 then
      battle_gui.current_pos = 4
    end
  elseif battle_gui.current_stage == 2 then
    if battle_gui.current_pos == 0 then
      battle_gui.current_pos = 1
    elseif battle_gui.current_pos == 1 then
      if battle_gui.current_limb > 1 then
        battle_gui.current_limb = battle_gui.current_limb - 1
      end
    elseif battle_gui.current_pos == 100 then
      battle_gui.current_pos = 0
    end
  end

  CURRENT_OPPONENT:stance()
  rebuild_text()
  recolor_text()
end)
