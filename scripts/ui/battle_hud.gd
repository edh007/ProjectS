extends CanvasLayer
class_name BattleHUD

## 에너지/HP/블록/덱 카운트 HUD

signal end_turn_pressed()

@onready var _energy_label: Label = $HUDContainer/EnergyLabel
@onready var _hp_label: Label = $HUDContainer/PlayerStatus/HPLabel
@onready var _block_label: Label = $HUDContainer/PlayerStatus/BlockLabel
@onready var _draw_button: Button = $HUDContainer/DrawPileButton
@onready var _discard_button: Button = $HUDContainer/DiscardPileButton
@onready var _end_turn_button: Button = $HUDContainer/EndTurnButton
@onready var _turn_label: Label = $HUDContainer/TurnLabel


func _ready() -> void:
	_end_turn_button.pressed.connect(func(): end_turn_pressed.emit())


func bind_player(player: Object) -> void:
	player.energy_changed.connect(_update_energy)
	player.hp_changed.connect(_update_hp)
	player.block_changed.connect(_update_block)
	player.draw_pile_changed.connect(_update_draw_pile)
	player.discard_pile_changed.connect(_update_discard_pile)
	# 초기값 적용
	_update_energy(player.current_energy, player.max_energy)
	_update_hp(player.current_hp, player.max_hp)
	_update_block(player.block)
	_update_draw_pile(player.draw_pile_count())
	_update_discard_pile(player.discard_pile_count())


func set_turn_label(turn: String) -> void:
	if turn == "player":
		_turn_label.text = "Your Turn"
		_end_turn_button.disabled = false
	else:
		_turn_label.text = "Enemy Turn"
		_end_turn_button.disabled = true


func _update_energy(current: int, maximum: int) -> void:
	_energy_label.text = "Energy: %d / %d" % [current, maximum]


func _update_hp(current: int, maximum: int) -> void:
	_hp_label.text = "HP: %d / %d" % [current, maximum]


func _update_block(current: int) -> void:
	_block_label.text = "Block: %d" % current


func _update_draw_pile(count: int) -> void:
	_draw_button.text = "Draw: %d" % count


func _update_discard_pile(count: int) -> void:
	_discard_button.text = "Disc: %d" % count
