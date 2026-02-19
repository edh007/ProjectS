extends Resource
class_name EnemyData

## 적 하나의 기본 데이터 정의

@export var id: String = ""
@export var enemy_name: String = ""
@export var max_hp: int = 0
## move_pattern: EnemyMove의 배열. 인덱스 순서대로 순환한다.
@export var move_pattern: Array[EnemyMove] = []
## first_move_index: 첫 턴에 사용할 move_pattern 인덱스
## -1이면 패턴 순서대로 (0번부터)
@export var first_move_index: int = -1
