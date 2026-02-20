#include "effect_handler.h"
#include "combatant.h"
#include "player_combatant.h"
#include "enemy_combatant.h"
#include <algorithm>
#include <cmath>

namespace EffectHandler {

static const ICardDatabase* s_db = nullptr;

void set_database(const ICardDatabase* db) {
    s_db = db;
}

int calculate_damage(int base, const Combatant* attacker, const Combatant* target) {
    int dmg = base;
    // 근력 가산
    dmg += attacker->get_status_stacks(StatusEffect::Type::STRENGTH);
    // 약화 적용 (공격자)
    if (attacker->has_status(StatusEffect::Type::WEAK)) {
        dmg = static_cast<int>(std::floor(dmg * 0.75f));
    }
    // 취약 적용 (대상)
    if (target->has_status(StatusEffect::Type::VULNERABLE)) {
        dmg = static_cast<int>(std::ceil(dmg * 1.5f));
    }
    return std::max(0, dmg);
}

void apply_card(const CardData* card,
                PlayerCombatant* player,
                const std::vector<EnemyCombatant*>& targets)
{
    if (!card) return;
    const CardEffectParams& e = card->effect;

    // 피해
    if (e.damage > 0) {
        for (EnemyCombatant* target : targets) {
            int dmg = calculate_damage(e.damage, player, target);
            for (int i = 0; i < e.hits; ++i) {
                target->take_damage(dmg);
            }
        }
    }
    // 블록
    if (e.block > 0) {
        player->add_block(e.block);
    }
    // 드로우
    if (e.draw > 0) {
        player->draw_cards(e.draw);
    }
    // 근력
    if (e.strength > 0) {
        player->add_status(StatusEffect::Type::STRENGTH, e.strength);
    }
    // 상태이상 부여
    for (EnemyCombatant* target : targets) {
        if (e.apply_vulnerable > 0)
            target->add_status(StatusEffect::Type::VULNERABLE, e.apply_vulnerable);
        if (e.apply_weak > 0)
            target->add_status(StatusEffect::Type::WEAK, e.apply_weak);
        if (e.apply_poison > 0)
            target->add_status(StatusEffect::Type::POISON, e.apply_poison);
    }
    // Wild Strike — 드로우 더미에 Wound 추가
    if (s_db && card->id == "wild_strike") {
        const CardData* wound = s_db->get_card("wound");
        if (wound) player->add_card_to_draw(wound);
    }
}

void apply_enemy_move(const EnemyMove* move,
                      EnemyCombatant* enemy,
                      PlayerCombatant* player)
{
    if (!move) return;
    const EnemyMoveParams& p = move->params;

    if (p.damage > 0) {
        int dmg = calculate_damage(p.damage, enemy, player);
        for (int i = 0; i < p.hits; ++i) {
            player->take_damage(dmg);
        }
    }
    if (p.block > 0) {
        enemy->add_block(p.block);
    }
    if (p.buff_strength > 0) {
        enemy->add_status(StatusEffect::Type::STRENGTH, p.buff_strength);
    }
    if (p.apply_poison > 0) {
        player->add_status(StatusEffect::Type::POISON, p.apply_poison);
    }
}

} // namespace EffectHandler
