extends RefCounted
class_name Deck

## 덱(카드 더미) 로직
## 카드 추가, 셔플, 드로우를 담당한다.

var _cards: Array[CardData] = []


func add_card(card: CardData) -> void:
	_cards.append(card)


func shuffle() -> void:
	_cards.shuffle()


func draw() -> CardData:
	if _cards.is_empty():
		return null
	return _cards.pop_back()


func size() -> int:
	return _cards.size()


func is_empty() -> bool:
	return _cards.is_empty()
