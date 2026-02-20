# Plan: Slay the Spire 핵심 메커니즘 구현

> 작성일: 2026-02-20
> 브랜치: `feat/slay-the-spire-core`
> 커밋: `90f2120`

---

## 배경 및 목표

Godot 4.x 카드게임 프로젝트에 Slay the Spire의 핵심 메커니즘을 실제 플레이 가능한 수준으로 구현한다.
계획 수립 시점에서 프로젝트는 스크립트 골격(`card.gd`, `card_hand.gd`, `deck.gd`, `game_state.gd` 등)만 존재하고 `.tscn` 씬 파일이 전혀 없는 상태였다.

### 구현 목표
- 에너지 기반 카드 플레이 시스템
- Draw / Discard 덱 사이클링
- 상태이상 (Strength, Dexterity, Vulnerable, Weak, Poison)
- 적 인텐트 시스템 (패턴 AI)
- 3개 방 선형 로그라이크 (전투→보상→전투→보상→보스→승리)
- 13종 카드, 3종 적

---

## 파일 목록

### Phase 1 — 데이터 레이어
| 파일 | 작업 |
|------|------|
| `resources/card_data.gd` | **수정**: CardType/TargetType enum, effect_params: Dictionary, playable 추가 |
| `resources/enemy_move.gd` | **신규**: EnemyMove extends Resource |
| `resources/enemy_data.gd` | **신규**: EnemyData extends Resource |
| `scripts/combat/status_effect.gd` | **신규**: StatusEffect.Type enum |
| `resources/cards/*.tres` | **신규**: 카드 13종 데이터 파일 |
| `resources/enemies/*.tres` | **신규**: 적 3종 데이터 파일 |

### Phase 2 — 전투 로직
| 파일 | 작업 |
|------|------|
| `scripts/combat/combatant.gd` | **신규**: HP/Block/Status 베이스 클래스 |
| `scripts/combat/player_combatant.gd` | **신규**: 에너지, 덱 사이클링 |
| `scripts/combat/enemy_combatant.gd` | **신규**: 인텐트 FSM, AI |
| `scripts/combat/effect_handler.gd` | **신규**: 피해 계산 + 카드/적 효과 실행 |
| `scripts/combat/combat_manager.gd` | **신규**: 턴 FSM 제어 |
| `scripts/deck.gd` | **유지**: PlayerCombatant에서 두 인스턴스로 재사용 |
| `scripts/card_hand.gd` | **수정**: card_played 시그널, get_all_card_data / clear_hand / refresh_playability 추가 |
| `scripts/card.gd` | **수정**: TypeBadge 노드 참조, set_playable(bool) 반투명 처리, _ready() 시그널 연결 |

### Phase 3 — 게임 흐름
| 파일 | 작업 |
|------|------|
| `scripts/run/run_manager.gd` | **신규**: 방 순서, 씬 전환 이벤트 (Autoload 등록) |
| `autoloads/game_state.gd` | **수정**: State enum에 COMBAT / REWARD / BOSS / VICTORY 추가, player_deck / player_hp 필드 |
| `project.godot` | **수정**: RunManager autoload 추가 |

### Phase 4 — 씬 파일
| 파일 | 작업 |
|------|------|
| `scenes/ui/main_menu.tscn` | **신규** |
| `scenes/cards/card.tscn` | **신규** |
| `scenes/enemies/enemy_view.tscn` + `scripts/ui/enemy_view.gd` | **신규** |
| `scenes/game/battle.tscn` + `scripts/battle_scene.gd` | **신규** |
| `scripts/ui/battle_hud.gd` | **신규**: 에너지/HP/블록/덱 카운트 HUD |
| `scenes/game/reward.tscn` + `scripts/reward_scene.gd` | **신규** |
| `scenes/game/game_over.tscn` + `scripts/game_over_scene.gd` | **신규** |
| `scenes/game/victory.tscn` + `scripts/victory_scene.gd` | **신규** |

---

## 핵심 설계

### 피해 계산식
```
base  = card_damage + attacker.Strength
dmg   = floor(base × 0.75)    # attacker에 Weak 있을 때
final = ceil(dmg  × 1.5)      # target에 Vulnerable 있을 때
final = max(0, final)
```

### 덱 사이클
```
draw_pile (Deck) → hand (Array[CardData]) → discard_pile (Deck)
draw_pile 소진 시: discard_pile 전체를 draw_pile로 셔플 이동
```

### 전투 턴 FSM
```
PLAYER_TURN → (EndTurn 버튼) → ENEMY_TURN → (모든 적 행동 완료) → PLAYER_TURN
전투 종료 조건: 모든 적 HP=0 (승리) / 플레이어 HP=0 (패배)
```

