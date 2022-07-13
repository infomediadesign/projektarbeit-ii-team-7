#include "level.h"
#include <engine/input/asset.h>


void Level::load_json(std::vector<std::shared_ptr<Entity>>* entities, const std::string path) {
	char real_path[256];

	asset_find(path.c_str(), real_path);

	simdjson::padded_string json = simdjson::padded_string::load(real_path);

	this->json_data = this->json_parser.iterate(json);

	simdjson::ondemand::object json_data_obj(this->json_data.get_object());
	std::string layer_id;

	for (u32 i = 0; i < (json_data_obj.count_fields() - 14); i++) {
		if (i == 0)
			layer_id = "/layers";
		else
			layer_id = "/layers" + std::to_string(i);

		simdjson::ondemand::array layers = this->json_data.at_pointer(layer_id).get_array();
		simdjson::ondemand::array background_array = layers.at_pointer("/0/chunks").get_array();

		for (u32 i = 0; i < background_array.count_elements(); i++) {

			simdjson::ondemand::object obj(this->json_data.at_pointer(layer_id + "/0/chunks/" + std::to_string(i)).get_object());

			std::vector<i64> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (i64 value : arr)
				values.push_back(value);

			background.emplace_back(std::move(values), static_cast<f64>(obj["x"]) * 0.1 + ((i + 1) % 16) * 0.1, 
				static_cast<f64>(obj["y"]) * 0.1 + ((i + 1) / 16) * 0.1);
		}

		simdjson::ondemand::array objects_array = this->json_data.at_pointer(layer_id + "/1/objects").get_array();

		for (u32 i = 0; i < objects_array.count_elements(); i++) {

			simdjson::ondemand::object obj(this->json_data.at_pointer(layer_id + "/1/objects/" + std::to_string(i)).get_object());

			simdjson::ondemand::array property_array =
				this->json_data.at_pointer(layer_id + "/1/objects/" + std::to_string(i) + "/properties").get_array();
			std::string_view script = "0";

			for (u32 i = 0; i < property_array.count_elements(); i++) {

				simdjson::ondemand::object inner_obj(property_array.at_pointer("/" + std::to_string(i)).get_object());

				objects.emplace_back(inner_obj["value"], static_cast<f64>(obj["x"]) * 0.1 + ((i + 1) % 16) * 0.1,
					static_cast<f64>(obj["y"]) * 0.1 + ((i + 1) / 16) * 0.1, obj["width"], obj["height"], obj["id"]);
			}
		}

		simdjson::ondemand::array collisions_array = this->json_data.at_pointer(layer_id + "/2/chunks").get_array();

		for (u32 i = 0; i < collisions_array.count_elements(); i++) {

			simdjson::ondemand::object obj(this->json_data.at_pointer(layer_id + "/2/chunks/" + std::to_string(i)).get_object());

			std::vector<i64> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (i64 value : arr) {
				values.push_back(value);
			}
			collisions.emplace_back(std::move(values), static_cast<f64>(obj["x"]) * 0.1 + ((i + 1) % 16) * 0.1,
				static_cast<f64>(obj["y"]) * 0.1 + ((i + 1) / 16) * 0.1);
		}
	}

	simdjson::ondemand::array tilesets = this->json_data["tilesets"];

	for (u32 i = 0; i < tilesets.count_elements(); i++) {

		simdjson::ondemand::object obj(json_data.at_pointer("/tilesets/" + std::to_string(i)).get_object());
		tileset_id_json.emplace_back(static_cast<std::string_view>(obj["source"]));
	}

	for (u32 i = 0; i < tileset_id_json.size(); i++) {
		std::string tileset = tileset_id_json.at(i);
		simdjson::padded_string id_json = simdjson::padded_string::load(tileset);

		this->id_json_data = json_parser.iterate(id_json);

		simdjson::ondemand::object id_obj = id_json_data;

		tileset_id_list.emplace_back(static_cast<std::string_view>(id_obj["image"]));
	}
}

void Level::level_init() {
	for (const BackgroundJSON arr : background) {
		u32 x = arr.x;
		u32 y = arr.y;

		for (u32 i = 0; i < arr.background_data.size(); i++) {
			u32 data = arr.background_data.at(i);

			if (data > 0) {
				std::shared_ptr<Entity> ent = this->ent_manager->ent_create();
				ent->set_ent_class(EntClass::BACKGROUND);
				ent->set_texture_path(tileset_id_list.at(data - 1));
				ent->set_pos(vector_make3(x, y, 0.0f));
				ent->set_active(true);
			}
		}
	}

	for (const ObjectJSON arr : objects) {
		u32 x = arr.x;
		u32 y = arr.y;
		u32 height = arr.height;
		u32 width = arr.width;
		u32 id = arr.tileset_id;
		std::string script = arr.script;
	}

	for (const CollisionJSON arr : collisions) {

		for (u32 i = 0; i < arr.collision_data.size(); i++) {
			u32 x = arr.x;
			u32 y = arr.y;
			u32 data = arr.collision_data.at(i);

			if (data > 0) {
				std::shared_ptr<Entity> ent = this->ent_manager->ent_create();
				ent->set_ent_class(EntClass::BACKGROUND);
				ent->set_texture_path(tileset_id_list.at(data - 1));
				ent->set_pos(vector_make3(x , y, 0.0f));
				ent->set_active(true);
			}
		}
	}
}

void Level::update() {
}
