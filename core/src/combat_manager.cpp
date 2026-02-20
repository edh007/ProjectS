#include "combat_manager.h"
#include "effect_handler.h"
#include <algorithm>

void CombatManager::start_combat(PlayerCombatant* player,
                                  std::vector<EnemyCombatant*> enemies)
{
    _player   = player;
    _enemies  = std::move(enemies);

    for (EnemyCombatant* e : _enemies) {
        e->on_died = [this, e]() { _check_victory(); };
    }
    _player->on_died = [this]() {
        if (_turn_state == TurnState::COMBAT_OVER) return;
        _turn_state = TurnState::COMBAT_OVER;
        if (on_log_message) on_log_message("플레이어 사망...");
        if (on_combat_ended) on_combat_ended("defeat");
    };

    _begin_player_turn();
}

void CombatManager::on_card_played(const CardData* card, EnemyCombatant* target) {
    if (_turn_state != TurnState::PLAYER_TURN) return;
    if (!_player->can_play_card(card)) return;

    std::vector<EnemyCombatant*> targets;
    if (card->target_type == CardData::TargetType::SINGLE) {
        EnemyCombatant* t = target ? target : _first_alive_enemy();
        if (t) targets.push_back(t);
    } else if (card->target_type == CardData::TargetType::ALL_ENEMIES) {
        targets = _alive_enemies();
    }

    _player->play_card(card);
    EffectHandler::apply_card(card, _player, targets);

    if (on_log_message)
        on_log_message("플레이어가 '" + card->card_name + "' 카드를 사용했습니다.");

    _check_victory();
}

void CombatManager::end_player_turn() {
    if (_turn_state != TurnState::PLAYER_TURN) return;
    _turn_state = TurnState::ENEMY_TURN;
    _player->end_turn();
    if (on_turn_changed) on_turn_changed("enemy");
    _execute_enemy_turns();
}

void CombatManager::_begin_player_turn() {
    _turn_state = TurnState::PLAYER_TURN;
    _player->start_turn();
    if (on_turn_changed) on_turn_changed("player");
    for (EnemyCombatant* e : _alive_enemies()) {
        e->start_turn();
    }
}

void CombatManager::_execute_enemy_turns() {
    for (EnemyCombatant* enemy : _alive_enemies()) {
        const EnemyMove* move = enemy->execute_move();
        if (move) {
            EffectHandler::apply_enemy_move(move, enemy, _player);
            if (on_log_message)
                on_log_message(enemy->display_name + "가 '" + move->move_name + "'을(를) 사용했습니다.");
        }
        if (!_player->is_alive()) return;
    }
    for (EnemyCombatant* enemy : _alive_enemies()) {
        enemy->tick_statuses_end_of_turn();
    }
    _begin_player_turn();
}

void CombatManager::_check_victory() {
    if (_alive_enemies().empty()) {
        _turn_state = TurnState::COMBAT_OVER;
        if (on_log_message)  on_log_message("전투 승리!");
        if (on_combat_ended) on_combat_ended("victory");
    }
}

std::vector<EnemyCombatant*> CombatManager::_alive_enemies() const {
    std::vector<EnemyCombatant*> result;
    for (EnemyCombatant* e : _enemies) {
        if (e->is_alive()) result.push_back(e);
    }
    return result;
}

EnemyCombatant* CombatManager::_first_alive_enemy() const {
    for (EnemyCombatant* e : _enemies) {
        if (e->is_alive()) return e;
    }
    return nullptr;
}
