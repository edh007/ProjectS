extends Node2D
class_name CardHand

## 플레이어 손패 관리
## 카드 추가/제거 및 부채꼴 배치를 담당한다.

const MAX_HAND_SIZE: int = 10

signal card_played(card_data: CardData)

@export var card_scene: PackedScene

var _hand: Array[CardData] = []


func add_card(card_data: CardData) -> bool:
	if _hand.size() >= MAX_HAND_SIZE:
		return false
	_hand.append(card_data)
	_spawn_card_node(card_data)
	_arrange_cards()
	return true


func remove_card(card_data: CardData) -> void:
	_hand.erase(card_data)
	_arrange_cards()


func hand_size() -> int:
	return _hand.size()


func _spawn_card_node(card_data: CardData) -> void:
	if card_scene == null:
		return
	var card_node: Node = card_scene.instantiate()
	add_child(card_node)
	# card_node에 card_data를 전달하는 방식은 Card 씬 구현에 맞게 수정한다.
	if card_node.has_method("setup"):
		card_node.setup(card_data)


func _arrange_cards() -> void:
	# 부채꼴 배치: 카드 수에 따라 간격을 조정한다.
	var count := get_child_count()
	if count == 0:
		return
	var spread: float = min(120.0, 800.0 / count)
	var start_x: float = -(count - 1) * spread / 2.0
	for i in count:
		get_child(i).position.x = start_x + i * spread
