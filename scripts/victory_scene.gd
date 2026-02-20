extends Control

## 승리 씬

@onready var _menu_button: Button = $VBoxContainer/MenuButton


func _ready() -> void:
	_menu_button.pressed.connect(_on_menu)


func _on_menu() -> void:
	get_tree().change_scene_to_file("res://scenes/ui/main_menu.tscn")
