#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QThread>
#include <QList>
#include "MovesController.h"
#include "OpponentMove.h"
#include "src/Game.h"
#include "src/Position.h"
#include "src/CheckersMoves.h"

class GameController : public QObject
{
    Q_OBJECT
public:
    using Move = CheckersMoves::MovesManager::Move;
    using MoveInfo = CheckersMoves::MovesManager::MoveInfo;
    using Coordinates = CheckersMoves::MovesManager::Coordinates;
    using MoveState = CheckersMoves::MovesValidator::MoveState;

    GameController(Position::CheckerColor playerColor,
                   int hardLevel,
                   QObject *parent = nullptr);
    ~GameController();
    Position getCurrentPosition()
    {
        return m_currentGame->getCurrentPosition();
    }
private:
    QThread oppMoveThread;
    OpponentMove *oppMoveObj;
    Game *m_currentGame;
    QPoint m_selectedCheckerCoords;
    bool m_isCheckerSelected;
    const Position::CheckerColor m_playerColor;
    Move *m_premovePtr;
    Move *m_previousMove;
    Game::GameState m_gameState;
    bool m_isGameOver;
    bool m_isPremoveDefined;
    bool m_isPlayerMoveNext;

    bool m_waitMoveAnimationToEnd;

    void updateGameState();
    void sendEndGameSignalIfNeeded();
    void setWhoMovesNext();
    void setPremove(const Move &playerMove);
    void resetPremove();
    void tryPlayerMove(QPoint moveToPoint);
    void tryPlayerMove(Move &move);
signals:
    void gameEnded(Game::GameState gameState);
    void opponentMoveSignal();
    void positionUpdated(Position newPosition);
    // void moveIsMade(QList<QPoint> movesChain,
    //                 QList<QPoint> takenCheckers,
    //                 Position::CellObjectType animatedCheckerType,
    //                 QList<Position::CellObjectType> m_takenCheckersType);
    void moveIsMade(QList<QPoint> movesChain,
                    QList<QPoint> takenCheckers,
                    Position::CellObjectType animatedCheckerType,
                    QList<Position::CellObjectType> m_takenCheckersType,
                    MoveInfo moveInfo
                    );
public slots:
    void tryToMove();
    void onMoveAnimationEnded();
    void onOpponentMoved(Move opponentMove, MoveInfo moveInfo);
    // void opponentMove();
    void analizeClickedPoint(QPoint clickedPoint);
};

#endif // GAMECONTROLLER_H
