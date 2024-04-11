
#ifndef __INSECTATTACK_H__
#define __INSECTATTACK_H__

const int INSECTATTACK_ANTS = 0;
const int INSECTATTACK_COCKROACHES = 1;

void InitialiseInsectAttack();

void CleanupInsectAttack();

void StartInsectAttack( int insectAttack );

void EndInsectAttack();

void DrawInsectAttack();

void UpdateInsectAttack( float deltaTime );

void ApplyShake();

bool IsInsectAttackActive();

#endif // __INSECTATTACK_H__
