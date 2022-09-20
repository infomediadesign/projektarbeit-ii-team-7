module('player', package.seeall)

local pdata = PLAYER_DATA or {
  health = 5,
  max_health = 5,
  ap = 2,
  max_ap = 2,
  inventory = {},
  current_weapon = nil
}
PLAYER_DATA = pdata

function player.get_data(key)
  return pdata[key]
end

function player.set_data(key, value)
  local old = pdata[key]

  pdata[key] = value

  return old, pdata[key]
end
