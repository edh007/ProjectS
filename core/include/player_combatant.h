#pragma once
#include <functional>
#include <vector>
#include "combatant.h"
#include "deck.h"
#include "card_data.h"

class PlayerCombatant : public Combatant {
public:
    static constexpr int MAX_HAND_SIZE  = 10;
    static constexpr int DRAW_PER_TURN  = 5;
    static constexpr int MAX_ENERGY     = 3;

    std::function<void(int cur, int max)>              on_energy_changed;
    std::function<void(const std::vector<const CardData*>&)> on_hand_changed;
    std::function<void(int count)>                     on_draw_pile_changed;
    std::function<void(int count)>                     on_discard_pile_changed;

    int max_energy     = MAX_ENERGY;
    int current_energy = 0;

    void setup_deck(const std::vector<const CardData*>& cards);
    void start_turn();
    void end_turn();
    void draw_cards(int count);

    bool play_card(const CardData* card);
    bool can_play_card(const CardData* card) const;

    void add_card_to_draw(const CardData* card);

    const std::vector<const CardData*>& get_hand() const;
    int draw_pile_count() const;
    int discard_pile_count() const;

private:
    Deck _draw_pile;
    Deck _discard_pile;
    std::vector<const CardData*> _hand;

    void _refill_draw_if_empty();
};
