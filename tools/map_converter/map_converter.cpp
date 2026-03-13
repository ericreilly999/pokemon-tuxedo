// map_converter.cpp
// Implementation of format converter for Pokemon Crossroads (Emerald) to FireRed map data

#include "map_converter.h"
#include "../mapjson/json11.h"
#include <fstream>
#include <sstream>
#include <cstring>

using json11::Json;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;

// Helper function to safely copy strings
static void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// Helper function to read file contents
static string read_file(const char* filepath, char* error_buffer, size_t error_size) {
    ifstream file(filepath);
    if (!file.is_open()) {
        snprintf(error_buffer, error_size, "Failed to open file: %s", filepath);
        return "";
    }
    
    ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Helper function to write file contents
static bool write_file(const char* filepath, const string& content, 
                       char* error_buffer, size_t error_size) {
    ofstream file(filepath);
    if (!file.is_open()) {
        snprintf(error_buffer, error_size, "Failed to open file for writing: %s", filepath);
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

// Helper to get string from JSON with default
static string json_get_string(const Json& obj, const string& key, const string& default_val = "") {
    if (obj[key].is_string()) {
        return obj[key].string_value();
    } else if (obj[key].is_number()) {
        return std::to_string(obj[key].int_value());
    } else if (obj[key].is_bool()) {
        return obj[key].bool_value() ? "TRUE" : "FALSE";
    }
    return default_val;
}

// Helper to get bool from JSON with default
static bool json_get_bool(const Json& obj, const string& key, bool default_val = false) {
    if (obj[key].is_bool()) {
        return obj[key].bool_value();
    } else if (obj[key].is_string()) {
        string val = obj[key].string_value();
        return (val == "TRUE" || val == "true" || val == "1");
    }
    return default_val;
}

// Helper to get int from JSON with default
static int json_get_int(const Json& obj, const string& key, int default_val = 0) {
    if (obj[key].is_number()) {
        return obj[key].int_value();
    }
    return default_val;
}

void InitMapData(MapData* data) {
    memset(data, 0, sizeof(MapData));
    data->requires_flash = false;
    data->allow_cycling = true;
    data->allow_escaping = false;
    data->allow_running = true;
    data->show_map_name = true;
    data->floor_number = 0;
    data->connections_no_include = false;
    safe_strcpy(data->weather, "WEATHER_SUNNY", sizeof(data->weather));
    safe_strcpy(data->map_type, "MAP_TYPE_TOWN", sizeof(data->map_type));
    safe_strcpy(data->battle_scene, "MAP_BATTLE_SCENE_NORMAL", sizeof(data->battle_scene));
}

bool ParseEmeraldMap(const char* emerald_path, MapData* out_data, 
                     char* error_buffer, size_t error_size) {
    if (!emerald_path || !out_data || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, "Invalid parameters to ParseEmeraldMap");
        }
        return false;
    }
    
    // Initialize output data
    InitMapData(out_data);
    
    // Read file
    string json_content = read_file(emerald_path, error_buffer, error_size);
    if (json_content.empty()) {
        return false;
    }
    
    // Parse JSON
    string parse_error;
    Json map_json = Json::parse(json_content, parse_error);
    
    if (!parse_error.empty()) {
        snprintf(error_buffer, error_size, "JSON parse error in %s: %s", 
                 emerald_path, parse_error.c_str());
        return false;
    }
    
    // Extract basic fields
    safe_strcpy(out_data->id, json_get_string(map_json, "id").c_str(), sizeof(out_data->id));
    safe_strcpy(out_data->name, json_get_string(map_json, "name").c_str(), sizeof(out_data->name));
    safe_strcpy(out_data->layout, json_get_string(map_json, "layout").c_str(), sizeof(out_data->layout));
    safe_strcpy(out_data->music, json_get_string(map_json, "music").c_str(), sizeof(out_data->music));
    safe_strcpy(out_data->region_map_section, json_get_string(map_json, "region_map_section").c_str(), 
                sizeof(out_data->region_map_section));
    safe_strcpy(out_data->weather, json_get_string(map_json, "weather", "WEATHER_SUNNY").c_str(), 
                sizeof(out_data->weather));
    safe_strcpy(out_data->map_type, json_get_string(map_json, "map_type", "MAP_TYPE_TOWN").c_str(), 
                sizeof(out_data->map_type));
    safe_strcpy(out_data->battle_scene, json_get_string(map_json, "battle_scene", "MAP_BATTLE_SCENE_NORMAL").c_str(), 
                sizeof(out_data->battle_scene));
    
    // Extract boolean fields
    out_data->requires_flash = json_get_bool(map_json, "requires_flash", false);
    out_data->allow_cycling = json_get_bool(map_json, "allow_cycling", true);
    out_data->allow_escaping = json_get_bool(map_json, "allow_escaping", false);
    out_data->allow_running = json_get_bool(map_json, "allow_running", true);
    out_data->show_map_name = json_get_bool(map_json, "show_map_name", true);
    
    // Extract numeric fields
    out_data->floor_number = json_get_int(map_json, "floor_number", 0);
    
    // Extract optional fields
    if (map_json["shared_events_map"].is_string()) {
        safe_strcpy(out_data->shared_events_map, 
                   json_get_string(map_json, "shared_events_map").c_str(), 
                   sizeof(out_data->shared_events_map));
    }
    if (map_json["shared_scripts_map"].is_string()) {
        safe_strcpy(out_data->shared_scripts_map, 
                   json_get_string(map_json, "shared_scripts_map").c_str(), 
                   sizeof(out_data->shared_scripts_map));
    }
    out_data->connections_no_include = json_get_bool(map_json, "connections_no_include", false);
    
    // Parse connections
    if (map_json["connections"].is_array()) {
        auto connections = map_json["connections"].array_items();
        out_data->connection_count = (connections.size() > MAX_CONNECTIONS) ? 
                                      MAX_CONNECTIONS : connections.size();
        for (size_t i = 0; i < out_data->connection_count; i++) {
            safe_strcpy(out_data->connections[i].direction, 
                       json_get_string(connections[i], "direction").c_str(), 
                       sizeof(out_data->connections[i].direction));
            out_data->connections[i].offset = json_get_int(connections[i], "offset", 0);
            safe_strcpy(out_data->connections[i].map, 
                       json_get_string(connections[i], "map").c_str(), 
                       sizeof(out_data->connections[i].map));
        }
    }
    
    // Parse warp events
    if (map_json["warp_events"].is_array()) {
        auto warps = map_json["warp_events"].array_items();
        out_data->warp_count = (warps.size() > MAX_WARPS) ? MAX_WARPS : warps.size();
        for (size_t i = 0; i < out_data->warp_count; i++) {
            out_data->warps[i].x = json_get_int(warps[i], "x", 0);
            out_data->warps[i].y = json_get_int(warps[i], "y", 0);
            out_data->warps[i].elevation = json_get_int(warps[i], "elevation", 0);
            out_data->warps[i].dest_warp_id = json_get_int(warps[i], "dest_warp_id", 0);
            safe_strcpy(out_data->warps[i].dest_map, 
                       json_get_string(warps[i], "dest_map").c_str(), 
                       sizeof(out_data->warps[i].dest_map));
        }
    }
    
    // Parse object events
    if (map_json["object_events"].is_array()) {
        auto objects = map_json["object_events"].array_items();
        out_data->object_count = (objects.size() > MAX_OBJECTS) ? MAX_OBJECTS : objects.size();
        for (size_t i = 0; i < out_data->object_count; i++) {
            string type = json_get_string(objects[i], "type", "object");
            safe_strcpy(out_data->objects[i].type, type.c_str(), sizeof(out_data->objects[i].type));
            
            out_data->objects[i].graphics_id = json_get_int(objects[i], "graphics_id", 0);
            out_data->objects[i].x = json_get_int(objects[i], "x", 0);
            out_data->objects[i].y = json_get_int(objects[i], "y", 0);
            out_data->objects[i].elevation = json_get_int(objects[i], "elevation", 0);
            
            if (type == "object" || type.empty()) {
                safe_strcpy(out_data->objects[i].movement_type, 
                           json_get_string(objects[i], "movement_type").c_str(), 
                           sizeof(out_data->objects[i].movement_type));
                out_data->objects[i].movement_range_x = json_get_int(objects[i], "movement_range_x", 0);
                out_data->objects[i].movement_range_y = json_get_int(objects[i], "movement_range_y", 0);
                safe_strcpy(out_data->objects[i].trainer_type, 
                           json_get_string(objects[i], "trainer_type").c_str(), 
                           sizeof(out_data->objects[i].trainer_type));
                safe_strcpy(out_data->objects[i].trainer_sight_or_berry_tree_id, 
                           json_get_string(objects[i], "trainer_sight_or_berry_tree_id").c_str(), 
                           sizeof(out_data->objects[i].trainer_sight_or_berry_tree_id));
                safe_strcpy(out_data->objects[i].script, 
                           json_get_string(objects[i], "script").c_str(), 
                           sizeof(out_data->objects[i].script));
                safe_strcpy(out_data->objects[i].flag, 
                           json_get_string(objects[i], "flag").c_str(), 
                           sizeof(out_data->objects[i].flag));
            } else if (type == "clone") {
                out_data->objects[i].target_local_id = json_get_int(objects[i], "target_local_id", 0);
                safe_strcpy(out_data->objects[i].target_map, 
                           json_get_string(objects[i], "target_map").c_str(), 
                           sizeof(out_data->objects[i].target_map));
            }
            
            if (objects[i]["local_id"].is_string()) {
                safe_strcpy(out_data->objects[i].local_id, 
                           json_get_string(objects[i], "local_id").c_str(), 
                           sizeof(out_data->objects[i].local_id));
            }
        }
    }
    
    // Parse coord events
    if (map_json["coord_events"].is_array()) {
        auto coords = map_json["coord_events"].array_items();
        out_data->coord_event_count = (coords.size() > MAX_COORD_EVENTS) ? 
                                       MAX_COORD_EVENTS : coords.size();
        for (size_t i = 0; i < out_data->coord_event_count; i++) {
            string type = json_get_string(coords[i], "type", "trigger");
            safe_strcpy(out_data->coord_events[i].type, type.c_str(), 
                       sizeof(out_data->coord_events[i].type));
            
            out_data->coord_events[i].x = json_get_int(coords[i], "x", 0);
            out_data->coord_events[i].y = json_get_int(coords[i], "y", 0);
            out_data->coord_events[i].elevation = json_get_int(coords[i], "elevation", 0);
            
            if (type == "trigger") {
                safe_strcpy(out_data->coord_events[i].var, 
                           json_get_string(coords[i], "var").c_str(), 
                           sizeof(out_data->coord_events[i].var));
                safe_strcpy(out_data->coord_events[i].var_value, 
                           json_get_string(coords[i], "var_value").c_str(), 
                           sizeof(out_data->coord_events[i].var_value));
                safe_strcpy(out_data->coord_events[i].script, 
                           json_get_string(coords[i], "script").c_str(), 
                           sizeof(out_data->coord_events[i].script));
            } else if (type == "weather") {
                safe_strcpy(out_data->coord_events[i].weather, 
                           json_get_string(coords[i], "weather").c_str(), 
                           sizeof(out_data->coord_events[i].weather));
            }
        }
    }
    
    // Parse bg events
    if (map_json["bg_events"].is_array()) {
        auto bgs = map_json["bg_events"].array_items();
        out_data->bg_event_count = (bgs.size() > MAX_BG_EVENTS) ? MAX_BG_EVENTS : bgs.size();
        for (size_t i = 0; i < out_data->bg_event_count; i++) {
            string type = json_get_string(bgs[i], "type", "sign");
            safe_strcpy(out_data->bg_events[i].type, type.c_str(), 
                       sizeof(out_data->bg_events[i].type));
            
            out_data->bg_events[i].x = json_get_int(bgs[i], "x", 0);
            out_data->bg_events[i].y = json_get_int(bgs[i], "y", 0);
            out_data->bg_events[i].elevation = json_get_int(bgs[i], "elevation", 0);
            
            if (type == "sign") {
                safe_strcpy(out_data->bg_events[i].player_facing_dir, 
                           json_get_string(bgs[i], "player_facing_dir").c_str(), 
                           sizeof(out_data->bg_events[i].player_facing_dir));
                safe_strcpy(out_data->bg_events[i].script, 
                           json_get_string(bgs[i], "script").c_str(), 
                           sizeof(out_data->bg_events[i].script));
            } else if (type == "hidden_item") {
                safe_strcpy(out_data->bg_events[i].item, 
                           json_get_string(bgs[i], "item").c_str(), 
                           sizeof(out_data->bg_events[i].item));
                safe_strcpy(out_data->bg_events[i].flag, 
                           json_get_string(bgs[i], "flag").c_str(), 
                           sizeof(out_data->bg_events[i].flag));
                out_data->bg_events[i].quantity = json_get_int(bgs[i], "quantity", 1);
                safe_strcpy(out_data->bg_events[i].underfoot, 
                           json_get_string(bgs[i], "underfoot", "FALSE").c_str(), 
                           sizeof(out_data->bg_events[i].underfoot));
            } else if (type == "secret_base") {
                safe_strcpy(out_data->bg_events[i].secret_base_id, 
                           json_get_string(bgs[i], "secret_base_id").c_str(), 
                           sizeof(out_data->bg_events[i].secret_base_id));
            }
        }
    }
    
    return true;
}

bool FormatFireRedMap(const MapData* data, const char* output_path, 
                      char* error_buffer, size_t error_size) {
    if (!data || !output_path || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, "Invalid parameters to FormatFireRedMap");
        }
        return false;
    }
    
    // Build JSON using ostringstream
    ostringstream json;
    json << "{\n";
    json << "  \"id\": \"" << data->id << "\",\n";
    json << "  \"name\": \"" << data->name << "\",\n";
    json << "  \"layout\": \"" << data->layout << "\",\n";
    json << "  \"music\": \"" << data->music << "\",\n";
    json << "  \"region_map_section\": \"" << data->region_map_section << "\",\n";
    json << "  \"requires_flash\": " << (data->requires_flash ? "true" : "false") << ",\n";
    json << "  \"weather\": \"" << data->weather << "\",\n";
    json << "  \"map_type\": \"" << data->map_type << "\",\n";
    json << "  \"allow_cycling\": " << (data->allow_cycling ? "true" : "false") << ",\n";
    json << "  \"allow_escaping\": " << (data->allow_escaping ? "true" : "false") << ",\n";
    json << "  \"allow_running\": " << (data->allow_running ? "true" : "false") << ",\n";
    json << "  \"show_map_name\": " << (data->show_map_name ? "true" : "false") << ",\n";
    json << "  \"floor_number\": " << data->floor_number << ",\n";
    json << "  \"battle_scene\": \"" << data->battle_scene << "\",\n";
    
    // Add connections
    json << "  \"connections\": [";
    for (uint16_t i = 0; i < data->connection_count; i++) {
        if (i > 0) json << ",";
        json << "\n    {\n";
        json << "      \"direction\": \"" << data->connections[i].direction << "\",\n";
        json << "      \"offset\": " << data->connections[i].offset << ",\n";
        json << "      \"map\": \"" << data->connections[i].map << "\"\n";
        json << "    }";
    }
    if (data->connection_count > 0) json << "\n  ";
    json << "],\n";
    
    // Add object_events
    json << "  \"object_events\": [";
    for (uint16_t i = 0; i < data->object_count; i++) {
        if (i > 0) json << ",";
        json << "\n    {\n";
        
        string type = data->objects[i].type;
        if (!type.empty() && type != "object") {
            json << "      \"type\": \"" << type << "\",\n";
        }
        
        json << "      \"graphics_id\": " << data->objects[i].graphics_id << ",\n";
        json << "      \"x\": " << data->objects[i].x << ",\n";
        json << "      \"y\": " << data->objects[i].y << ",\n";
        json << "      \"elevation\": " << data->objects[i].elevation;
        
        if (type == "clone") {
            json << ",\n";
            json << "      \"target_local_id\": " << data->objects[i].target_local_id << ",\n";
            json << "      \"target_map\": \"" << data->objects[i].target_map << "\"";
        } else {
            json << ",\n";
            json << "      \"movement_type\": \"" << data->objects[i].movement_type << "\",\n";
            json << "      \"movement_range_x\": " << data->objects[i].movement_range_x << ",\n";
            json << "      \"movement_range_y\": " << data->objects[i].movement_range_y << ",\n";
            json << "      \"trainer_type\": \"" << data->objects[i].trainer_type << "\",\n";
            json << "      \"trainer_sight_or_berry_tree_id\": \"" 
                 << data->objects[i].trainer_sight_or_berry_tree_id << "\",\n";
            json << "      \"script\": \"" << data->objects[i].script << "\",\n";
            json << "      \"flag\": \"" << data->objects[i].flag << "\"";
        }
        
        if (strlen(data->objects[i].local_id) > 0) {
            json << ",\n";
            json << "      \"local_id\": \"" << data->objects[i].local_id << "\"";
        }
        
        json << "\n    }";
    }
    if (data->object_count > 0) json << "\n  ";
    json << "],\n";
    
    // Add warp_events
    json << "  \"warp_events\": [";
    for (uint16_t i = 0; i < data->warp_count; i++) {
        if (i > 0) json << ",";
        json << "\n    {\n";
        json << "      \"x\": " << data->warps[i].x << ",\n";
        json << "      \"y\": " << data->warps[i].y << ",\n";
        json << "      \"elevation\": " << data->warps[i].elevation << ",\n";
        json << "      \"dest_warp_id\": " << data->warps[i].dest_warp_id << ",\n";
        json << "      \"dest_map\": \"" << data->warps[i].dest_map << "\"\n";
        json << "    }";
    }
    if (data->warp_count > 0) json << "\n  ";
    json << "],\n";
    
    // Add coord_events
    json << "  \"coord_events\": [";
    for (uint16_t i = 0; i < data->coord_event_count; i++) {
        if (i > 0) json << ",";
        json << "\n    {\n";
        json << "      \"type\": \"" << data->coord_events[i].type << "\",\n";
        json << "      \"x\": " << data->coord_events[i].x << ",\n";
        json << "      \"y\": " << data->coord_events[i].y << ",\n";
        json << "      \"elevation\": " << data->coord_events[i].elevation;
        
        string type = data->coord_events[i].type;
        if (type == "trigger") {
            json << ",\n";
            json << "      \"var\": \"" << data->coord_events[i].var << "\",\n";
            json << "      \"var_value\": \"" << data->coord_events[i].var_value << "\",\n";
            json << "      \"script\": \"" << data->coord_events[i].script << "\"";
        } else if (type == "weather") {
            json << ",\n";
            json << "      \"weather\": \"" << data->coord_events[i].weather << "\"";
        }
        
        json << "\n    }";
    }
    if (data->coord_event_count > 0) json << "\n  ";
    json << "],\n";
    
    // Add bg_events
    json << "  \"bg_events\": [";
    for (uint16_t i = 0; i < data->bg_event_count; i++) {
        if (i > 0) json << ",";
        json << "\n    {\n";
        json << "      \"type\": \"" << data->bg_events[i].type << "\",\n";
        json << "      \"x\": " << data->bg_events[i].x << ",\n";
        json << "      \"y\": " << data->bg_events[i].y << ",\n";
        json << "      \"elevation\": " << data->bg_events[i].elevation;
        
        string type = data->bg_events[i].type;
        if (type == "sign") {
            json << ",\n";
            json << "      \"player_facing_dir\": \"" << data->bg_events[i].player_facing_dir << "\",\n";
            json << "      \"script\": \"" << data->bg_events[i].script << "\"";
        } else if (type == "hidden_item") {
            json << ",\n";
            json << "      \"item\": \"" << data->bg_events[i].item << "\",\n";
            json << "      \"flag\": \"" << data->bg_events[i].flag << "\",\n";
            json << "      \"quantity\": " << data->bg_events[i].quantity << ",\n";
            json << "      \"underfoot\": \"" << data->bg_events[i].underfoot << "\"";
        } else if (type == "secret_base") {
            json << ",\n";
            json << "      \"secret_base_id\": \"" << data->bg_events[i].secret_base_id << "\"";
        }
        
        json << "\n    }";
    }
    if (data->bg_event_count > 0) json << "\n  ";
    json << "]\n";
    
    json << "}\n";
    
    // Write to file
    return write_file(output_path, json.str(), error_buffer, error_size);
}

