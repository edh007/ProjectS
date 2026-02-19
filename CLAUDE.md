# ProjectS — Claude Base Rules

## 1. 세션 대화 로그 백업

- 매 세션이 끝날 때 반드시 `logs/sessions/YYYY-MM-DD-NNN.md` 파일로 대화 내용을 저장한다.
  - `NNN` = 같은 날짜 내 세션 순번 (001, 002, …)
- 로그 파일 포맷은 아래 템플릿을 따른다.

```markdown
# Session Log — YYYY-MM-DD-NNN

## 날짜
YYYY-MM-DD

## 요약
(한두 문장으로 이 세션에서 한 작업 요약)

## 대화 내용
### 사용자
…
### Claude
…

## 변경된 파일
- path/to/file.ext — 변경 내용 한 줄 설명

## 관련 브랜치 / 커밋
- branch: `브랜치명`
- commit: `커밋 해시 (생성 시 기재)`
```

---

## 2. Git 브랜치 & 커밋 규칙

소스 변경이 발생할 때마다 아래 절차를 반드시 따른다.

### 2-1. 브랜치 생성

| 작업 유형 | 브랜치 명명 규칙 | 예시 |
|-----------|----------------|------|
| 새 기능 추가 | `feat/짧은-설명` | `feat/card-hand-layout` |
| 버그 수정 | `fix/짧은-설명` | `fix/deck-shuffle-crash` |
| 리팩터링 | `refactor/짧은-설명` | `refactor/game-manager` |
| 문서/설정 | `chore/짧은-설명` | `chore/project-setup` |
| 세션 단위 작업 | `session/YYYY-MM-DD` | `session/2026-02-20` |

- 브랜치는 항상 최신 `main` 으로부터 분기한다.
- 브랜치명은 영문 소문자와 하이픈(`-`)만 사용한다.

### 2-2. 커밋 메시지 규칙

```
<type>: <요약 (한글 또는 영문, 50자 이내)>

<본문 — 필요 시, 변경 이유와 내용을 간략히>
```

`type` 목록: `feat` | `fix` | `refactor` | `docs` | `chore` | `test`

### 2-3. 절차 요약

```
1. git checkout main && git pull          # 최신 main 기준
2. git checkout -b <브랜치명>              # 새 브랜치 생성
3. (코드 작업)
4. git add <변경 파일들>                   # 변경 파일만 명시적으로 스테이징
5. git commit -m "<type>: <요약>"
6. (세션 로그 파일도 커밋에 포함)
```

> 브랜치를 원격에 push하거나 PR을 생성하는 것은 사용자의 명시적 요청이 있을 때만 수행한다.

---

## 3. 기타 규칙

- 한 세션에서 여러 기능을 작업한 경우 기능 단위로 커밋을 분리한다.
- `.env`, 인증 정보, 시크릿 키는 절대 커밋하지 않는다.
- 커밋 전 항상 `git status`와 `git diff`로 스테이징 내용을 확인한다.
