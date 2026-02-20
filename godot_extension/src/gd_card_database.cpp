#include "gd_card_database.h"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "effect_handler.h"

namespace godot {

GDCardDatabase* GDCardDatabase::_singleton = nullptr;

GDCardDatabase::GDCardDatabase() {
    _singleton = this;
}

GDCardDatabase::~GDCardDatabase() {
    if (_singleton == this) _singleton = nullptr;
}

GDCardDatabase* GDCardDatabase::get_singleton() {
    return _singleton;
}

void GDCardDatabase::_ready() {
    _load_cards();
    EffectHandler::set_database(this);
}

const CardData* GDCardDatabase::get_card(const std::string& id) const {
    auto it = _cpp_cards.find(id);
    if (it == _cpp_cards.end()) return nullptr;
    return &it->second;
}

Ref<Resource> GDCardDatabase::get_gd_resource(const std::string& id) const {
    auto it = _gd_resources.find(id);
    if (it == _gd_resources.end()) return Ref<Resource>();
    return it->second;
}

void GDCardDatabase::_load_cards() {
    PackedStringArray files = DirAccess::get_files_at("res://resources/cards/");
    ResourceLoader* rl = ResourceLoader::get_singleton();

    for (int i = 0; i < files.size(); ++i) {
        String file = files[i];
        if (!file.ends_with(".tres")) continue;

        String path = "res://resources/cards/" + file;
        Ref<Resource> res = rl->load(path);
        if (res.is_null()) continue;

        CardData cd = _convert_card_resource(res);
        _cpp_cards[cd.id]    = cd;
        _gd_resources[cd.id] = res;
    }
    UtilityFunctions::print("GDCardDatabase: loaded ", (int)_cpp_cards.size(), " cards");
}

CardData GDCardDatabase::_convert_card_resource(const Ref<Resource>& res) const {
    CardData cd;
    cd.id          = String(res->get("id")).utf8().get_data();
    cd.card_name   = String(res->get("card_name")).utf8().get_data();
    cd.description = String(res->get("description")).utf8().get_data();
    cd.cost        = (int)res->get("cost");
    cd.card_type   = static_cast<CardData::CardType>((int)res->get("card_type"));
    cd.target_type = static_cast<CardData::TargetType>((int)res->get("target_type"));
    cd.playable    = (bool)res->get("playable");

    Dictionary params = res->get("effect_params");
    cd.effect.damage  = params.has("damage")   ? (int)params["damage"]   : 0;
    cd.effect.block   = params.has("block")    ? (int)params["block"]    : 0;
    cd.effect.hits    = params.has("hits")     ? (int)params["hits"]     : 1;
    cd.effect.draw    = params.has("draw")     ? (int)params["draw"]     : 0;
    cd.effect.strength = params.has("strength") ? (int)params["strength"] : 0;
    cd.effect.apply_vulnerable = params.has("apply_vulnerable")
                                 ? (int)params["apply_vulnerable"] : 0;
    cd.effect.apply_weak  = params.has("apply_weak")  ? (int)params["apply_weak"]  : 0;
    cd.effect.apply_poison = params.has("apply_poison") ? (int)params["apply_poison"] : 0;
    cd.effect.exhaust = params.has("exhaust")  ? (bool)params["exhaust"] : false;

    return cd;
}

void GDCardDatabase::_bind_methods() {
    // GDScript에서 직접 호출하는 메서드 없음 (내부 서비스 클래스)
}

} // namespace godot
