extends Node
class_name GameManager

## 한 판의 게임 흐름을 제어하는 매니저

signal turn_started(turn_number: int)
signal game_ended(player_won: bool)

@export var hand: CardHand

var _deck: Deck
var _turn: int = 0
var _draw_count: int = 5


func start_game(card_ids: Array[String]) -> void:
	_deck = Deck.new()
	for id in card_ids:
		var card := CardDatabase.get_card(id)
		if card != null:
			_deck.add_card(card)
	_deck.shuffle()
	_start_turn()


func _start_turn() -> void:
	_turn += 1
	turn_started.emit(_turn)
	_draw_cards(_draw_count)


func _draw_cards(count: int) -> void:
	for i in count:
		if _deck.is_empty():
			break
		var card := _deck.draw()
		hand.add_card(card)


func end_turn() -> void:
	_start_turn()


func end_game(player_won: bool) -> void:
	game_ended.emit(player_won)
	GameState.change_state(GameState.State.GAME_OVER)
