#ifndef OPPONENTMOVE_H
#define OPPONENTMOVE_H

#include <QObject>
#include "src/Game.h"
#include "src/CheckersMoves.h"

class OpponentMove : public QObject
{
    Q_OBJECT
public:
    using Move = CheckersMoves::MovesManager::Move;
    using MoveInfo = CheckersMoves::MovesManager::MoveInfo;

    explicit OpponentMove(Game *currentGame, QObject *parent = nullptr);

signals:
    void opponentMoved(Move, MoveInfo);
public slots:
    void opponentMove();
private:
    Game *m_currentGame;
};

#endif // OPPONENTMOVE_H
