#include "gd_enemy_combatant.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/object.hpp>

namespace godot {

GDEnemyCombatant::GDEnemyCombatant() {
    _connect_callbacks();
}

void GDEnemyCombatant::_connect_callbacks() {
    _core.on_hp_changed = [this](int cur, int mx) {
        emit_signal("hp_changed", cur, mx);
    };
    _core.on_block_changed = [this](int cur) {
        emit_signal("block_changed", cur);
    };
    _core.on_died = [this]() {
        emit_signal("died");
    };
    _core.on_intent_changed = [this](const EnemyMove* move) {
        emit_signal("intent_changed", _move_to_dict(move));
    };
}

void GDEnemyCombatant::setup_from_data(Object* enemy_res) {
    if (!enemy_res) return;

    // GDScript EnemyData Resource → C++ EnemyData 변환
    _enemy_data.id         = String(enemy_res->get("id")).utf8().get_data();
    _enemy_data.enemy_name = String(enemy_res->get("enemy_name")).utf8().get_data();
    _enemy_data.max_hp     = (int)enemy_res->get("max_hp");
    _enemy_data.first_move_index = (int)enemy_res->get("first_move_index");

    Array move_pattern = enemy_res->get("move_pattern");
    _enemy_data.move_pattern.clear();
    for (int i = 0; i < move_pattern.size(); ++i) {
        Object* move_res = move_pattern[i].operator Object*();
        if (!move_res) continue;

        EnemyMove em;
        em.move_name  = String(move_res->get("move_name")).utf8().get_data();
        em.move_type  = static_cast<EnemyMove::MoveType>((int)move_res->get("move_type"));
        em.intent_icon = String(move_res->get("intent_icon")).utf8().get_data();

        Dictionary p = move_res->get("move_params");
        em.params.damage        = p.has("damage")        ? (int)p["damage"]        : 0;
        em.params.block         = p.has("block")         ? (int)p["block"]         : 0;
        em.params.hits          = p.has("hits")          ? (int)p["hits"]          : 1;
        em.params.buff_strength = p.has("strength")      ? (int)p["strength"]      : 0;
        em.params.apply_poison  = p.has("apply_poison")  ? (int)p["apply_poison"]  : 0;

        _enemy_data.move_pattern.push_back(em);
    }

    _core.setup_from_data(&_enemy_data);
}

bool GDEnemyCombatant::is_alive() const {
    return _core.is_alive();
}

Dictionary GDEnemyCombatant::get_current_move() const {
    return _move_to_dict(_core.get_current_move());
}

String GDEnemyCombatant::get_display_name() const { return String(_core.display_name.c_str()); }
int    GDEnemyCombatant::get_current_hp()   const { return _core.current_hp; }
int    GDEnemyCombatant::get_max_hp()       const { return _core.max_hp; }
int    GDEnemyCombatant::get_block()        const { return _core.block; }

Dictionary GDEnemyCombatant::_move_to_dict(const EnemyMove* move) {
    Dictionary d;
    if (!move) {
        d["intent_icon"]    = String("unknown");
        d["move_name"]      = String("");
        d["damage"]         = 0;
        d["hits"]           = 1;
        d["block"]          = 0;
        d["buff_strength"]  = 0;
        d["apply_poison"]   = 0;
        return d;
    }
    d["intent_icon"]   = String(move->intent_icon.c_str());
    d["move_name"]     = String(move->move_name.c_str());
    d["damage"]        = move->params.damage;
    d["hits"]          = move->params.hits;
    d["block"]         = move->params.block;
    d["buff_strength"] = move->params.buff_strength;
    d["apply_poison"]  = move->params.apply_poison;
    return d;
}

void GDEnemyCombatant::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup_from_data", "enemy_res"), &GDEnemyCombatant::setup_from_data);
    ClassDB::bind_method(D_METHOD("is_alive"),                     &GDEnemyCombatant::is_alive);
    ClassDB::bind_method(D_METHOD("get_current_move"),             &GDEnemyCombatant::get_current_move);

    ClassDB::bind_method(D_METHOD("get_display_name"), &GDEnemyCombatant::get_display_name);
    ClassDB::bind_method(D_METHOD("get_current_hp"),   &GDEnemyCombatant::get_current_hp);
    ClassDB::bind_method(D_METHOD("get_max_hp"),       &GDEnemyCombatant::get_max_hp);
    ClassDB::bind_method(D_METHOD("get_block"),        &GDEnemyCombatant::get_block);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "display_name"), "", "get_display_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "current_hp"),   "", "get_current_hp");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "max_hp"),       "", "get_max_hp");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "block"),        "", "get_block");

    ADD_SIGNAL(MethodInfo("hp_changed",
        PropertyInfo(Variant::INT, "current"),
        PropertyInfo(Variant::INT, "maximum")));
    ADD_SIGNAL(MethodInfo("block_changed",
        PropertyInfo(Variant::INT, "current")));
    ADD_SIGNAL(MethodInfo("intent_changed",
        PropertyInfo(Variant::DICTIONARY, "move")));
    ADD_SIGNAL(MethodInfo("died"));
}

} // namespace godot
