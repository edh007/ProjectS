#include "gd_combat_manager.h"
#include "gd_card_database.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/object.hpp>

namespace godot {

void GDCombatManager::_connect_callbacks() {
    _core.on_combat_ended = [this](const std::string& result) {
        emit_signal("combat_ended", String::utf8(result.c_str()));
    };
    _core.on_turn_changed = [this](const std::string& turn) {
        emit_signal("turn_changed", String::utf8(turn.c_str()));
    };
    _core.on_log_message = [this](const std::string& msg) {
        emit_signal("log_message", String::utf8(msg.c_str()));
    };
}

void GDCombatManager::start_combat(Object* player_obj, const Array& enemies_arr) {
    _connect_callbacks();

    GDPlayerCombatant* gd_player = Object::cast_to<GDPlayerCombatant>(player_obj);
    if (!gd_player) {
        UtilityFunctions::push_error("GDCombatManager: player is not GDPlayerCombatant");
        return;
    }

    _player_ref = Ref<GDPlayerCombatant>(gd_player);
    _enemy_refs = enemies_arr;

    std::vector<EnemyCombatant*> cpp_enemies;
    for (int i = 0; i < enemies_arr.size(); ++i) {
        GDEnemyCombatant* e = Object::cast_to<GDEnemyCombatant>(
            enemies_arr[i].operator Object*());
        if (e) cpp_enemies.push_back(e->get_core());
    }

    _core.start_combat(gd_player->get_core(), cpp_enemies);
}

void GDCombatManager::on_card_played(Ref<Resource> card, Object* target_obj) {
    if (card.is_null()) return;

    GDCardDatabase* db = GDCardDatabase::get_singleton();
    if (!db) return;

    std::string id = String(card->get("id")).utf8().get_data();
    const CardData* cd = db->get_card(id);
    if (!cd) {
        UtilityFunctions::push_error("GDCombatManager: card not found: ", card->get("id"));
        return;
    }

    GDEnemyCombatant* gd_target = Object::cast_to<GDEnemyCombatant>(target_obj);
    EnemyCombatant* cpp_target = gd_target ? gd_target->get_core() : nullptr;

    _core.on_card_played(cd, cpp_target);
}

void GDCombatManager::end_player_turn() {
    _core.end_player_turn();
}

void GDCombatManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start_combat", "player", "enemies"),
                         &GDCombatManager::start_combat);
    ClassDB::bind_method(D_METHOD("on_card_played", "card", "target"),
                         &GDCombatManager::on_card_played);
    ClassDB::bind_method(D_METHOD("end_player_turn"),
                         &GDCombatManager::end_player_turn);

    ADD_SIGNAL(MethodInfo("combat_ended",
        PropertyInfo(Variant::STRING, "result")));
    ADD_SIGNAL(MethodInfo("turn_changed",
        PropertyInfo(Variant::STRING, "turn")));
    ADD_SIGNAL(MethodInfo("log_message",
        PropertyInfo(Variant::STRING, "msg")));
}

} // namespace godot
