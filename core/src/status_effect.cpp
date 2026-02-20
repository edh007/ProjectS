#include "status_effect.h"

bool StatusEffect::tick_end_of_turn() {
    switch (type) {
        case Type::VULNERABLE:
        case Type::WEAK:
            stacks -= 1;
            return stacks <= 0;
        case Type::POISON:
            stacks -= 1;
            return stacks <= 0;
        default:
            // STRENGTH, DEXTERITY: 영구 유지
            return false;
    }
}

std::string StatusEffect::type_name(Type t) {
    switch (t) {
        case Type::STRENGTH:   return "근력";
        case Type::DEXTERITY:  return "민첩";
        case Type::VULNERABLE: return "취약";
        case Type::WEAK:       return "약화";
        case Type::POISON:     return "독";
        default:               return "?";
    }
}
