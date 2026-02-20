extends Resource
class_name CardData

## 카드 하나의 데이터 정의 (Resource로 .tres 파일에 저장된다)

enum CardType { ATTACK, SKILL, POWER, STATUS }
enum TargetType { SINGLE, ALL_ENEMIES, SELF, NONE }

@export var id: String = ""
@export var card_name: String = ""
@export var description: String = ""
@export var cost: int = 0
@export var card_type: CardType = CardType.ATTACK
@export var target_type: TargetType = TargetType.SINGLE
@export var artwork: Texture2D = null
## effect_params 예시:
## { "damage": 6 }
## { "block": 5 }
## { "damage": 8, "apply_vulnerable": 2 }
## { "draw": 1 }
## { "strength": 2 }
@export var effect_params: Dictionary = {}
@export var playable: bool = true