bool ValidateMapRoundTrip(const MapData* original, const MapData* converted) {
    if (!original || !converted) {
        return false;
    }
    
    // Compare basic fields
    if (strcmp(original->name, converted->name) != 0) return false;
    if (strcmp(original->id, converted->id) != 0) return false;
    if (strcmp(original->layout, converted->layout) != 0) return false;
    if (strcmp(original->music, converted->music) != 0) return false;
    if (strcmp(original->weather, converted->weather) != 0) return false;
    if (strcmp(original->map_type, converted->map_type) != 0) return false;
    
    // Compare boolean fields
    if (original->requires_flash != converted->requires_flash) return false;
    if (original->allow_cycling != converted->allow_cycling) return false;
    if (original->allow_escaping != converted->allow_escaping) return false;
    if (original->allow_running != converted->allow_running) return false;
    if (original->show_map_name != converted->show_map_name) return false;
    
    // Compare numeric fields
    if (original->floor_number != converted->floor_number) return false;
    
    // Compare array counts
    if (original->connection_count != converted->connection_count) return false;
    if (original->warp_count != converted->warp_count) return false;
    if (original->object_count != converted->object_count) return false;
    if (original->coord_event_count != converted->coord_event_count) return false;
    if (original->bg_event_count != converted->bg_event_count) return false;
    
    return true;
}
