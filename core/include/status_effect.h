#pragma once
#include <string>

class StatusEffect {
public:
    enum class Type {
        STRENGTH,
        DEXTERITY,
        VULNERABLE,
        WEAK,
        POISON,
    };

    Type type;
    int  stacks = 0;

    StatusEffect(Type t, int s) : type(t), stacks(s) {}

    /// 턴 종료 시 스택 감소. true이면 이 상태이상을 제거해야 한다.
    bool tick_end_of_turn();

    static std::string type_name(Type t);
};
