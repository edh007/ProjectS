extends RefCounted
class_name StatusEffect

## 상태이상 타입과 스택 관리

enum Type {
	STRENGTH,    # 공격력 증가 (영구)
	DEXTERITY,   # 블록 증가 (영구)
	VULNERABLE,  # 받는 피해 1.5배 (턴마다 감소)
	WEAK,        # 주는 피해 0.75배 (턴마다 감소)
	POISON,      # 턴 시작 시 N 피해 후 스택 1 감소
}

var type: Type
var stacks: int

func _init(t: Type, s: int) -> void:
	type = t
	stacks = s


static func type_name(t: Type) -> String:
	match t:
		Type.STRENGTH:   return "힘"
		Type.DEXTERITY:  return "민첩"
		Type.VULNERABLE: return "취약"
		Type.WEAK:       return "약화"
		Type.POISON:     return "독"
	return "Unknown"


## 턴 종료 시 감소 처리. true를 반환하면 제거할 것.
func tick_end_of_turn() -> bool:
	match type:
		Type.VULNERABLE, Type.WEAK:
			stacks -= 1
			return stacks <= 0
		Type.POISON:
			# 포이즌 tick은 EffectHandler가 처리하므로 여기서는 스택만 감소
			stacks -= 1
			return stacks <= 0
	return false
