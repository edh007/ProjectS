extends Node2D

## 전투 씬 진입점
## CombatManager, PlayerCombatant, EnemyCombatant, UI를 연결한다.

@onready var _enemy_area: HBoxContainer = $EnemyArea
@onready var _hand_area: CardHand = $HandArea
@onready var _hud: BattleHUD = $BattleHUD
@onready var _combat_manager: CombatManager = $CombatManager
@onready var _log_label: Label = $LogLabel

var _player: PlayerCombatant = PlayerCombatant.new()
var _enemies: Array[EnemyCombatant] = []

const EnemyViewScene := preload("res://scenes/enemies/enemy_view.tscn")


func _ready() -> void:
	_setup_player()
	_setup_enemies()
	_connect_signals()
	_combat_manager.start_combat(_player, _enemies, _hand_area)


func _setup_player() -> void:
	_player.setup("Player", GameState.player_max_hp)
	_player.current_hp = GameState.player_hp
	_player.setup_deck(GameState.player_deck.duplicate())
	_hud.bind_player(_player)


func _setup_enemies() -> void:
	var enemy_id := RunManager.current_enemy_id
	var enemy_data: EnemyData = load("res://resources/enemies/%s.tres" % enemy_id)
	if enemy_data == null:
		push_error("EnemyData not found: " + enemy_id)
		return

	var enemy := EnemyCombatant.new()
	enemy.setup_from_data(enemy_data)
	_enemies.append(enemy)

	var view: EnemyView = EnemyViewScene.instantiate()
	_enemy_area.add_child(view)
	view.bind(enemy)


func _connect_signals() -> void:
	_hand_area.card_played.connect(_on_card_played)
	_hud.end_turn_pressed.connect(_combat_manager.end_player_turn)
	_combat_manager.combat_ended.connect(_on_combat_ended)
	_combat_manager.turn_changed.connect(_hud.set_turn_label)
	_combat_manager.turn_changed.connect(_on_turn_changed)
	_combat_manager.log_message.connect(_on_log_message)


func _on_card_played(card_data: CardData) -> void:
	# 단일 타겟: 적이 한 명이면 자동 선택
	var target: EnemyCombatant = null
	for e in _enemies:
		if e.is_alive():
			target = e
			break
	_combat_manager.on_card_played(card_data, target)
	_refresh_hand_playability()


func _on_turn_changed(turn: String) -> void:
	if turn == "player":
		# 손패 갱신 (새 카드 드로우 후)
		_rebuild_hand()
		_refresh_hand_playability()


func _rebuild_hand() -> void:
	_hand_area.clear_hand()
	for card_data in _player.get_hand():
		_hand_area.add_card(card_data)


func _refresh_hand_playability() -> void:
	_hand_area.refresh_playability(func(cd: CardData): return _player.can_play_card(cd))


func _on_combat_ended(result: String) -> void:
	# HP 유지
	GameState.player_hp = _player.current_hp
	await get_tree().create_timer(1.5).timeout
	RunManager.complete_combat(result)


func _on_log_message(text: String) -> void:
	_log_label.text = text
