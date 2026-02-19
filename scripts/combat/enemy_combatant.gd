extends Combatant
class_name EnemyCombatant

## 적 전투 상태: 인텐트 패턴 AI

signal intent_changed(move: EnemyMove)

var data: EnemyData = null
var _move_index: int = 0
var _turn_count: int = 0

## 전투 시작 시 초기화
func setup_from_data(enemy_data: EnemyData) -> void:
	data = enemy_data
	setup(enemy_data.enemy_name, enemy_data.max_hp)
	# 첫 번째 인텐트 결정
	if enemy_data.first_move_index >= 0:
		_move_index = enemy_data.first_move_index
	else:
		_move_index = 0
	_turn_count = 0
	intent_changed.emit(get_current_move())


## 현재 인텐트(다음에 사용할 행동) 반환
func get_current_move() -> EnemyMove:
	if data == null or data.move_pattern.is_empty():
		return null
	return data.move_pattern[_move_index]


## 실제 행동 실행. EffectHandler에게 처리를 위임한다.
## 반환값: 실행한 EnemyMove
func execute_move() -> EnemyMove:
	var move := get_current_move()
	_advance_move()
	return move


## 다음 턴 인텐트로 전진
func _advance_move() -> void:
	if data == null or data.move_pattern.is_empty():
		return
	_turn_count += 1
	# Cultist 패턴: 첫 턴(Incantation)이 끝난 후엔 Dark Strike(index=1) 반복
	if data.first_move_index >= 0 and _turn_count >= 1:
		# 첫 번째 행동 이후에는 인덱스 1부터 순환
		_move_index = (_move_index % (data.move_pattern.size() - 1)) + 1
	else:
		_move_index = (_move_index + 1) % data.move_pattern.size()
	intent_changed.emit(get_current_move())


func start_turn() -> void:
	reset_block()
	tick_poison_start_of_turn()
