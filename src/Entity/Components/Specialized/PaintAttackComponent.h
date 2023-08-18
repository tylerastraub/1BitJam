#ifndef PAINT_ATTACK_COMPONENT_H
#define PAINT_ATTACK_COMPONENT_H

struct PaintAttackComponent {
    int msSinceLastPaintAttack = 400;
    int msCantActAfterPaintAttack = 300;
    bool attacking = false;
    bool wasAttackingLastTick = false;
};

#endif