#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QList>
#include <QTimer>
#include "src/Position.h"
#include "src/Game.h"
#include "MovesController.h"
#include "EndGameOverlay.h"

class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    using MoveInfo = CheckersMoves::MovesManager::MoveInfo;

    explicit BoardWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

    void showEndOverlay();
    // void showEndGamePanel(Game::GameState gameState);
signals:
    void boardResized(int xOffset, int yOffset, int boardSide);
    void cellSelected(QPoint coords);
    void moveAnimationEnded();
public slots:
    void updatePosition(Position newPosition);
    void animateMove(
        QList<QPoint> movesChain,
        QList<QPoint> takenCheckers,
        Position::CellObjectType animatedCheckerType,
        QList<Position::CellObjectType> m_takenCheckersType,
        MoveInfo moveInfo
        );
    void onGameOverSignal(Game::GameState gameState);
private slots:
    void onAnimTimerTimeOut();
    void startAnimTimer();
    void emitMoveAnimationEnded();
private:
    EndGameOverlay *m_endGameOverlay = nullptr;

    Position m_currentPosition;

    // ANIMATION
    QList<QPoint> m_lastMoveCells;
    QTimer *m_animTimer;
    bool m_isAnimContinues;
    const int m_animDuration; // milliseconds
    const int m_animInterval;
    int m_timeEllapsed;
    int m_animationsCount;
    int m_animationIndex;
    QList<QPoint> m_movesChain;
    QList<QPoint> m_takenCheckers;
    Position::CellObjectType m_animatedCheckerType;
    MoveInfo m_moveInfo;
    QList<Position::CellObjectType> m_takenCheckersType;
    // ANIMATION

    const QPixmap m_blackChecker;
    const QPixmap m_whiteChecker;
    const QPixmap m_blackKing;
    const QPixmap m_whiteKing;

    QPixmap m_blackCheckerResized;
    QPixmap m_whiteCheckerResized;
    QPixmap m_blackKingResized;
    QPixmap m_whiteKingResized;
    int m_pieceOffset;

    QPoint m_pressedCoords;
    bool m_isGameOver;
    bool m_cellSelected;
    bool m_mousePressed;
    int m_coordinatesSpace;
    int m_cellSize;
    int m_boardSide;
    int m_xOffset;
    int m_yOffset;
    bool m_highlightedCellExist;
    QPoint m_highlightedCell;

    void highlightLastMove(QPainter *painter, QColor color);
    void drawEmptyBoard(
        QPainter *painter,
        QColor boardBackground,
        QColor coordLettersColor,
        QColor lightColor,
        QColor darkColor,
        QColor innerBorderColor
        );
    void drawCoordinates(QPainter *painter, QColor lettersColor);
    void drawBoardFramework(
        QPainter *painter,
        QColor boardBackground,
        QColor innerBorderColor
        );
    void drawPosition(QPainter *painter);
    void drawChecker(
        QPainter *painter,
        Position::CellObjectType checkerType,
        double row,
        double column);
    void drawAnimatedChecker(QPainter *painter);
    void highlightCells(
        QPainter* painter,
        QColor lastMoveMarker,
        QColor highlightedCellColor,
        QColor selectedCellColor
        );
    void drawBoardCells(QPainter *painter, QColor light, QColor dark);
    void drawCell(QPoint point, QPainter *painter, QColor color);
    bool isCurrentCheckerAnimating(QPoint point);
    void endOrContinueAnimation();
};

#endif // BOARDWIDGET_H
