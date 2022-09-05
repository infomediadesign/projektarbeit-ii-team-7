module('event', package.seeall)

local storage = EVENT_STORAGE or {}
EVENT_STORAGE = EVENT_STORAGE or storage

function event.handler(id, handler_id, callback)
  storage[id] = storage[id] or {}

  storage[id][handler_id] = callback
end

function event.run(id, ...)
  if storage[id] then
    for handler_id, callback in pairs(storage[id]) do
      local success, a, b, c, d, e, f = pcall(callback, ...)

      if success and a ~= nil then
        return a, b, c, d, e, f
      elseif not success then
        print('Handler "'..handler_id..'" for event "'..id..'" has failed to run! ('..a..')')
      end
    end
  end

  if GAME and GAME[id] then
    local success, a, b, c, d, e, f = pcall(GAME[id], GAME, ...)

    if success then
      return a, b, c, d, e, f
    elseif not success then
      print('Event "'..id..'" has failed to run! ('..a..')')
    end
  end
end
