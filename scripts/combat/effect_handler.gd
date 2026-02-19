extends RefCounted
class_name EffectHandler

## 피해 계산 및 카드/적 효과 실행

## 피해 계산식:
## base = card_damage + attacker.Strength
## after_weak = floor(base × 0.75)   # attacker에 Weak 있을 때
## final = ceil(after_weak × 1.5)    # target에 Vulnerable 있을 때
## final = max(0, final)
static func calculate_damage(base: int, attacker: Combatant, target: Combatant) -> int:
	var dmg: float = base + attacker.get_status_stacks(StatusEffect.Type.STRENGTH)
	if attacker.has_status(StatusEffect.Type.WEAK):
		dmg = floor(dmg * 0.75)
	if target.has_status(StatusEffect.Type.VULNERABLE):
		dmg = ceil(dmg * 1.5)
	return maxi(0, int(dmg))


## 카드 효과 적용
## player: PlayerCombatant, targets: Array[EnemyCombatant]
## card_data: 플레이할 카드
static func apply_card(
	card_data: CardData,
	player: PlayerCombatant,
	targets: Array
) -> void:
	var p := card_data.effect_params
	var hits: int = p.get("hits", 1)

	match card_data.target_type:
		CardData.TargetType.SINGLE:
			if targets.is_empty():
				return
			var target: EnemyCombatant = targets[0]
			# 피해
			if p.has("damage"):
				for _i in hits:
					var dmg := calculate_damage(p["damage"], player, target)
					target.take_damage(dmg)
			# 상태이상 적용
			_apply_debuffs(p, target)

		CardData.TargetType.ALL_ENEMIES:
			for target in targets:
				if p.has("damage"):
					for _i in hits:
						var dmg := calculate_damage(p["damage"], player, target)
						target.take_damage(dmg)
				_apply_debuffs(p, target)

		CardData.TargetType.SELF, CardData.TargetType.NONE:
			pass

	# 플레이어 효과
	if p.has("block"):
		var blk: int = p["block"] + player.get_status_stacks(StatusEffect.Type.DEXTERITY)
		player.add_block(maxi(0, blk))

	if p.has("strength"):
		player.add_status(StatusEffect.Type.STRENGTH, p["strength"])

	if p.has("draw"):
		player.draw_cards(p["draw"])

	if p.get("add_wound_to_draw", false):
		var wound: CardData = CardDatabase.get_card("wound")
		if wound != null:
			player.add_card_to_draw(wound)


static func _apply_debuffs(p: Dictionary, target: EnemyCombatant) -> void:
	if p.has("apply_vulnerable"):
		target.add_status(StatusEffect.Type.VULNERABLE, p["apply_vulnerable"])
	if p.has("apply_weak"):
		target.add_status(StatusEffect.Type.WEAK, p["apply_weak"])
	if p.has("apply_poison"):
		target.add_status(StatusEffect.Type.POISON, p["apply_poison"])


## 적 행동 효과 적용
static func apply_enemy_move(
	move: EnemyMove,
	enemy: EnemyCombatant,
	player: PlayerCombatant
) -> void:
	var p := move.move_params
	match move.move_type:
		EnemyMove.MoveType.ATTACK:
			var hits: int = p.get("hits", 1)
			if p.has("damage"):
				for _i in hits:
					var dmg := calculate_damage(p["damage"], enemy, player)
					player.take_damage(dmg)

		EnemyMove.MoveType.DEFEND:
			if p.has("block"):
				enemy.add_block(p["block"])

		EnemyMove.MoveType.BUFF:
			if p.has("strength"):
				enemy.add_status(StatusEffect.Type.STRENGTH, p["strength"])
			if p.has("block"):
				enemy.add_block(p["block"])
			if p.has("dexterity"):
				enemy.add_status(StatusEffect.Type.DEXTERITY, p["dexterity"])

		EnemyMove.MoveType.SKIP:
			pass
