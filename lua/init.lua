-- A lazy way to get the root folder of Lua stuff.
-- The `require` function is relative to cwd, not file. Which is kind of stupid.
LUA_PATH = string.gsub(INIT_FILE, '/init%.lua$', '')
OLD_REQUIRE = OLD_REQUIRE or require
CURRENT_FILE_DIRECTORY = LUA_PATH

function get_dir(path)
  if string.find(path, '/') then
    return string.gsub(string.gsub(path, '^(.+)/[%w]+$', '%1'), '^(.+)/[%w]+%.lua$', '%1')
  else
    return ''
  end
end

-- Override the require function to use relative paths!
function require(mod)
  local old_dir = CURRENT_FILE_DIRECTORY
  local s, _err = pcall(OLD_REQUIRE, mod)

  if not s then
    local new_dir = get_dir(mod)

    if new_dir ~= '' then
      CURRENT_FILE_DIRECTORY = CURRENT_FILE_DIRECTORY..'/'..new_dir
    end

    local s, err = pcall(OLD_REQUIRE, old_dir..'/'..mod)

    if not s then
      print('Cannot include file \''..mod..'\'\n'..err)
    end
  end

  CURRENT_FILE_DIRECTORY = old_dir
end

require 'table_utils'
require 'enums'
require 'event'

print 'Lua system initialized'

local e = ent.create()

print 'entity test:'
print(e:get_active())
print(e:get_entity_index())
print(e:get_lifetime())

local old_lifetime = e:set_lifetime(9999)

printtable(e:get_scale())

e:set_scale(10, 20)

printtable(e:get_scale())

e:set_scale({20, 10})

printtable(e:get_scale())

print('old lifetime: '..old_lifetime..', new lifetime: '..e:get_lifetime())

event.handler('pre_set_stage', 'stage_handler', function(old, new)
  print('old stage: '..old..'\nnew stage: '..new)
end)

event.handler('post_set_stage', 'battle_initiator', function()
  if game.getstage() ~= GS_BATTLE then
    game.setstage(GS_BATTLE)
  end
end)