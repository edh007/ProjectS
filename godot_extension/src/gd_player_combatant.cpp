#include "gd_player_combatant.h"
#include "gd_card_database.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

GDPlayerCombatant::GDPlayerCombatant() {
    _connect_callbacks();
}

void GDPlayerCombatant::_connect_callbacks() {
    _core.on_hp_changed = [this](int cur, int mx) {
        emit_signal("hp_changed", cur, mx);
    };
    _core.on_block_changed = [this](int cur) {
        emit_signal("block_changed", cur);
    };
    _core.on_died = [this]() {
        emit_signal("died");
    };
    _core.on_energy_changed = [this](int cur, int mx) {
        emit_signal("energy_changed", cur, mx);
    };
    _core.on_hand_changed = [this](const std::vector<const CardData*>& hand) {
        Array gd_hand;
        GDCardDatabase* db = GDCardDatabase::get_singleton();
        if (db) {
            for (const CardData* cd : hand) {
                Ref<Resource> res = db->get_gd_resource(cd->id);
                if (res.is_valid()) gd_hand.push_back(res);
            }
        }
        emit_signal("hand_changed", gd_hand);
    };
    _core.on_draw_pile_changed = [this](int count) {
        emit_signal("draw_pile_changed", count);
    };
    _core.on_discard_pile_changed = [this](int count) {
        emit_signal("discard_pile_changed", count);
    };
}

void GDPlayerCombatant::setup(const String& name, int max_hp) {
    _core.setup(name.utf8().get_data(), max_hp);
}

void GDPlayerCombatant::setup_deck(const Array& cards) {
    GDCardDatabase* db = GDCardDatabase::get_singleton();
    if (!db) {
        UtilityFunctions::push_error("GDCardDatabase not ready");
        return;
    }
    std::vector<const CardData*> cpp_cards;
    for (int i = 0; i < cards.size(); ++i) {
        Ref<Resource> res = cards[i];
        if (res.is_null()) continue;
        std::string id = String(res->get("id")).utf8().get_data();
        const CardData* cd = db->get_card(id);
        if (cd) cpp_cards.push_back(cd);
    }
    _core.setup_deck(cpp_cards);
}

void GDPlayerCombatant::start_turn()       { _core.start_turn(); }
void GDPlayerCombatant::end_turn()         { _core.end_turn(); }
void GDPlayerCombatant::draw_cards(int n)  { _core.draw_cards(n); }

bool GDPlayerCombatant::play_card(Ref<Resource> card) {
    GDCardDatabase* db = GDCardDatabase::get_singleton();
    if (!db || card.is_null()) return false;
    std::string id = _card_id_from_resource(card);
    const CardData* cd = db->get_card(id);
    if (!cd) return false;
    return _core.play_card(cd);
}

bool GDPlayerCombatant::can_play_card(Ref<Resource> card) const {
    GDCardDatabase* db = GDCardDatabase::get_singleton();
    if (!db || card.is_null()) return false;
    std::string id = _card_id_from_resource(card);
    const CardData* cd = db->get_card(id);
    if (!cd) return false;
    return _core.can_play_card(cd);
}

Array GDPlayerCombatant::get_hand() const {
    Array result;
    GDCardDatabase* db = GDCardDatabase::get_singleton();
    if (!db) return result;
    for (const CardData* cd : _core.get_hand()) {
        Ref<Resource> res = db->get_gd_resource(cd->id);
        if (res.is_valid()) result.push_back(res);
    }
    return result;
}

int GDPlayerCombatant::draw_pile_count()    const { return _core.draw_pile_count(); }
int GDPlayerCombatant::discard_pile_count() const { return _core.discard_pile_count(); }

