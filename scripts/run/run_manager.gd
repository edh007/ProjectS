extends Node

## 런(로그라이크 진행) 관리 Autoload
## 방 순서: 전투→보상→전투→보상→보스→승리

signal room_changed(room_index: int, room_type: String)

## 방 순서 정의
const ROOM_SEQUENCE: Array[Dictionary] = [
	{"type": "combat",  "enemy_id": "cultist"},
	{"type": "reward"},
	{"type": "combat",  "enemy_id": "jaw_worm"},
	{"type": "reward"},
	{"type": "combat",  "enemy_id": "slime_boss"},
	{"type": "victory"},
]

## 씬 경로
const SCENE_BATTLE  := "res://scenes/game/battle.tscn"
const SCENE_REWARD  := "res://scenes/game/reward.tscn"
const SCENE_GAMEOVER := "res://scenes/game/game_over.tscn"
const SCENE_VICTORY := "res://scenes/game/victory.tscn"
const SCENE_MENU    := "res://scenes/ui/main_menu.tscn"

var current_room: int = 0
var current_enemy_id: String = ""
## 보상 선택용: 이번 보상에서 제공할 카드 3장
var reward_choices: Array[CardData] = []


func start_run() -> void:
	current_room = 0
	GameState.reset()
	GameState.player_hp = GameState.player_max_hp
	_init_player_deck()
	_go_to_room(0)


func complete_combat(result: String) -> void:
	if result == "defeat":
		GameState.change_state(GameState.State.GAME_OVER)
		get_tree().change_scene_to_file(SCENE_GAMEOVER)
		return
	# 승리 → 다음 방으로
	_go_to_room(current_room + 1)


func complete_reward(chosen_card: CardData) -> void:
	if chosen_card != null:
		GameState.player_deck.append(chosen_card)
	_go_to_room(current_room + 1)


func get_current_room_data() -> Dictionary:
	if current_room < ROOM_SEQUENCE.size():
		return ROOM_SEQUENCE[current_room]
	return {"type": "victory"}


func _go_to_room(index: int) -> void:
	current_room = index
	if index >= ROOM_SEQUENCE.size():
		GameState.change_state(GameState.State.VICTORY)
		get_tree().change_scene_to_file(SCENE_VICTORY)
		return

	var room := ROOM_SEQUENCE[index]
	match room["type"]:
		"combat":
			current_enemy_id = room.get("enemy_id", "cultist")
			var is_boss := (current_enemy_id == "slime_boss")
			GameState.change_state(GameState.State.BOSS if is_boss else GameState.State.COMBAT)
			get_tree().change_scene_to_file(SCENE_BATTLE)
		"reward":
			_prepare_reward()
			GameState.change_state(GameState.State.REWARD)
			get_tree().change_scene_to_file(SCENE_REWARD)
		"victory":
			GameState.change_state(GameState.State.VICTORY)
			get_tree().change_scene_to_file(SCENE_VICTORY)

	room_changed.emit(current_room, room["type"])


func _prepare_reward() -> void:
	reward_choices.clear()
	var all_ids := ["strike", "defend", "bash", "iron_wave", "thunderclap",
					"twin_strike", "clothesline", "shrug_it_off",
					"pommel_strike", "inflame", "cleave", "wild_strike"]
	all_ids.shuffle()
	for i in mini(3, all_ids.size()):
		var card := CardDatabase.get_card(all_ids[i])
		if card != null:
			reward_choices.append(card)


func _init_player_deck() -> void:
	GameState.player_deck.clear()
	# 초기 덱: Strike × 5, Defend × 4, Bash × 1
	var counts := {"strike": 5, "defend": 4, "bash": 1}
	for card_id in counts:
		var card := CardDatabase.get_card(card_id)
		if card != null:
			for _i in counts[card_id]:
				GameState.player_deck.append(card)
