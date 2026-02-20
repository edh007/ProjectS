#pragma once

/// CardData.effect_params Dictionary를 대체하는 타입 안전 구조체
struct CardEffectParams {
    int  damage           = 0;
    int  block            = 0;
    int  hits             = 1;
    int  draw             = 0;
    int  strength         = 0;
    int  apply_vulnerable = 0;  // 취약 부여 스택
    int  apply_weak       = 0;  // 약화 부여 스택
    int  apply_poison     = 0;  // 독 부여 스택
    bool exhaust          = false;
};

/// EnemyMove.move_params Dictionary를 대체하는 타입 안전 구조체
struct EnemyMoveParams {
    int damage        = 0;
    int block         = 0;
    int hits          = 1;
    int buff_strength = 0;
    int apply_poison  = 0;
};
