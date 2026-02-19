extends CanvasLayer

## 인게임 HUD (점수, 턴 정보, 버튼 등)

@onready var _turn_label: Label = $MarginContainer/HBoxContainer/TurnLabel
@onready var _score_label: Label = $MarginContainer/HBoxContainer/ScoreLabel
@onready var _end_turn_button: Button = $MarginContainer/HBoxContainer/EndTurnButton

signal end_turn_requested


func _ready() -> void:
	_end_turn_button.pressed.connect(func(): end_turn_requested.emit())


func update_turn(turn_number: int) -> void:
	_turn_label.text = "Turn: %d" % turn_number


func update_score(score: int) -> void:
	_score_label.text = "Score: %d" % score
