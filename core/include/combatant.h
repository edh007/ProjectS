#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "status_effect.h"

class Combatant {
public:
    // 콜백 (GDExtension/Unreal 래퍼가 시그널/델리게이트로 변환)
    std::function<void(int cur, int max)>                     on_hp_changed;
    std::function<void(int cur)>                              on_block_changed;
    std::function<void()>                                     on_died;
    std::function<void(StatusEffect::Type type, int stacks)>  on_status_changed;

    std::string display_name;
    int max_hp     = 0;
    int current_hp = 0;
    int block      = 0;

    virtual ~Combatant() = default;

    void setup(const std::string& name, int max_hp);
    bool is_alive() const;

    void take_damage(int amount);
    void heal(int amount);
    void add_block(int amount);
    void reset_block();

    void add_status(StatusEffect::Type type, int stacks);
    int  get_status_stacks(StatusEffect::Type type) const;
    bool has_status(StatusEffect::Type type) const;

    void tick_statuses_end_of_turn();
    void tick_poison_start_of_turn();

protected:
    std::unordered_map<int, StatusEffect> _statuses;  // int key = enum int
};
