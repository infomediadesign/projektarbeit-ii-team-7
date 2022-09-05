require 'bit'

local CUSTOM_LUA_COMMAND = 14

function GAME:create_bindings()
  game.bind(bit.bor(KEY_F, KEY_PRESS), CUSTOM_LUA_COMMAND)
  game.bind(bit.bor(KEY_F, KEY_RELEASE), -CUSTOM_LUA_COMMAND)
end

function GAME:process_input(cmds)
  for _k, cmd in ipairs(cmds) do
    if cmd == CUSTOM_LUA_COMMAND then
      print 'you pressed F! congratulations!'
    elseif cmd == -CUSTOM_LUA_COMMAND then
      print 'and you released it, good job'
    end
  end
end
