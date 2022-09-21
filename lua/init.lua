-- A lazy way to get the root folder of Lua stuff.
-- The `require` function is relative to cwd, not file. Which is kind of stupid.
LUA_PATH = string.gsub(INIT_FILE, '/init%.lua$', '')
CURRENT_FILE_DIRECTORY = LUA_PATH

function get_dir(path)
  if string.find(path, '/') then
    return string.gsub(string.gsub(path, '^(.+)/[%w]+$', '%1'), '^(.+)/[%w]+%.lua$', '%1')
  else
    return ''
  end
end

function file_exists(name)
  local f = io.open(name, "r")

  if f ~= nil then
    io.close(f)

    return true
  else
    return false
  end
end

function string.ends_with(str, ending)
  return string.sub(str, -string.len(ending)) == ending
end

-- Override the require function to use relative paths!
function include(fn)
  local orig = fn
  local old_dir = CURRENT_FILE_DIRECTORY

  if not string.ends_with(fn, '.lua') then
    fn = fn..'.lua'
  end

  if not file_exists(fn) then
    fn = CURRENT_FILE_DIRECTORY..'/'..fn

    if not file_exists(fn) then
      error('Cannot include file '..orig..' (file does not exist)')
    end
  end

  CURRENT_FILE_DIRECTORY = string.gsub(fn, '/[%w+_]+%.lua$', '')

  local s, err = pcall(dofile, fn)

  if not s then
    print('Lua error in file '..fn..':')
    print(err)
  end

  CURRENT_FILE_DIRECTORY = old_dir
end

include 'table_utils'
include 'math_utils'
include 'enums'
include 'event'
include 'entity'
include 'game/game'

if not LUA_STARTED then
  print 'Lua boot complete'
else
  print 'Lua refreshed'
end

LUA_STARTED = true
