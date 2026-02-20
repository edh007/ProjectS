#pragma once
#include <functional>
#include <string>
#include <vector>
#include "player_combatant.h"
#include "enemy_combatant.h"

class CombatManager {
public:
    enum class TurnState { PLAYER_TURN, ENEMY_TURN, COMBAT_OVER };

    std::function<void(const std::string& result)> on_combat_ended;  // "victory"|"defeat"
    std::function<void(const std::string& turn)>   on_turn_changed;  // "player"|"enemy"
    std::function<void(const std::string& msg)>    on_log_message;

    void start_combat(PlayerCombatant* player,
                      std::vector<EnemyCombatant*> enemies);

    void on_card_played(const CardData* card, EnemyCombatant* target);
    void end_player_turn();

    TurnState get_turn_state() const { return _turn_state; }

private:
    PlayerCombatant*             _player     = nullptr;
    std::vector<EnemyCombatant*> _enemies;
    TurnState                    _turn_state = TurnState::PLAYER_TURN;

    void _begin_player_turn();
    void _execute_enemy_turns();
    void _check_victory();

    std::vector<EnemyCombatant*> _alive_enemies() const;
    EnemyCombatant*              _first_alive_enemy() const;
};
