# Architecture Decision Records: Full Multi-Region Support

This document tracks key architectural and strategic decisions made during the development of the multi-region system for Pokemon Tuxedo.

---

## ADR-001: MVP Scope - Ship Kanto + Hoenn First

**Date:** March 16, 2026  
**Status:** Accepted  
**Deciders:** User, Kiro

### Context

The original plan called for porting Johto, Hoenn, and Sinnoh regions from Pokemon Crossroads. However, we discovered:

1. **Johto** is not available in Pokemon Crossroads Beta 1.0 (scheduled for Beta 2.0, unknown release date)
2. **Sinnoh** is not in Crossroads and no GBA-compatible source exists (Sinnoh was originally a DS game)
3. **Hoenn** has been fully converted (104 maps) but the cross-region infrastructure doesn't exist yet

### Decision

Ship a 2-region MVP (Kanto + Hoenn) first, then add Johto later.

### Rationale

- Hoenn content is ready but unusable without the Region Management system
- The cross-region infrastructure (Phase 5) is the core feature that enables multi-region gameplay
- Waiting for Crossroads Beta 2.0 has unknown timeline risk
- A working 2-region game provides immediate value and validates the architecture

### Consequences

- Phase 5 (Region Management Integration) becomes the immediate priority
- Phase 3 (Johto) and Phase 4 (Sinnoh) are deferred
- MVP delivers Kanto → Hoenn progression with full region transition system
- Johto can be added incrementally after MVP ships

---

## ADR-002: Use Pokemon CrystalDust as Johto Source

**Date:** March 16, 2026  
**Status:** Accepted  
**Deciders:** User, Kiro

### Context

With Johto unavailable in Pokemon Crossroads, we evaluated alternative sources:

| Source | Engine | Compatibility | Availability |
|--------|--------|---------------|--------------|
| Pokemon Crossroads Beta 2.0 | pokeemerald | Perfect | Unknown timeline |
| Pokemon CrystalDust | pokeemerald | High | Available now |
| Pokemon Crystal Shards | pokeemerald | High | Available now |
| Manual map creation | N/A | Perfect | Very high effort |

### Decision

Use Pokemon CrystalDust (https://github.com/Sierraffinity/CrystalDust) as the source for Johto maps.

### Rationale

- CrystalDust is based on pokeemerald, same engine family as our converter targets
- Complete Johto region with all routes, cities, caves, and dungeons
- Actively maintained project with recent releases
- Our existing format converter should work with minimal modifications
- Same map.json format, tileset structure, and script system as Crossroads

### Consequences

- Need to clone CrystalDust repository and analyze its structure
- May need minor converter adjustments for naming conventions
- Script flags/variables may need remapping
- Johto integration becomes a data task, not an architecture task

### Risks

- CrystalDust may have different event implementations than original Crystal
- Tileset palettes may need adjustment for FireRed compatibility
- Script complexity for major Johto events (Radio Tower, Lake of Rage) needs review

---

## ADR-003: Descope Sinnoh from Initial Release

**Date:** March 16, 2026  
**Status:** Accepted  
**Deciders:** User, Kiro

### Context

Sinnoh (Pokemon Diamond/Pearl/Platinum) was originally a Nintendo DS game with:
- Dual-screen gameplay
- Different tile system than GBA
- 3D graphical elements
- No existing GBA decomp or port

No GBA-compatible Sinnoh source exists in the ROM hacking community.

### Decision

Remove Sinnoh from the initial release scope. Consider it a future expansion.

### Rationale

- Creating Sinnoh maps manually would require months of mapping work
- DS → GBA conversion is not a simple format change; it's a recreation
- The 3-region game (Kanto + Hoenn + Johto) still provides substantial content
- Resources are better spent on shipping a polished 3-region experience

### Consequences

- Requirements 4.1-4.9 (Sinnoh Region Implementation) are descoped
- Phase 4 tasks are removed from the implementation plan
- Region unlock sequence changes: Kanto → Hoenn → Johto (was Kanto → Johto → Hoenn → Sinnoh)
- Badge tracking reduced from 32 to 24 badges
- Save data structures simplified (3 regions instead of 4)

### Future Considerations

- Monitor community for GBA Sinnoh projects
- Could be added as post-release DLC/expansion
- Manual creation remains an option if there's demand

---

## ADR-004: Reorder Region Unlock Sequence

**Date:** March 16, 2026  
**Status:** Accepted  
**Deciders:** User, Kiro

### Context

Original unlock sequence: Kanto → Johto → Hoenn → Sinnoh

With Sinnoh descoped and Hoenn already converted, we need to decide the new sequence.

### Decision

New unlock sequence: Kanto → Hoenn → Johto

### Rationale

- Hoenn is fully converted and ready for integration
- Johto requires additional work (CrystalDust integration)
- Players can experience multi-region gameplay sooner with Hoenn
- Allows MVP release while Johto integration continues

### Consequences

- Requirement 1.1-1.3 updated: Kanto E4 → Hoenn, Hoenn E4 → Johto
- Level scaling formulas adjusted for new progression
- Region tickets: HOENN_TICKET awarded after Kanto E4, JOHTO_TICKET after Hoenn E4

---

## ADR-005: Prioritize Cross-Region Infrastructure Over Content

**Date:** March 16, 2026  
**Status:** Accepted  
**Deciders:** User, Kiro

### Context

We have two paths forward:
1. Continue converting more region content (blocked on source data)
2. Build the cross-region infrastructure to enable existing content

### Decision

Prioritize Phase 5 (Region Management Integration) immediately.

### Rationale

- 104 Hoenn maps are converted but unusable without region transition system
- Infrastructure work is not blocked by external dependencies
- Creates a testable, playable game with existing content
- Validates architecture before adding more regions
- Unblocks parallel work on Johto integration

### Consequences

- Phase 5 becomes the immediate next phase
- Phase 3 (Johto) moves after Phase 5
- Testing can begin with 2-region game
- Architecture issues discovered early

---

## Decision Log

| ID | Decision | Date | Status |
|----|----------|------|--------|
| ADR-001 | MVP Scope - Ship Kanto + Hoenn First | 2026-03-16 | Accepted |
| ADR-002 | Use Pokemon CrystalDust as Johto Source | 2026-03-16 | Accepted |
| ADR-003 | Descope Sinnoh from Initial Release | 2026-03-16 | Accepted |
| ADR-004 | Reorder Region Unlock Sequence | 2026-03-16 | Accepted |
| ADR-005 | Prioritize Cross-Region Infrastructure Over Content | 2026-03-16 | Accepted |
