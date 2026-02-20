#pragma once
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include "enemy_combatant.h"
#include "enemy_data.h"

namespace godot {

/// EnemyCombatant C++ 코어 래퍼
/// GDScript EnemyData Resource를 받아 내부적으로 C++ EnemyData로 변환·소유한다.
class GDEnemyCombatant : public RefCounted {
    GDCLASS(GDEnemyCombatant, RefCounted)
public:
    GDEnemyCombatant();

    // --- GDScript 메서드 ---
    void setup_from_data(Object* enemy_res);

    bool       is_alive() const;
    Dictionary get_current_move() const;

    // --- 프로퍼티 getter ---
    String get_display_name() const;
    int    get_current_hp()   const;
    int    get_max_hp()       const;
    int    get_block()        const;

    // --- C++ 내부 접근 (GDCombatManager에서 사용) ---
    EnemyCombatant* get_core() { return &_core; }

protected:
    static void _bind_methods();

private:
    EnemyData      _enemy_data;  // 변환·소유한 C++ 데이터
    EnemyCombatant _core;

    void _connect_callbacks();
    static Dictionary _move_to_dict(const EnemyMove* move);
};

} // namespace godot
