extends Node
class_name CombatManager

## 전투 턴 FSM 제어
## PLAYER_TURN → (EndTurn) → ENEMY_TURN → (완료) → PLAYER_TURN
## 승리: 모든 적 HP=0 / 패배: 플레이어 HP=0

signal combat_ended(result: String)   # "victory" | "defeat"
signal turn_changed(turn: String)     # "player" | "enemy"
signal log_message(text: String)

enum TurnState { PLAYER_TURN, ENEMY_TURN, COMBAT_OVER }

var player: PlayerCombatant = null
var enemies: Array[EnemyCombatant] = []
var turn_state: TurnState = TurnState.PLAYER_TURN

## CardHand 노드 참조 (씬 연결용)
var card_hand: CardHand = null


func start_combat(p: PlayerCombatant, es: Array[EnemyCombatant], hand: CardHand) -> void:
	player = p
	enemies = es
	card_hand = hand

	# 적 HP=0 감지
	for enemy in enemies:
		enemy.died.connect(_on_enemy_died.bind(enemy))

	player.died.connect(_on_player_died)

	_begin_player_turn()


## 카드 플레이 (CardHand.card_played 시그널에서 호출)
func on_card_played(card_data: CardData, target_enemy: EnemyCombatant) -> void:
	if turn_state != TurnState.PLAYER_TURN:
		return
	if not player.can_play_card(card_data):
		return

	# 실제 타겟 결정
	var targets: Array = []
	match card_data.target_type:
		CardData.TargetType.SINGLE:
			if target_enemy != null:
				targets = [target_enemy]
			elif not enemies.is_empty():
				targets = [_first_alive_enemy()]
		CardData.TargetType.ALL_ENEMIES:
			targets = _alive_enemies()
		_:
			targets = []

	player.play_card(card_data)
	EffectHandler.apply_card(card_data, player, targets)
	log_message.emit("플레이어가 '%s' 카드를 사용했습니다." % card_data.card_name)

	_check_victory()


## EndTurn 버튼에서 호출
func end_player_turn() -> void:
	if turn_state != TurnState.PLAYER_TURN:
		return
	turn_state = TurnState.ENEMY_TURN
	player.end_turn()
	turn_changed.emit("enemy")
	_execute_enemy_turns()


func _begin_player_turn() -> void:
	turn_state = TurnState.PLAYER_TURN
	player.start_turn()
	turn_changed.emit("player")
	# 적 턴 시작 처리 (독 등)
	for enemy in _alive_enemies():
		enemy.start_turn()


func _execute_enemy_turns() -> void:
	for enemy in _alive_enemies():
		var move := enemy.execute_move()
		if move != null:
			EffectHandler.apply_enemy_move(move, enemy, player)
			log_message.emit("%s가 '%s'을(를) 사용했습니다." % [enemy.display_name, move.move_name])
		if not player.is_alive():
			return
	# 적 턴 종료
	for enemy in _alive_enemies():
		enemy.tick_statuses_end_of_turn()
	_begin_player_turn()


func _check_victory() -> void:
	if _alive_enemies().is_empty():
		turn_state = TurnState.COMBAT_OVER
		log_message.emit("전투 승리!")
		combat_ended.emit("victory")


func _on_enemy_died(_enemy: EnemyCombatant) -> void:
	_check_victory()


func _on_player_died() -> void:
	if turn_state == TurnState.COMBAT_OVER:
		return
	turn_state = TurnState.COMBAT_OVER
	log_message.emit("플레이어 사망...")
	combat_ended.emit("defeat")


func _alive_enemies() -> Array[EnemyCombatant]:
	var result: Array[EnemyCombatant] = []
	for e in enemies:
		if e.is_alive():
			result.append(e)
	return result


func _first_alive_enemy() -> EnemyCombatant:
	for e in enemies:
		if e.is_alive():
			return e
	return null