String GDPlayerCombatant::get_display_name()   const { return String::utf8(_core.display_name.c_str()); }
int    GDPlayerCombatant::get_current_hp()     const { return _core.current_hp; }
void   GDPlayerCombatant::set_current_hp(int hp)     { _core.current_hp = hp; }
int    GDPlayerCombatant::get_max_hp()         const { return _core.max_hp; }
int    GDPlayerCombatant::get_block()          const { return _core.block; }
int    GDPlayerCombatant::get_current_energy() const { return _core.current_energy; }
int    GDPlayerCombatant::get_max_energy()     const { return _core.max_energy; }

std::string GDPlayerCombatant::_card_id_from_resource(const Ref<Resource>& res) const {
    return String(res->get("id")).utf8().get_data();
}

void GDPlayerCombatant::_bind_methods() {
    // 메서드
    ClassDB::bind_method(D_METHOD("setup", "name", "max_hp"),   &GDPlayerCombatant::setup);
    ClassDB::bind_method(D_METHOD("setup_deck", "cards"),       &GDPlayerCombatant::setup_deck);
    ClassDB::bind_method(D_METHOD("start_turn"),                &GDPlayerCombatant::start_turn);
    ClassDB::bind_method(D_METHOD("end_turn"),                  &GDPlayerCombatant::end_turn);
    ClassDB::bind_method(D_METHOD("draw_cards", "count"),       &GDPlayerCombatant::draw_cards);
    ClassDB::bind_method(D_METHOD("play_card", "card"),         &GDPlayerCombatant::play_card);
    ClassDB::bind_method(D_METHOD("can_play_card", "card"),     &GDPlayerCombatant::can_play_card);
    ClassDB::bind_method(D_METHOD("get_hand"),                  &GDPlayerCombatant::get_hand);
    ClassDB::bind_method(D_METHOD("draw_pile_count"),           &GDPlayerCombatant::draw_pile_count);
    ClassDB::bind_method(D_METHOD("discard_pile_count"),        &GDPlayerCombatant::discard_pile_count);

    // 프로퍼티 getter
    ClassDB::bind_method(D_METHOD("get_display_name"),   &GDPlayerCombatant::get_display_name);
    ClassDB::bind_method(D_METHOD("get_current_hp"),           &GDPlayerCombatant::get_current_hp);
    ClassDB::bind_method(D_METHOD("set_current_hp", "hp"),    &GDPlayerCombatant::set_current_hp);
    ClassDB::bind_method(D_METHOD("get_max_hp"),              &GDPlayerCombatant::get_max_hp);
    ClassDB::bind_method(D_METHOD("get_block"),          &GDPlayerCombatant::get_block);
    ClassDB::bind_method(D_METHOD("get_current_energy"), &GDPlayerCombatant::get_current_energy);
    ClassDB::bind_method(D_METHOD("get_max_energy"),     &GDPlayerCombatant::get_max_energy);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "display_name"), "",     "get_display_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "current_hp"),   "set_current_hp", "get_current_hp");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "max_hp"),       "",     "get_max_hp");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "block"),        "",     "get_block");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "current_energy"), "",   "get_current_energy");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "max_energy"),   "",     "get_max_energy");

    // 시그널
    ADD_SIGNAL(MethodInfo("hp_changed",
        PropertyInfo(Variant::INT, "current"),
        PropertyInfo(Variant::INT, "maximum")));
    ADD_SIGNAL(MethodInfo("block_changed",
        PropertyInfo(Variant::INT, "current")));
    ADD_SIGNAL(MethodInfo("energy_changed",
        PropertyInfo(Variant::INT, "current"),
        PropertyInfo(Variant::INT, "maximum")));
    ADD_SIGNAL(MethodInfo("hand_changed",
        PropertyInfo(Variant::ARRAY, "hand")));
    ADD_SIGNAL(MethodInfo("draw_pile_changed",
        PropertyInfo(Variant::INT, "count")));
    ADD_SIGNAL(MethodInfo("discard_pile_changed",
        PropertyInfo(Variant::INT, "count")));
    ADD_SIGNAL(MethodInfo("died"));
}

} // namespace godot
