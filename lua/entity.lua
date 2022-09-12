function valid(o)
  if (type(o) == 'table' or type(o) == 'userdata') and o.is_valid then
    return o:is_valid()
  end

  return false
end

function ent.all()
  return ENTS
end

function ent.find_by_id(id)
  for _, v in ipairs(ENTS) do
    if v:get_entity_index() == id then
      return v
    end
  end
end

function ent.find_by_class(cn)
  local ents = {}

  for _, v in ipairs(ENTS) do
    if v:get_ent_class() == cn then
      table.insert(ents, v)
    end
  end

  return ents
end
