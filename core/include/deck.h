#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "card_data.h"

class Deck {
public:
    void add_card(const CardData* card);
    void shuffle();
    const CardData* draw();     // 없으면 nullptr 반환
    int  size() const;
    bool is_empty() const;
    void clear();

private:
    std::vector<const CardData*> _cards;
    std::mt19937 _rng{std::random_device{}()};
};
