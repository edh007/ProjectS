# C++ Core + GDExtension 마이그레이션 계획서

> 작성일: 2026-02-21
> 목표: 게임 로직을 엔진 무관 C++ 코어로 분리하고, GDExtension으로 Godot에 연결한다.
> 재사용 대상: Unreal Engine, 자체 엔진 등

---

## 0. 목표 아키텍처

```
┌─────────────────────────────────────────────────┐
│              GDScript (UI/씬 관리)               │
│  card.gd, card_hand.gd, battle_hud.gd,          │
│  battle_scene.gd, run_manager.gd, ...            │
└────────────────────┬────────────────────────────┘
                     │ 호출 / 시그널 수신
┌────────────────────▼────────────────────────────┐
│         GDExtension 래퍼 (godot-cpp)             │
│  GDCombatant, GDPlayerCombatant,                 │
│  GDCombatManager, GDCardDatabase, ...            │
│  (C++ 콜백 → Godot 시그널 변환)                  │
└────────────────────┬────────────────────────────┘
                     │ 소유 / 위임
┌────────────────────▼────────────────────────────┐
│         C++ Core (엔진 무관, STL만 사용)          │
│  CardData, Deck, Combatant, PlayerCombatant,     │
│  EnemyCombatant, CombatManager, EffectHandler,   │
│  StatusEffect, CardDatabase (캐시)               │
└─────────────────────────────────────────────────┘
         ↕ 동일한 core/ 를 재사용
┌─────────────────────────────────────────────────┐
│         Unreal 래퍼 (미래 작업)                  │
│  UCombatant, UCombatManager, ...                 │
│  (C++ 콜백 → Unreal Delegate 변환)               │
└─────────────────────────────────────────────────┘
```

---

## 1. 최종 디렉터리 구조

```
ProjectS/
│
├── core/                          ← 순수 C++ (엔진 의존성 없음)
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── card_data.h
│   │   ├── effect_params.h        ← Dictionary 대체 typed struct
│   │   ├── enemy_data.h
│   │   ├── enemy_move.h
│   │   ├── status_effect.h
│   │   ├── deck.h
│   │   ├── combatant.h
│   │   ├── player_combatant.h
│   │   ├── enemy_combatant.h
│   │   ├── effect_handler.h
│   │   ├── combat_manager.h
│   │   └── i_card_database.h      ← EffectHandler 의존성 역전용 인터페이스
│   └── src/
│       ├── status_effect.cpp
│       ├── deck.cpp
│       ├── combatant.cpp
│       ├── player_combatant.cpp
│       ├── enemy_combatant.cpp
│       ├── effect_handler.cpp
│       └── combat_manager.cpp
│
├── godot_extension/               ← GDExtension 래퍼 (godot-cpp 사용)
│   ├── CMakeLists.txt
│   ├── godot-cpp/                 ← git submodule
│   ├── src/
│   │   ├── register_types.cpp
│   │   ├── register_types.h
│   │   ├── gd_card_data.h/.cpp
│   │   ├── gd_combatant.h/.cpp
│   │   ├── gd_player_combatant.h/.cpp
│   │   ├── gd_enemy_combatant.h/.cpp
│   │   ├── gd_combat_manager.h/.cpp
│   │   └── gd_card_database.h/.cpp
│   └── project.gdextension        ← Godot에서 로드할 설정 파일
│
├── autoloads/                     ← 기존 유지 (씬 관리)
├── scripts/                       ← GDScript 얇은 래퍼로 교체
├── resources/                     ← .tres 파일 유지
├── scenes/                        ← 씬 파일 유지
└── docs/
    └── plan-cpp-gdextension.md    ← 이 파일
```

---

## 2. 핵심 설계 결정

### 2-1. 시그널 추상화
C++ 코어는 `std::function` 콜백을 사용한다.
GDExtension 래퍼가 이를 Godot 시그널로 변환한다.

