extends Control

## 메인 메뉴 UI 제어

@onready var _play_button: Button = $VBoxContainer/PlayButton
@onready var _quit_button: Button = $VBoxContainer/QuitButton


func _ready() -> void:
	_play_button.pressed.connect(_on_play_pressed)
	_quit_button.pressed.connect(_on_quit_pressed)


func _on_play_pressed() -> void:
	RunManager.start_run()


func _on_quit_pressed() -> void:
	get_tree().quit()
