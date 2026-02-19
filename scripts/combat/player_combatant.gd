extends Combatant
class_name PlayerCombatant

## 플레이어 전투 상태: 에너지, 드로우 더미, 버린 더미, 손패 관리

signal energy_changed(current: int, maximum: int)
signal hand_changed(hand: Array)
signal draw_pile_changed(count: int)
signal discard_pile_changed(count: int)

const MAX_HAND_SIZE: int = 10
const DRAW_PER_TURN: int = 5
const MAX_ENERGY: int = 3

var max_energy: int = MAX_ENERGY
var current_energy: int = 0

var _draw_pile: Deck = Deck.new()
var _discard_pile: Deck = Deck.new()
var _hand: Array[CardData] = []

## 전투 시작 시 덱 초기화
func setup_deck(cards: Array[CardData]) -> void:
	_draw_pile = Deck.new()
	_discard_pile = Deck.new()
	_hand.clear()
	for card in cards:
		_draw_pile.add_card(card)
	_draw_pile.shuffle()
	draw_pile_changed.emit(_draw_pile.size())
	discard_pile_changed.emit(_discard_pile.size())
	hand_changed.emit(_hand)


func start_turn() -> void:
	reset_block()
	current_energy = max_energy
	energy_changed.emit(current_energy, max_energy)
	draw_cards(DRAW_PER_TURN)


func end_turn() -> void:
	# 손패 전체를 버린 더미로 이동
	for card in _hand:
		_discard_pile.add_card(card)
	_hand.clear()
	tick_statuses_end_of_turn()
	hand_changed.emit(_hand)
	discard_pile_changed.emit(_discard_pile.size())


func draw_cards(count: int) -> void:
	for i in count:
		if _hand.size() >= MAX_HAND_SIZE:
			break
		_refill_draw_if_empty()
		if _draw_pile.is_empty():
			break
		var card := _draw_pile.draw()
		_hand.append(card)
	hand_changed.emit(_hand)
	draw_pile_changed.emit(_draw_pile.size())


func _refill_draw_if_empty() -> void:
	if not _draw_pile.is_empty():
		return
	if _discard_pile.is_empty():
		return
	while not _discard_pile.is_empty():
		_draw_pile.add_card(_discard_pile.draw())
	_draw_pile.shuffle()
	discard_pile_changed.emit(_discard_pile.size())
	draw_pile_changed.emit(_draw_pile.size())


## 카드를 손패에서 제거하고 버린 더미로 보낸다.
func play_card(card_data: CardData) -> bool:
	if not can_play_card(card_data):
		return false
	current_energy -= card_data.cost
	energy_changed.emit(current_energy, max_energy)
	_hand.erase(card_data)
	_discard_pile.add_card(card_data)
	hand_changed.emit(_hand)
	discard_pile_changed.emit(_discard_pile.size())
	return true


func can_play_card(card_data: CardData) -> bool:
	return card_data.playable and current_energy >= card_data.cost and _hand.has(card_data)


## Wild Strike 등 드로우 더미에 카드 추가
func add_card_to_draw(card_data: CardData) -> void:
	_draw_pile.add_card(card_data)
	_draw_pile.shuffle()
	draw_pile_changed.emit(_draw_pile.size())


func get_hand() -> Array[CardData]:
	return _hand


func draw_pile_count() -> int:
	return _draw_pile.size()


func discard_pile_count() -> int:
	return _discard_pile.size()
