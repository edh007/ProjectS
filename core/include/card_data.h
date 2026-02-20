#pragma once
#include <string>
#include "effect_params.h"

struct CardData {
    enum class CardType   { ATTACK, SKILL, POWER, STATUS };
    enum class TargetType { SINGLE, ALL_ENEMIES, SELF, NONE };

    std::string   id;
    std::string   card_name;
    std::string   description;
    int           cost        = 1;
    CardType      card_type   = CardType::ATTACK;
    TargetType    target_type = TargetType::SINGLE;
    std::string   artwork_path;   // Texture2D 대신 경로 문자열
    CardEffectParams effect;
    bool          playable    = true;
};
