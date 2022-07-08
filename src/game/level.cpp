#include "level.h"

#include <engine/input/asset.h>


void Level::load_json(std::vector<std::shared_ptr<Entity>>* entities, const std::string path) {
	char real_path[256];

	asset_find(path.c_str(), real_path);

	simdjson::padded_string json = simdjson::padded_string::load(real_path);

	this->json_data = this->json_parser.iterate(json);

	simdjson::ondemand::object json_data_obj(this->json_data.get_object());
	u32 json_data_size = json_data_obj.count_fields();
	std::string layer_id;

	for (u32 i = 0; i < (json_data_size - 14); i++) {
		if (i == 0)
			layer_id = "/layers";
		else
			layer_id = "/layers" + std::to_string(i);

		simdjson::ondemand::array layers = this->json_data.at_pointer(layer_id).get_array();

		simdjson::ondemand::array background_array = layers.at_pointer("/0/chunks").get_array();

		u32 bg_array_size = background_array.count_elements();

		for (u32 i = 0; i < bg_array_size; i++) {

			std::string json_pointer = layer_id + "/0/chunks/" + std::to_string(i);

			simdjson::ondemand::object obj(this->json_data.at_pointer(json_pointer).get_object());

			double x = obj["x"];
			double y = obj["y"];

			std::vector<u32> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (double value : arr)
				values.push_back(value);


			this->background_content.emplace_back(x, y, std::move(values));
		}

		simdjson::ondemand::array objects_array = this->json_data.at_pointer(layer_id + "/1/objects").get_array();
		u32 obj_array_size = objects_array.count_elements();

		for (u32 i = 0; i < obj_array_size; i++) {

			std::string json_pointer = layer_id + "/1/objects/" + std::to_string(i);

			simdjson::ondemand::object obj(this->json_data.at_pointer(json_pointer).get_object());

			double x = obj["x"];
			double y = obj["y"];
			double width = obj["width"];
			double height = obj["height"];
			double tileset_id = obj["id"];

			simdjson::ondemand::array property_array = this->json_data.at_pointer(json_pointer + "/properties").get_array();
			u32 prop_array_size = property_array.count_elements();
			std::string_view script = "0";

			for (u32 i = 0; i < prop_array_size; i++) {

				simdjson::ondemand::object obj(property_array.at_pointer("/" + std::to_string(i)).get_object());

				script = obj["value"];
			}

			objects_content.emplace_back(x, y, width, height, tileset_id, script);
		}

		simdjson::ondemand::array collisions_array = this->json_data.at_pointer(layer_id + "/2/chunks").get_array();

		u32 co_array_size = collisions_array.count_elements();

		for (u32 i = 0; i < co_array_size; i++) {

			std::string json_pointer = layer_id + "/2/chunks/" + std::to_string(i);

			simdjson::ondemand::object obj(this->json_data.at_pointer(json_pointer).get_object());

			double x = obj["x"];
			double y = obj["y"];

			std::vector<double> values;

			simdjson::ondemand::array arr((obj["data"].get_array()));

			for (double value : arr)
				values.push_back(value);

			collisions_content.emplace_back(x, y, std::move(values));
		}
	}

	simdjson::ondemand::array tilesets = this->json_data["tilesets"];
	u32 tilesets_size = tilesets.count_elements();


	for (int i = 0; i < tilesets_size; i++) {

		std::string json_pointer = "/tilesets/" + std::to_string(i);
		std::string_view tileset_id;

		simdjson::ondemand::object obj(json_data.at_pointer(json_pointer).get_object());
		std::string_view tileset = obj["source"];



		tileset_id_jsons.emplace_back(tileset);
	}

	for (u32 i = 0; i < tileset_id_jsons.size(); i++) {
		std::cout << tileset_id_jsons[i] << std::endl;
	}


	for (u32 i = 0; i < tileset_id_jsons.size(); i++) {
		std::string tileset = tileset_id_jsons.at(i);
		simdjson::padded_string id_json = simdjson::padded_string::load(tileset);

		this->id_json_data = json_parser.iterate(id_json);

		simdjson::ondemand::object id_obj = id_json_data;
		std::string_view tileset_id = id_obj["image"];

		tileset_id_list.emplace_back(tileset_id);
	}
}

void Level::update() {
	for (const auto& arr : background_content) {
		u32 x_counter = 0;
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
				ent->set_pos(vector_make3(x * 0.1 + x_counter * 0.1, y * 0.1 + y_counter * 0.1, 0.0f));
				ent->set_active(true);
			}
			x_counter++;

			if ((i + 1) % 16 == 0) {
				x_counter = 0;
				y_counter++;
			}
			else if ((i + 1) % 256 == 0) {
				y_counter = 0;
			}
		}
	}


	for (const auto& arr : objects_content) {
		u32 x = arr.x;
		u32 y = arr.y;
		u32 height = arr.height;
		u32 width = arr.width;
		u32 id = arr.tileset_id;
		std::string script = arr.script;

	}

	for (const auto& arr : collisions_content) {
		u32 x_counter = 0;
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
				ent->set_pos(vector_make3(x * 0.1 + x_counter * 0.1, y * 0.1 + y_counter * 0.1, 0.0f));
				ent->set_active(true);
			}

			x_counter++;

			if ((i + 1) % 16 == 0) {
				x_counter = 0;
				y_counter++;
			}
			else if ((i + 1) % 256 == 0) {
				y_counter = 0;
			}
		}
	}
}
