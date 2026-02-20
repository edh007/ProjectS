extends Resource
class_name EnemyMove

## 적의 행동 하나를 정의하는 Resource

enum MoveType { ATTACK, DEFEND, BUFF, SKIP }

@export var move_name: String = ""
@export var move_type: MoveType = MoveType.ATTACK
## move_params 예시:
## { "damage": 11 }
## { "block": 5 }
## { "strength": 3 }
## { "damage": 35, "hits": 2 }
@export var move_params: Dictionary = {}
## intent_icon: "attack", "defend", "buff", "unknown"
@export var intent_icon: String = "attack"
