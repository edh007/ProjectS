#include "deck.h"

void Deck::add_card(const CardData* card) {
    _cards.push_back(card);
}

void Deck::shuffle() {
    std::shuffle(_cards.begin(), _cards.end(), _rng);
}

const CardData* Deck::draw() {
    if (_cards.empty()) return nullptr;
    const CardData* card = _cards.back();
    _cards.pop_back();
    return card;
}

int Deck::size() const {
    return static_cast<int>(_cards.size());
}

bool Deck::is_empty() const {
    return _cards.empty();
}

void Deck::clear() {
    _cards.clear();
}