```cpp
// core: std::function 콜백
class Combatant {
public:
    std::function<void(int cur, int max)> on_hp_changed;
    std::function<void()>                 on_died;
};

// GDExtension 래퍼: 콜백 → Godot 시그널
class GDCombatant : public godot::RefCounted {
    GDCLASS(GDCombatant, RefCounted)
    core::Combatant _core;
public:
    void _ready() {
        _core.on_hp_changed = [this](int c, int m) {
            emit_signal("hp_changed", c, m);
        };
    }
};
```

### 2-2. Dictionary → Typed Struct
`effect_params: Dictionary`를 타입 안전한 구조체로 교체한다.

```cpp
// core/include/effect_params.h
struct CardEffectParams {
    int  damage          = 0;
    int  block           = 0;
    int  hits            = 1;
    int  draw            = 0;
    int  strength        = 0;
    int  apply_vulnerable = 0;
    int  apply_weak      = 0;
    int  apply_poison    = 0;
    bool exhaust         = false;
};

struct EnemyMoveParams {
    int damage = 0;
    int block  = 0;
    int hits   = 1;
    int buff_strength = 0;
};
```

### 2-3. CardDatabase 의존성 역전
`EffectHandler`가 `CardDatabase`를 직접 참조하지 않도록 인터페이스를 사용한다.

```cpp
// core/include/i_card_database.h
class ICardDatabase {
public:
    virtual ~ICardDatabase() = default;
    virtual const CardData* get_card(const std::string& id) const = 0;
};

// EffectHandler가 인터페이스 포인터만 받음
class EffectHandler {
public:
    static void set_database(ICardDatabase* db);
    static void apply_card(...);
};
```

### 2-4. 빌드 시스템
- **C++ Core**: CMake (독립 정적 라이브러리 `libgame_core.a`)
- **GDExtension**: CMake + godot-cpp (`.so`/`.dll` 공유 라이브러리)
- **Unreal (미래)**: C++ Core를 Unreal 빌드 시스템에 직접 포함

---

## 3. 파일별 마이그레이션 분류

### A. C++ Core로 완전 이전

| GDScript 파일 | C++ 파일 | 비고 |
|---|---|---|
| `resources/card_data.gd` | `core/include/card_data.h` | Texture2D 제거, artwork는 path(string)으로 |
| `resources/enemy_data.gd` | `core/include/enemy_data.h` | |
| `resources/enemy_move.gd` | `core/include/enemy_move.h` | move_params → EnemyMoveParams struct |
| `scripts/combat/status_effect.gd` | `core/include/status_effect.h` + `.cpp` | tick_end_of_turn 로직 이전 |
| `scripts/deck.gd` | `core/include/deck.h` + `.cpp` | std::vector + std::shuffle |
| `scripts/combat/combatant.gd` | `core/include/combatant.h` + `.cpp` | signal → std::function 콜백 |
| `scripts/combat/player_combatant.gd` | `core/include/player_combatant.h` + `.cpp` | |
| `scripts/combat/enemy_combatant.gd` | `core/include/enemy_combatant.h` + `.cpp` | |
| `scripts/combat/effect_handler.gd` | `core/include/effect_handler.h` + `.cpp` | ICardDatabase 주입 |
| `scripts/combat/combat_manager.gd` | `core/include/combat_manager.h` + `.cpp` | CardHand 참조 제거 |

### B. GDExtension 래퍼 작성

| C++ Core 클래스 | GDExtension 래퍼 | Godot 노드 타입 |
|---|---|---|
| `core::Combatant` | `GDCombatant` | `RefCounted` |
| `core::PlayerCombatant` | `GDPlayerCombatant` | `RefCounted` |
| `core::EnemyCombatant` | `GDEnemyCombatant` | `RefCounted` |
| `core::CombatManager` | `GDCombatManager` | `Node` |
| `core::CardData` (struct) | `GDCardData` | `Resource` |
| `core::ICardDatabase` 구현 | `GDCardDatabase` | `Node` (Autoload) |

### C. GDScript 유지 (변경 없음 또는 소폭 수정)

