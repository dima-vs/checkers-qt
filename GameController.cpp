#include "GameController.h"

#include <QDebug>
#include <QThread>
GameController::GameController(
    Position::CheckerColor playerColor, int hardLevel, QObject *parent) :
    m_playerColor(playerColor), m_premovePtr(nullptr),
    m_previousMove(nullptr), QObject{parent}
{
    m_currentGame = new Game(playerColor, hardLevel);

    updateGameState();
    setWhoMovesNext();
    m_waitMoveAnimationToEnd = !m_isPlayerMoveNext;

    oppMoveObj = new OpponentMove(m_currentGame);

    oppMoveObj->moveToThread(&oppMoveThread);
    connect(this, &GameController::opponentMoveSignal,
            oppMoveObj, &OpponentMove::opponentMove);
    connect(oppMoveObj, &OpponentMove::opponentMoved,
            this, &GameController::onOpponentMoved);
    oppMoveThread.start();
    resetPremove();
}

void GameController::analizeClickedPoint(QPoint clickedPoint)
{
    Position currentPosition = m_currentGame->getCurrentPosition();
    Position::CellObjectType objectType =
        currentPosition.getCellObjectType(
            clickedPoint.y(),
            clickedPoint.x()
        );

    Position::CheckerColor checkerColor =
        Position::getCheckerColor(objectType);

    if (checkerColor == m_playerColor)
    {
        m_selectedCheckerCoords = QPoint(clickedPoint);
        m_isCheckerSelected = true;
        resetPremove();
    }
    else if (m_isCheckerSelected)
    {
        tryPlayerMove(clickedPoint);
    }
    else
    {
        resetPremove();
    }
}

void GameController::tryPlayerMove(QPoint moveToPoint)
{
    Move playerMove;
    MovesController::initMove(
        playerMove,
        m_selectedCheckerCoords,
        moveToPoint);

    if (!m_isPlayerMoveNext || m_waitMoveAnimationToEnd)
    {
        setPremove(playerMove);
        return;
    }
    tryPlayerMove(playerMove);
}

void GameController::tryPlayerMove(Move& move)
{
    if (m_isGameOver)
        return;

    qDebug() << "Info: enter GameController::tryPlayerMove()";
    MoveInfo moveInfo;
    MoveState moveState = m_currentGame->tryPlayerMove(move, moveInfo);
    if (moveState == MoveState::SUCCESS)
    {
        setWhoMovesNext();

        emit moveIsMade(
            MovesController::getMoveChain(move),
            QList<QPoint>(),
            MovesController::getObjTypeOfCheckerMoved(
                move, m_currentGame->getCurrentPosition()
            ),
            QList<Position::CellObjectType>(),
            moveInfo
            );
    }
    qDebug() << "Info: leave GameController::tryPlayerMove()";
}

void GameController::setPremove(const Move& playerMove)
{
    delete m_premovePtr;
    m_premovePtr = new Move;
    MovesController::initMove(*m_premovePtr);
    MovesController::copyMove(playerMove, *m_premovePtr);
    m_isPremoveDefined = true;
}

void GameController::resetPremove()
{
    if (!m_isPremoveDefined)
        return;

    delete m_premovePtr;
    m_premovePtr = nullptr;
    m_isPremoveDefined = false;
}

void GameController::tryToMove()
{
    if (m_isGameOver)
        return;

    if (!m_isPlayerMoveNext)
    {
        emit opponentMoveSignal();
        return;
    }

    if (m_isPremoveDefined)
        tryPlayerMove(*m_premovePtr);
}

void GameController::setWhoMovesNext()
{
    m_isPlayerMoveNext = m_currentGame->whoIsNext() == m_playerColor;
}

void GameController::onOpponentMoved(Move opponentMove, MoveInfo moveInfo)
{
    qDebug() << "Info: enter GameController::onOpponentMoved()";
    setWhoMovesNext();
    m_waitMoveAnimationToEnd = true;

    emit moveIsMade(MovesController::getMoveChain(opponentMove),
                    QList<QPoint>(),
                    MovesController::getObjTypeOfCheckerMoved(
                        opponentMove, m_currentGame->getCurrentPosition()
                    ),
                    QList<Position::CellObjectType>(),
                    moveInfo
                    );
    qDebug() << "Info: leaves GameController::onOpponentMoved()";
}

void GameController::onMoveAnimationEnded()
{
    qDebug() << "Info: enter GameController::onMoveAnimationEnded()";
    m_waitMoveAnimationToEnd = false;
    emit positionUpdated(m_currentGame->getCurrentPosition());

    updateGameState();
    sendEndGameSignalIfNeeded();

    tryToMove();
    qDebug() << "Info: leave GameController::onMoveAnimationEnded()";
}

void GameController::updateGameState()
{
    m_gameState = m_currentGame->getCurrentState();
    m_isGameOver = (m_gameState == Game::GameState::GAME_CONTINUES) ?
                       false : true;
}

void GameController::sendEndGameSignalIfNeeded()
{
    if (m_isGameOver)
        emit gameEnded(m_gameState);
}

GameController::~GameController()
{
    oppMoveThread.quit();
    oppMoveThread.wait();
}


