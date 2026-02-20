extends RefCounted
class_name Combatant

## HP, 블록, 상태이상을 관리하는 전투 참여자 베이스 클래스

signal hp_changed(current: int, maximum: int)
signal block_changed(current: int)
signal died()
signal status_changed(type: StatusEffect.Type, stacks: int)

var display_name: String = ""
var max_hp: int = 0
var current_hp: int = 0
var block: int = 0

## 상태이상 딕셔너리: StatusEffect.Type → StatusEffect
var _statuses: Dictionary = {}


func setup(p_name: String, p_max_hp: int) -> void:
	display_name = p_name
	max_hp = p_max_hp
	current_hp = p_max_hp
	block = 0
	_statuses.clear()


func is_alive() -> bool:
	return current_hp > 0


## 블록을 먼저 흡수한 뒤 HP를 감소시킨다.
func take_damage(amount: int) -> void:
	if amount <= 0:
		return
	var absorbed := mini(block, amount)
	block -= absorbed
	block_changed.emit(block)
	var remaining := amount - absorbed
	if remaining > 0:
		current_hp = maxi(0, current_hp - remaining)
		hp_changed.emit(current_hp, max_hp)
		if current_hp == 0:
			died.emit()


func heal(amount: int) -> void:
	current_hp = mini(max_hp, current_hp + amount)
	hp_changed.emit(current_hp, max_hp)


func add_block(amount: int) -> void:
	block += amount
	block_changed.emit(block)


## 매 턴 시작 시 블록 초기화
func reset_block() -> void:
	block = 0
	block_changed.emit(block)


func add_status(type: StatusEffect.Type, stacks: int) -> void:
	if _statuses.has(type):
		_statuses[type].stacks += stacks
	else:
		_statuses[type] = StatusEffect.new(type, stacks)
	status_changed.emit(type, _statuses[type].stacks)


func get_status_stacks(type: StatusEffect.Type) -> int:
	if _statuses.has(type):
		return _statuses[type].stacks
	return 0


func has_status(type: StatusEffect.Type) -> bool:
	return _statuses.has(type) and _statuses[type].stacks > 0


## 턴 종료 시 상태이상 tick 처리
func tick_statuses_end_of_turn() -> void:
	var to_remove: Array[StatusEffect.Type] = []
	for type in _statuses:
		var se: StatusEffect = _statuses[type]
		if se.tick_end_of_turn():
			to_remove.append(type)
		else:
			status_changed.emit(type, se.stacks)
	for type in to_remove:
		_statuses.erase(type)
		status_changed.emit(type, 0)


## 독 턴 시작 처리 (EffectHandler에서 호출)
func tick_poison_start_of_turn() -> void:
	if not has_status(StatusEffect.Type.POISON):
		return
	var stacks := get_status_stacks(StatusEffect.Type.POISON)
	take_damage(stacks)
	# Poison 스택은 tick_statuses_end_of_turn에서 감소
