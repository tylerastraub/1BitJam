#ifndef PAINT_ATTACK_COMPONENT_H
#define PAINT_ATTACK_COMPONENT_H

struct PaintAttackComponent {
    int msSinceLastPaintAttack = 500;
    int msCantActAfterPaintAttack = 300;
    bool attacking = false;
};

#endif