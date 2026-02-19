extends Resource
class_name CardData

## 카드 하나의 데이터 정의 (Resource로 .tres 파일에 저장된다)

@export var id: String = ""
@export var card_name: String = ""
@export var description: String = ""
@export var cost: int = 0
@export var attack: int = 0
@export var defense: int = 0
@export var artwork: Texture2D = null
