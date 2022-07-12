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

			std::vector<u32> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (i64 value : arr)
				values.push_back(value);

			this->background.emplace_back(obj["x"], obj["y"], std::move(values));
		}

		simdjson::ondemand::array objects_array = this->json_data.at_pointer(layer_id + "/1/objects").get_array();

		for (u32 i = 0; i < objects_array.count_elements(); i++) {

			simdjson::ondemand::object obj(this->json_data.at_pointer(layer_id + "/1/objects/" + std::to_string(i)).get_object());

			simdjson::ondemand::array property_array =
				this->json_data.at_pointer(layer_id + "/1/objects/" + std::to_string(i) + "/properties").get_array();
			std::string_view script = "0";

			for (u32 i = 0; i < property_array.count_elements(); i++) {

				simdjson::ondemand::object obj(property_array.at_pointer("/" + std::to_string(i)).get_object());

				script = obj["value"];
			}
			objects.emplace_back(obj["x"], obj["y"], obj["width"], obj["height"], obj["id"], script);
		}

		simdjson::ondemand::array collisions_array = this->json_data.at_pointer(layer_id + "/2/chunks").get_array();

		for (u32 i = 0; i < collisions_array.count_elements(); i++) {

			simdjson::ondemand::object obj(this->json_data.at_pointer(layer_id + "/2/chunks/" + std::to_string(i)).get_object());

			std::vector<i64> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (i64 value : arr) {
				values.push_back(value);
			}
			collisions.emplace_back(obj["x"], obj["y"], std::move(values));
		}
	}

	simdjson::ondemand::array tilesets = this->json_data["tilesets"];

	for (u32 i = 0; i < tilesets.count_elements(); i++) {

		simdjson::ondemand::object obj(json_data.at_pointer("/tilesets/" + std::to_string(i)).get_object());

		tileset_id_json.emplace_back(std::string_view(obj["source"]));
	}

	for (u32 i = 0; i < tileset_id_json.size(); i++) {
		std::string tileset = tileset_id_json.at(i);
		simdjson::padded_string id_json = simdjson::padded_string::load(tileset);

		this->id_json_data = json_parser.iterate(id_json);

		simdjson::ondemand::object id_obj = id_json_data;

		tileset_id_list.emplace_back(id_obj["image"]);
	}
}

void Level::init() {
	for (const Background arr : background) {
		u32 y_counter = 0;

		u32 x = arr.x;
		u32 y = arr.y;

		for (u32 i = 0; i < arr.background_data.size(); i++) {
			u32 data = arr.background_data.at(i);

			if (data > 0) {
				std::string id = tileset_id_list.at(data - 1);
				std::shared_ptr<Entity> ent = this->ent_manager->ent_create();
				ent->set_ent_class(EntClass::BACKGROUND);
				ent->set_texture_path(id);
				ent->set_pos(vector_make3(x * 0.1 + (i % 16) * 0.1, y * 0.1 + y_counter * 0.1, 0.0f));
				ent->set_active(true);
			}

			if ((i + 1) % 16 == 0) {
				y_counter++;
			}

			if ((i + 1) % 256 == 0) {
				y_counter = 0;
			}
		}
	}

	for (const Object arr : objects) {
		u32 x = arr.x;
		u32 y = arr.y;
		u32 height = arr.height;
		u32 width = arr.width;
		u32 id = arr.tileset_id;
		std::string script = arr.script;
	}

	for (const Collision arr : collisions) {
		u32 y_counter = 0;

		for (u32 i = 0; i < arr.collisions_data.size(); i++) {
			u32 x = arr.x;
			u32 y = arr.y;
			u32 data = arr.collisions_data.at(i);

			if (data > 0) {
				std::string id = tileset_id_list.at(data - 1);
				std::shared_ptr<Entity> ent = this->ent_manager->ent_create();
				ent->set_ent_class(EntClass::BACKGROUND);
				ent->set_texture_path(id);
				ent->set_pos(vector_make3(x * 0.1 + ((i + 1) % 16) * 0.1, y * 0.1 + y_counter * 0.1, 0.0f));
				ent->set_active(true);
			}

			if ((i + 1) % 16 == 0) {
				y_counter++;
			}

			if ((i + 1) % 256 == 0) {
				y_counter = 0;
			}
		}
	}
}

void Level::update() {
}
