#include "combatant.h"
#include <algorithm>

void Combatant::setup(const std::string& name, int p_max_hp) {
    display_name = name;
    max_hp       = p_max_hp;
    current_hp   = p_max_hp;
    block        = 0;
    _statuses.clear();
}

bool Combatant::is_alive() const {
    return current_hp > 0;
}

void Combatant::take_damage(int amount) {
    if (amount <= 0) return;
    int absorbed  = std::min(block, amount);
    block        -= absorbed;
    if (on_block_changed) on_block_changed(block);
    int remaining = amount - absorbed;
    if (remaining > 0) {
        current_hp = std::max(0, current_hp - remaining);
        if (on_hp_changed) on_hp_changed(current_hp, max_hp);
        if (current_hp == 0 && on_died) on_died();
    }
}

void Combatant::heal(int amount) {
    current_hp = std::min(max_hp, current_hp + amount);
    if (on_hp_changed) on_hp_changed(current_hp, max_hp);
}

void Combatant::add_block(int amount) {
    block += amount;
    if (on_block_changed) on_block_changed(block);
}

void Combatant::reset_block() {
    block = 0;
    if (on_block_changed) on_block_changed(block);
}

void Combatant::add_status(StatusEffect::Type type, int stacks) {
    int key = static_cast<int>(type);
    auto it = _statuses.find(key);
    if (it != _statuses.end()) {
        it->second.stacks += stacks;
    } else {
        _statuses.emplace(key, StatusEffect(type, stacks));
    }
    if (on_status_changed) on_status_changed(type, _statuses.at(key).stacks);
}

int Combatant::get_status_stacks(StatusEffect::Type type) const {
    auto it = _statuses.find(static_cast<int>(type));
    return (it != _statuses.end()) ? it->second.stacks : 0;
}

bool Combatant::has_status(StatusEffect::Type type) const {
    return get_status_stacks(type) > 0;
}

void Combatant::tick_statuses_end_of_turn() {
    std::vector<int> to_remove;
    for (auto& [key, se] : _statuses) {
        if (se.tick_end_of_turn()) {
            to_remove.push_back(key);
        } else {
            if (on_status_changed) on_status_changed(se.type, se.stacks);
        }
    }
    for (int key : to_remove) {
        StatusEffect::Type type = _statuses.at(key).type;
        _statuses.erase(key);
        if (on_status_changed) on_status_changed(type, 0);
    }
}

void Combatant::tick_poison_start_of_turn() {
    int stacks = get_status_stacks(StatusEffect::Type::POISON);
    if (stacks > 0) take_damage(stacks);
}
