Tileset = Tileset or { __index = Tileset }

function Tileset.create(tex, w, h, aw, ah)
  local obj = {}

  obj.ent = ent.create()
  obj.width = w
  obj.height = h
  obj.aperture_width = aw
  obj.aperture_height = ah
  obj.texture_path = tex
  obj.index = 0

  obj.ent:set_uv_size({ aw / w, ah / h })
  obj.ent:set_texture_path(tex)
  obj.ent:set_active(true)

  setmetatable(obj, Tileset)
  Tileset.__index = Tileset

  return obj
end

function Tileset:set_index(idx, opt)
  if opt then
    self.ent:set_uv_offset({
      self.aperture_width / self.width * idx,
      self.aperture_height / self.height * opt,
    })
  else
    local row_size = self.width / self.aperture_width

    self.ent:set_uv_offset({
      self.aperture_width / self.width * math.floor(idx % row_size),
      self.aperture_height / self.height * math.floor(idx / row_size),
    })
  end
end
