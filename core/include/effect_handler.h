#pragma once
#include <vector>
#include "card_data.h"
#include "enemy_move.h"
#include "i_card_database.h"

class Combatant;
class PlayerCombatant;
class EnemyCombatant;

namespace EffectHandler {

    /// 카드 데이터베이스 주입 (GDExtension 초기화 시 한 번 호출)
    void set_database(const ICardDatabase* db);

    /// 피해 계산 (근력/약화/취약 적용)
    int calculate_damage(int base, const Combatant* attacker, const Combatant* target);

    /// 카드 효과 적용
    void apply_card(const CardData* card,
                    PlayerCombatant* player,
                    const std::vector<EnemyCombatant*>& targets);

    /// 적 무브 효과 적용
    void apply_enemy_move(const EnemyMove* move,
                          EnemyCombatant* enemy,
                          PlayerCombatant* player);

} // namespace EffectHandler
