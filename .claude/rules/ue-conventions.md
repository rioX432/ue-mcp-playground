# Unreal Engine Conventions

Follow Epic's official standard. Source:
[Epic C++ Coding Standard for Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine).

## C++ Naming (class prefixes)

| Prefix | Meaning |
|---|---|
| `U` | Class inheriting from `UObject` |
| `A` | Class inheriting from `AActor` |
| `S` | Class inheriting from `SWidget` (Slate) |
| `I` | Abstract interface |
| `T` | Template class |
| `E` | Enum |
| `F` | Most other plain classes/structs |
| `b` | Boolean variables (`bHasFadedIn`, `bPendingDestruction`) |

- Type and variable names are **nouns**; method names are **verbs**.
- Macros: fully capitalized, underscore-separated, `UE_` prefixed.
- Template params: disambiguate with `In` prefix (`template <typename InElementType>`).
- New non-`UCLASS`/`USTRUCT` APIs go in a `UE::` namespace (nested where sensible,
  e.g. `UE::MCPPlayground::`); implementation details go in a `Private` sub-namespace.

## Asset Naming (content)

Use type prefixes for assets so paths are predictable for MCP lookups:

| Prefix | Asset type |
|---|---|
| `BP_` | Blueprint |
| `SM_` | Static Mesh |
| `M_` / `MI_` | Material / Material Instance |
| `T_` | Texture |
| `IA_` / `IMC_` | Input Action / Input Mapping Context (Enhanced Input) |
| `WBP_` | Widget Blueprint (UMG) |

## General

- Code comments and commit messages in **English**, concise.
- No magic numbers — define as `constexpr`/config.
- Prefer `UPROPERTY`/`UFUNCTION` exposure deliberately; only expose to Blueprint
  what designers need (`BlueprintCallable`, `BlueprintReadWrite`).

## Git & LFS (mandatory)

UE content (`.uasset`, `.umap`) is binary and large. `.gitattributes` routes these
through Git LFS. **Never commit `Binaries/`, `Intermediate/`, `Saved/`, or
`DerivedDataCache/`** (see `.gitignore`). Commit `Content/`, `Config/`, `Source/`,
and `*.uproject`.

## Testing

- Use UE Automation tests (`IMPLEMENT_SIMPLE_AUTOMATION_TEST`) or Functional Tests
  for gameplay. Run headless with `-nullrhi`.
- Pair every gameplay change with a test that proves the behavior (see
  `.claude/rules/mcp-workflow.md` Definition of Done).
- Test names describe behavior, not implementation.
