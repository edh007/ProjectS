extends Control

## 게임 오버 씬

@onready var _retry_button: Button = $VBoxContainer/RetryButton
@onready var _quit_button: Button = $VBoxContainer/QuitButton


func _ready() -> void:
	_retry_button.pressed.connect(_on_retry)
	_quit_button.pressed.connect(_on_quit)


func _on_retry() -> void:
	RunManager.start_run()


func _on_quit() -> void:
	get_tree().change_scene_to_file("res://scenes/ui/main_menu.tscn")
