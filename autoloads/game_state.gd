extends Node

## 전역 게임 상태 관리 싱글턴
## 씬 전환 시에도 유지되어야 하는 데이터를 보관한다.

signal state_changed(new_state: State)

enum State {
	MAIN_MENU,
	GAME,
	GAME_OVER,
}

var current_state: State = State.MAIN_MENU
var player_score: int = 0


func change_state(new_state: State) -> void:
	current_state = new_state
	state_changed.emit(new_state)


func reset() -> void:
	player_score = 0
	change_state(State.MAIN_MENU)
