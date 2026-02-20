#include "player_combatant.h"
#include <algorithm>

void PlayerCombatant::setup_deck(const std::vector<const CardData*>& cards) {
    _draw_pile.clear();
    _discard_pile.clear();
    _hand.clear();
    for (const CardData* card : cards) {
        _draw_pile.add_card(card);
    }
    _draw_pile.shuffle();
    if (on_draw_pile_changed)    on_draw_pile_changed(_draw_pile.size());
    if (on_discard_pile_changed) on_discard_pile_changed(_discard_pile.size());
    if (on_hand_changed)         on_hand_changed(_hand);
}

void PlayerCombatant::start_turn() {
    reset_block();
    current_energy = max_energy;
    if (on_energy_changed) on_energy_changed(current_energy, max_energy);
    draw_cards(DRAW_PER_TURN);
}

void PlayerCombatant::end_turn() {
    for (const CardData* card : _hand) {
        _discard_pile.add_card(card);
    }
    _hand.clear();
    tick_statuses_end_of_turn();
    if (on_hand_changed)         on_hand_changed(_hand);
    if (on_discard_pile_changed) on_discard_pile_changed(_discard_pile.size());
}

void PlayerCombatant::draw_cards(int count) {
    for (int i = 0; i < count; ++i) {
        if (static_cast<int>(_hand.size()) >= MAX_HAND_SIZE) break;
        _refill_draw_if_empty();
        if (_draw_pile.is_empty()) break;
        _hand.push_back(_draw_pile.draw());
    }
    if (on_hand_changed)      on_hand_changed(_hand);
    if (on_draw_pile_changed) on_draw_pile_changed(_draw_pile.size());
}

void PlayerCombatant::_refill_draw_if_empty() {
    if (!_draw_pile.is_empty()) return;
    if (_discard_pile.is_empty()) return;
    while (!_discard_pile.is_empty()) {
        _draw_pile.add_card(_discard_pile.draw());
    }
    _draw_pile.shuffle();
    if (on_discard_pile_changed) on_discard_pile_changed(_discard_pile.size());
    if (on_draw_pile_changed)    on_draw_pile_changed(_draw_pile.size());
}

bool PlayerCombatant::play_card(const CardData* card) {
    if (!can_play_card(card)) return false;
    current_energy -= card->cost;
    if (on_energy_changed) on_energy_changed(current_energy, max_energy);
    auto it = std::find(_hand.begin(), _hand.end(), card);
    if (it != _hand.end()) _hand.erase(it);
    _discard_pile.add_card(card);
    if (on_hand_changed)         on_hand_changed(_hand);
    if (on_discard_pile_changed) on_discard_pile_changed(_discard_pile.size());
    return true;
}

bool PlayerCombatant::can_play_card(const CardData* card) const {
    if (!card || !card->playable) return false;
    if (current_energy < card->cost) return false;
    return std::find(_hand.begin(), _hand.end(), card) != _hand.end();
}

void PlayerCombatant::add_card_to_draw(const CardData* card) {
    _draw_pile.add_card(card);
    _draw_pile.shuffle();
    if (on_draw_pile_changed) on_draw_pile_changed(_draw_pile.size());
}

const std::vector<const CardData*>& PlayerCombatant::get_hand() const {
    return _hand;
}

int PlayerCombatant::draw_pile_count() const    { return _draw_pile.size(); }
int PlayerCombatant::discard_pile_count() const { return _discard_pile.size(); }
