extends Control
class_name Card

## 카드 한 장의 시각적 표현 및 인터랙션 처리

signal clicked(card: Card)

@onready var _artwork: TextureRect = $Artwork
@onready var _name_label: Label = $NameLabel
@onready var _cost_label: Label = $CostLabel
@onready var _attack_label: Label = $AttackLabel
@onready var _defense_label: Label = $DefenseLabel
@onready var _desc_label: Label = $DescLabel

var data: CardData = null
var _is_hovered: bool = false


func setup(card_data: CardData) -> void:
	data = card_data
	_name_label.text = card_data.card_name
	_cost_label.text = str(card_data.cost)
	_attack_label.text = str(card_data.attack)
	_defense_label.text = str(card_data.defense)
	_desc_label.text = card_data.description
	if card_data.artwork != null:
		_artwork.texture = card_data.artwork


func _on_mouse_entered() -> void:
	_is_hovered = true
	# 호버 시 카드를 살짝 위로 올린다.
	position.y -= 20


func _on_mouse_exited() -> void:
	_is_hovered = false
	position.y += 20


func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_LEFT:
			clicked.emit(self)
