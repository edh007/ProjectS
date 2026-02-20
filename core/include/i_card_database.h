#pragma once
#include <string>
#include "card_data.h"

/// EffectHandler가 CardDatabase에 직접 의존하지 않도록 하는 인터페이스.
/// GDExtension과 Unreal 모두 이 인터페이스를 구현하여 주입한다.
class ICardDatabase {
public:
    virtual ~ICardDatabase() = default;
    virtual const CardData* get_card(const std::string& id) const = 0;
};