### 로그라이크 진행 순서
```
방 0: COMBAT  (Cultist)
방 1: REWARD  (3장 선택)
방 2: COMBAT  (Jaw Worm)
방 3: REWARD  (3장 선택)
방 4: COMBAT  (Slime Boss) ← BOSS 상태
방 5: VICTORY
```

---

## 카드 목록

| ID | 이름 | 비용 | 효과 |
|----|------|:----:|------|
| strike | Strike | 1 | 6 피해 |
| defend | Defend | 1 | 5 블록 |
| bash | Bash | 2 | 8 피해 + 취약 2턴 |
| iron_wave | Iron Wave | 1 | 5 피해 + 5 블록 |
| thunderclap | Thunderclap | 1 | 전체 4 피해 + 취약 1턴 |
| twin_strike | Twin Strike | 1 | 5 피해 × 2 |
| clothesline | Clothesline | 2 | 14 피해 + 약화 2턴 |
| shrug_it_off | Shrug It Off | 1 | 8 블록 + 드로우 1 |
| pommel_strike | Pommel Strike | 1 | 9 피해 + 드로우 1 |
| inflame | Inflame | 1 | Power: 힘 +2 (영구) |
| cleave | Cleave | 1 | 전체 8 피해 |
| wild_strike | Wild Strike | 1 | 12 피해 + 상처를 드로우 더미에 추가 |
| wound | Wound | — | 플레이 불가 (패 막힘용 상태이상) |

**초기 덱 구성**: Strike × 5, Defend × 4, Bash × 1

---

## 적 목록

| ID | 이름 | HP | 패턴 |
|----|------|:--:|------|
| cultist | Cultist | 50 | 1턴: 힘 +3 / 이후: 6 피해 반복 |
| jaw_worm | Jaw Worm | 42 | Chomp(11) → Thrash(7 피해+블록5) → Bellow(힘+3, 블록+6) 순환 |
| slime_boss | Slime Boss | 140 | Slam(35×2) ↔ Preparing(스킵) 반복 |

---

## 시그널 흐름

```
Card.clicked
  → CardHand._on_card_clicked
  → CardHand.card_played (emit)
  → BattleScene._on_card_played
  → CombatManager.on_card_played
  → EffectHandler.apply_card
  → EnemyCombatant.take_damage
  → EnemyView (hp_changed 시그널로 자동 갱신)

EndTurnButton.pressed
  → BattleHUD.end_turn_pressed (emit)
  → CombatManager.end_player_turn
  → PlayerCombatant.end_turn (손패 → discard)
  → EnemyCombatant.execute_move (각 적 순서대로)
  → EffectHandler.apply_enemy_move
  → CombatManager._begin_player_turn (새 턴 시작)

CombatManager.combat_ended
  → BattleScene._on_combat_ended
  → GameState.player_hp 유지
  → RunManager.complete_combat
  → RunManager._go_to_room (다음 방 씬 전환)
```

---

## 씬 노드 구조

### `battle.tscn`
```
BattleScene (Node2D) [battle_scene.gd]
├── Background (ColorRect)
├── EnemyArea (HBoxContainer)
│   └── EnemyView (enemy_view.tscn 인스턴스, 동적 추가)
├── HandArea (Node2D) [card_hand.gd]  ← card_scene: card.tscn
├── LogLabel (Label)
├── BattleHUD (CanvasLayer) [battle_hud.gd]
│   └── HUDContainer (HBoxContainer)
│       ├── EnergyLabel
│       ├── PlayerStatus / HPLabel, BlockLabel
│       ├── DrawPileButton, DiscardPileButton
│       ├── TurnLabel
│       └── EndTurnButton
└── CombatManager (Node) [combat_manager.gd]
```

### `card.tscn`
```
Card (Control) [card.gd]
└── PanelContainer
    └── VBoxContainer
        ├── CostLabel
        ├── Artwork (TextureRect)
        ├── NameLabel
        ├── TypeBadge
        └── DescLabel
```

### `enemy_view.tscn`
```
EnemyView (VBoxContainer) [enemy_view.gd]
├── NameLabel
├── EnemySprite (ColorRect)
├── HPLabel
├── BlockLabel
└── IntentLabel
```

---

## 검증 계획

1. 메인 메뉴 → Start 버튼 → `battle.tscn` 이동 확인
2. 턴 시작 시 카드 5장 드로우 확인
3. 카드 플레이 → 에너지 감소, 효과 적용, 버린 더미 이동 확인
4. 적 턴 → 인텐트 표시 내용과 실제 행동 일치 확인
5. 드로우 더미 소진 → 버린 더미 셔플 재사용 확인
6. 전투 승리 → 보상 씬 → 카드 선택 → 다음 전투 확인
7. HP 0 → 게임 오버 씬 확인
8. Slime Boss 격파 → 승리 씬 확인
