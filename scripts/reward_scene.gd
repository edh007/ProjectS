extends Control

## 보상 씬: 카드 3장 중 1장을 선택하여 덱에 추가

@onready var _card_container: HBoxContainer = $VBoxContainer/CardContainer
@onready var _skip_button: Button = $VBoxContainer/SkipButton
@onready var _title_label: Label = $VBoxContainer/TitleLabel

const CardScene := preload("res://scenes/cards/card.tscn")


func _ready() -> void:
	_title_label.text = "Choose a Card"
	_skip_button.pressed.connect(_on_skip)
	_populate_choices()


func _populate_choices() -> void:
	for card_data in RunManager.reward_choices:
		var card_node: Card = CardScene.instantiate()
		_card_container.add_child(card_node)
		card_node.setup(card_data)
		card_node.set_playable(true)
		card_node.clicked.connect(_on_card_chosen.bind(card_data))


func _on_card_chosen(card_data: CardData) -> void:
	RunManager.complete_reward(card_data)


func _on_skip() -> void:
	RunManager.complete_reward(null)
