extends Node

## 전역 게임 상태 관리 싱글턴
## 씬 전환 시에도 유지되어야 하는 데이터를 보관한다.

signal state_changed(new_state: State)

enum State {
	MAIN_MENU,
	COMBAT,
	REWARD,
	BOSS,
	GAME_OVER,
	VICTORY,
}

var current_state: State = State.MAIN_MENU
var player_score: int = 0

## 런 중 플레이어 덱 (RunManager가 관리)
var player_deck: Array[CardData] = []
var player_hp: int = 80
var player_max_hp: int = 80


func change_state(new_state: State) -> void:
	current_state = new_state
	state_changed.emit(new_state)


func reset() -> void:
	player_score = 0
	player_hp = 80
	player_max_hp = 80
	player_deck.clear()
	change_state(State.MAIN_MENU)