| 파일 | 이유 |
|---|---|
| `scripts/card.gd` | Control 상속, 렌더링/입력 |
| `scripts/card_hand.gd` | Node2D, 씬 인스턴스화 |
| `scripts/ui/battle_hud.gd` | UI 레이어 |
| `scripts/ui/enemy_view.gd` | UI 레이어 |
| `scripts/battle_scene.gd` | 씬 오케스트레이터 |
| `scripts/run/run_manager.gd` | 씬 전환 (`change_scene_to_file`) |
| `autoloads/game_state.gd` | Autoload 구조 유지 |
| `scripts/reward_scene.gd` 등 | 씬 UI 글루 |

---

## 4. 단계별 구현 계획

### Phase 0. 환경 설정
**목표**: 빌드 환경 구축, godot-cpp 연동 확인

- [ ] godot-cpp를 `godot_extension/godot-cpp/`에 git submodule로 추가
- [ ] `core/CMakeLists.txt` 작성 (정적 라이브러리)
- [ ] `godot_extension/CMakeLists.txt` 작성 (공유 라이브러리, core 링크)
- [ ] `project.gdextension` 파일 작성
- [ ] 빈 GDExtension 클래스 하나를 빌드하여 Godot에서 로드 확인

**산출물**: 빌드 가능한 빈 GDExtension, `Hello from C++` 출력 확인

---

### Phase 1. C++ Core 데이터 구조체
**목표**: 순수 데이터 타입 정의 (의존성 없음)

- [ ] `effect_params.h` — CardEffectParams, EnemyMoveParams struct
- [ ] `card_data.h` — CardData struct (enum CardType, TargetType 포함)
- [ ] `enemy_move.h` — EnemyMove struct (enum MoveType 포함)
- [ ] `enemy_data.h` — EnemyData struct (std::vector<EnemyMove>)
- [ ] `status_effect.h/.cpp` — StatusEffect class, tick_end_of_turn()
- [ ] `i_card_database.h` — ICardDatabase 순수 인터페이스

**검증**: 단위 테스트로 StatusEffect tick 로직 확인

---

### Phase 2. C++ Core 로직 레이어
**목표**: 전투 핵심 로직 이전 (std::function 콜백 기반)

- [ ] `deck.h/.cpp` — Deck<T> 또는 Deck (CardData 전용)
- [ ] `combatant.h/.cpp` — Combatant base (콜백: on_hp_changed, on_died 등)
- [ ] `player_combatant.h/.cpp` — PlayerCombatant (Deck 3개, 에너지 관리)
- [ ] `enemy_combatant.h/.cpp` — EnemyCombatant (AI 패턴 순환)
- [ ] `effect_handler.h/.cpp` — EffectHandler namespace (ICardDatabase 주입)
- [ ] `combat_manager.h/.cpp` — CombatManager FSM (콜백: on_combat_ended, on_turn_changed)

**검증**: C++ 전용 테스트 main으로 전투 1회 시뮬레이션 (렌더링 없이)

---

### Phase 3. GDExtension 래퍼
**목표**: C++ Core를 GDScript에서 사용 가능하도록 노출

- [ ] `register_types.cpp` — 모든 GD 클래스 등록
- [ ] `gd_card_data.h/.cpp` — GDCardData (Resource 상속, .tres 호환)
- [ ] `gd_combatant.h/.cpp` — GDCombatant (RefCounted, hp_changed 시그널)
- [ ] `gd_player_combatant.h/.cpp` — GDPlayerCombatant (energy/hand/pile 시그널)
- [ ] `gd_enemy_combatant.h/.cpp` — GDEnemyCombatant (intent_changed 시그널)
- [ ] `gd_combat_manager.h/.cpp` — GDCombatManager (Node, combat_ended/turn_changed 시그널)
- [ ] `gd_card_database.h/.cpp` — GDCardDatabase (ICardDatabase 구현, .tres 로드)

**검증**: GDScript에서 GDCombatManager 인스턴스 생성 및 시그널 수신 확인

---

### Phase 4. GDScript 교체
**목표**: 기존 GDScript를 GDExtension 클래스를 사용하는 얇은 래퍼로 교체

