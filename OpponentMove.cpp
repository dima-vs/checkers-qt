#include "OpponentMove.h"

OpponentMove::OpponentMove(Game *currentGame, QObject *parent)
    : m_currentGame(currentGame), QObject{parent}
{}

void OpponentMove::opponentMove()
{
    Move opponentMove;
    MoveInfo moveInfo;
    m_currentGame->opponentMove(opponentMove, moveInfo);
    emit opponentMoved(opponentMove, moveInfo);
}
