#pragma once
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include "player_combatant.h"

namespace godot {

/// PlayerCombatant C++ 코어 래퍼
/// GDScript의 PlayerCombatant와 동일한 시그널/프로퍼티/메서드를 노출한다.
class GDPlayerCombatant : public RefCounted {
    GDCLASS(GDPlayerCombatant, RefCounted)
public:
    GDPlayerCombatant();

    // --- GDScript 메서드 ---
    void setup(const String& name, int max_hp);
    void setup_deck(const Array& cards);    // Array[CardData Resource]

    void start_turn();
    void end_turn();
    void draw_cards(int count);

    bool play_card(Ref<Resource> card);
    bool can_play_card(Ref<Resource> card) const;

    Array get_hand() const;             // Array[CardData Resource]
    int   draw_pile_count() const;
    int   discard_pile_count() const;

    // --- 프로퍼티 getter / setter ---
    String get_display_name()       const;
    int    get_current_hp()         const;
    void   set_current_hp(int hp);          // 저장된 HP 복원용
    int    get_max_hp()             const;
    int    get_block()              const;
    int    get_current_energy()     const;
    int    get_max_energy()         const;

    // --- C++ 내부 접근 (GDCombatManager에서 사용) ---
    PlayerCombatant* get_core() { return &_core; }

protected:
    static void _bind_methods();

private:
    PlayerCombatant _core;

    void _connect_callbacks();
    std::string _card_id_from_resource(const Ref<Resource>& res) const;
};

} // namespace godot