- [ ] `battle_scene.gd` — GDPlayerCombatant, GDCombatManager 사용으로 교체
- [ ] `autoloads/card_database.gd` — GDCardDatabase로 위임
- [ ] `scripts/ui/battle_hud.gd` — GDPlayerCombatant 시그널 연결 방식 유지
- [ ] `scripts/ui/enemy_view.gd` — GDEnemyCombatant 시그널 연결 방식 유지

**검증**: 기존 게임 플레이와 동일하게 동작 확인

---

### Phase 5. 정리 및 문서화
**목표**: 재사용 가능성 검증, Unreal 연동 가이드 작성

- [ ] core/ 독립 빌드 확인 (Godot 없이 CMake만으로)
- [ ] Unreal 래퍼 프로토타입 인터페이스 설계 (UCombatManager 스켈레톤)
- [ ] README 업데이트 (빌드 방법, 구조 설명)

---

## 5. 시그널 대응표

GDScript의 기존 시그널이 GDExtension에서 그대로 유지되어 GDScript 호출자는 변경 없다.

| GDScript 시그널 | C++ Core 콜백 | GDExtension 시그널 |
|---|---|---|
| `hp_changed(cur, max)` | `on_hp_changed` | `ADD_SIGNAL("hp_changed")` |
| `block_changed(cur)` | `on_block_changed` | `ADD_SIGNAL("block_changed")` |
| `died()` | `on_died` | `ADD_SIGNAL("died")` |
| `status_changed(type, stacks)` | `on_status_changed` | `ADD_SIGNAL("status_changed")` |
| `energy_changed(cur, max)` | `on_energy_changed` | `ADD_SIGNAL("energy_changed")` |
| `hand_changed(hand)` | `on_hand_changed` | `ADD_SIGNAL("hand_changed")` |
| `draw_pile_changed(count)` | `on_draw_pile_changed` | `ADD_SIGNAL("draw_pile_changed")` |
| `discard_pile_changed(count)` | `on_discard_pile_changed` | `ADD_SIGNAL("discard_pile_changed")` |
| `intent_changed(move)` | `on_intent_changed` | `ADD_SIGNAL("intent_changed")` |
| `combat_ended(result)` | `on_combat_ended` | `ADD_SIGNAL("combat_ended")` |
| `turn_changed(turn)` | `on_turn_changed` | `ADD_SIGNAL("turn_changed")` |
| `log_message(text)` | `on_log_message` | `ADD_SIGNAL("log_message")` |

---

## 6. C++ Core 의존성 그래프 (구현 순서)

```
[Phase 1]
effect_params.h     (의존성 없음)
card_data.h         (effect_params.h)
enemy_move.h        (effect_params.h)
enemy_data.h        (enemy_move.h)
status_effect.h/.cpp (의존성 없음)
i_card_database.h   (card_data.h)

[Phase 2]
deck.cpp            (card_data.h)
combatant.cpp       (status_effect.h)
player_combatant.cpp (combatant.h, deck.h, card_data.h)
enemy_combatant.cpp  (combatant.h, enemy_data.h, enemy_move.h)
effect_handler.cpp   (card_data.h, combatant.h, i_card_database.h)
combat_manager.cpp   (player_combatant.h, enemy_combatant.h, effect_handler.h)
```

---

## 7. 참고 사항

### Godot 버전
- Godot **4.3+** 기준 (GDExtension API 안정화 버전)
- godot-cpp 브랜치: `godot-4.3` 이상

### 빌드 환경
- CMake 3.20+
- C++17 이상
- Windows: MSVC 2022 또는 MinGW-w64
- macOS: Clang (Xcode Command Line Tools)
- Linux: GCC 11+

### 주의사항
1. GDExtension 클래스는 Godot 에디터를 열기 전에 `.gdextension` 파일이 존재해야 한다.
2. `@export` 프로퍼티는 GDExtension에서 `ClassDB::bind_method` + `ADD_PROPERTY` 매크로로 선언해야 `.tres`에서 로드된다.
3. `CardData`를 GDExtension Resource로 만들면 기존 `.tres` 파일을 그대로 사용할 수 있다.
4. 개발 중에는 GDScript와 GDExtension을 병행 운영하여 단계별 검증이 가능하다.
