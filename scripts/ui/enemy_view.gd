extends VBoxContainer
class_name EnemyView

## 적 한 마리의 시각적 표현 (HP바, 블록, 인텐트 표시)
## GDEnemyCombatant(C++ 래퍼)를 받아 바인딩한다.

@onready var _name_label: Label = $NameLabel
@onready var _hp_label: Label = $HPLabel
@onready var _block_label: Label = $BlockLabel
@onready var _intent_label: Label = $IntentLabel
@onready var _enemy_sprite: ColorRect = $EnemySprite

var _combatant = null  # GDEnemyCombatant


func bind(enemy: Object) -> void:
	_combatant = enemy
	_name_label.text = enemy.display_name
	_update_hp(enemy.current_hp, enemy.max_hp)
	_update_block(enemy.block)
	_update_intent(enemy.get_current_move())

	enemy.hp_changed.connect(_update_hp)
	enemy.block_changed.connect(_update_block)
	enemy.intent_changed.connect(_update_intent)
	enemy.died.connect(_on_died)


func _update_hp(current: int, maximum: int) -> void:
	_hp_label.text = "HP: %d / %d" % [current, maximum]


func _update_block(current: int) -> void:
	_block_label.text = "Block: %d" % current


func _update_intent(move: Dictionary) -> void:
	if move.is_empty():
		_intent_label.text = "Intent: ?"
		return
	match move.get("intent_icon", "unknown"):
		"attack":
			var dmg: int = move.get("damage", 0)
			var hits: int = move.get("hits", 1)
			if hits > 1:
				_intent_label.text = "Intent: ⚔ %d×%d" % [dmg, hits]
			else:
				_intent_label.text = "Intent: ⚔ %d" % dmg
		"defend":
			_intent_label.text = "Intent: 🛡"
		"buff":
			_intent_label.text = "Intent: ↑"
		_:
			_intent_label.text = "Intent: ?"


func _on_died() -> void:
	queue_free()
