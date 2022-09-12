local opponents = {
  test = function()
    local e = ent.create()
    e:set_ent_class(ENTCLASS_ENEMY)
    e:set_texture_path('assets/debug/foxy_64x64.png')
    e:set_pos({0, -0.23, 0})

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
end
