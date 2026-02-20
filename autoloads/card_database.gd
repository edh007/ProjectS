extends Node

## 카드 데이터 싱글턴
## 모든 카드 정의를 로드하고 ID로 조회할 수 있다.

var _cards: Dictionary = {}


func _ready() -> void:
	_load_cards()


func get_card(card_id: String) -> CardData:
	return _cards.get(card_id, null)


func get_all_cards() -> Array[CardData]:
	var result: Array[CardData] = []
	result.assign(_cards.values())
	return result


func _load_cards() -> void:
	# res://resources/cards/ 폴더 안의 모든 .tres 파일을 로드한다.
	var dir := DirAccess.open("res://resources/cards/")
	if dir == null:
		return
	dir.list_dir_begin()
	var file_name := dir.get_next()
	while file_name != "":
		if file_name.ends_with(".tres"):
			var card := load("res://resources/cards/" + file_name) as CardData
			if card != null:
				_cards[card.id] = card
		file_name = dir.get_next()
