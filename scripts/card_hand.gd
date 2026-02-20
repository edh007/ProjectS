extends Node2D
class_name CardHand

## 플레이어 손패 관리
## 카드 추가/제거 및 가로 배치를 담당한다.

const MAX_HAND_SIZE: int = 10

signal card_played(card_data: CardData)

@export var card_scene: PackedScene

var _hand: Array[CardData] = []
var _card_nodes: Array[Node] = []


func add_card(card_data: CardData) -> bool:
	if _hand.size() >= MAX_HAND_SIZE:
		return false
	_hand.append(card_data)
	_spawn_card_node(card_data)
	_arrange_cards()
	return true


func remove_card(card_data: CardData) -> void:
	var idx := _hand.find(card_data)
	if idx < 0:
		return
	_hand.remove_at(idx)
	if idx < _card_nodes.size():
		_card_nodes[idx].queue_free()
		_card_nodes.remove_at(idx)
	_arrange_cards()


func clear_hand() -> void:
	for node in _card_nodes:
		node.queue_free()
	_card_nodes.clear()
	_hand.clear()


func get_all_card_data() -> Array[CardData]:
	return _hand


func hand_size() -> int:
	return _hand.size()


func refresh_playability(can_play_func: Callable) -> void:
	for i in _card_nodes.size():
		if i < _hand.size() and _card_nodes[i].has_method("set_playable"):
			_card_nodes[i].set_playable(can_play_func.call(_hand[i]))


func _spawn_card_node(card_data: CardData) -> void:
	if card_scene == null:
		return
	var card_node := card_scene.instantiate()
	add_child(card_node)
	_card_nodes.append(card_node)
	if card_node.has_method("setup"):
		card_node.setup(card_data)
	if card_node.has_signal("clicked"):
		card_node.clicked.connect(_on_card_clicked)


func _on_card_clicked(card_node: Node) -> void:
	var card_data: CardData = card_node.data if card_node.get("data") != null else null
	if card_data == null:
		return
	card_played.emit(card_data)


func _arrange_cards() -> void:
	var count := _card_nodes.size()
	if count == 0:
		return
	var spread: float = min(120.0, 800.0 / count)
	var start_x: float = -(count - 1) * spread / 2.0
	for i in count:
		_card_nodes[i].position.x = start_x + i * spread
