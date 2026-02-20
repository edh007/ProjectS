#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include "combat_manager.h"
#include "gd_player_combatant.h"
#include "gd_enemy_combatant.h"

namespace godot {

/// CombatManager C++ 코어 래퍼 Node
/// battle_scene.gd에서 $CombatManager로 참조한다.
class GDCombatManager : public Node {
    GDCLASS(GDCombatManager, Node)
public:
    void start_combat(Object* player, const Array& enemies);
    void on_card_played(Ref<Resource> card, Object* target);
    void end_player_turn();

protected:
    static void _bind_methods();

private:
    CombatManager           _core;
    Ref<GDPlayerCombatant>  _player_ref;   // 생명주기 유지
    Array                   _enemy_refs;   // 생명주기 유지

    void _connect_callbacks();
};

} // namespace godot
