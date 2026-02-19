extends Control
class_name Card

## 카드 한 장의 시각적 표현 및 인터랙션 처리

signal clicked(card: Card)

@onready var _artwork: TextureRect = $PanelContainer/VBoxContainer/Artwork
@onready var _name_label: Label = $PanelContainer/VBoxContainer/NameLabel
@onready var _cost_label: Label = $PanelContainer/VBoxContainer/CostLabel
@onready var _type_badge: Label = $PanelContainer/VBoxContainer/TypeBadge
@onready var _desc_label: Label = $PanelContainer/VBoxContainer/DescLabel
@onready var _panel: PanelContainer = $PanelContainer

var data: CardData = null
var _is_playable: bool = true

static var _type_names: Array[String] = ["공격", "스킬", "파워", "상태이상"]


func _ready() -> void:
	mouse_entered.connect(_on_mouse_entered)
	mouse_exited.connect(_on_mouse_exited)
	gui_input.connect(_on_gui_input)


func setup(card_data: CardData) -> void:
	data = card_data
	_name_label.text = card_data.card_name
	_cost_label.text = str(card_data.cost) if card_data.playable else "-"
	_desc_label.text = card_data.description
	_type_badge.text = _type_names[card_data.card_type]
	if card_data.artwork != null:
		_artwork.texture = card_data.artwork
	set_playable(card_data.playable)


func set_playable(can_play: bool) -> void:
	_is_playable = can_play
	modulate.a = 1.0 if can_play else 0.5


func _on_mouse_entered() -> void:
	if _is_playable:
		position.y -= 20


func _on_mouse_exited() -> void:
	if _is_playable:
		position.y += 20


func _on_gui_input(event: InputEvent) -> void:
	if not _is_playable:
		return
	if event is InputEventMouseButton and event.pressed:
		if event.button_index == MOUSE_BUTTON_LEFT:
			clicked.emit(self)
