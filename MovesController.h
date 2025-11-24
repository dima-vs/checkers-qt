#ifndef MOVESCONTROLLER_H
#define MOVESCONTROLLER_H

#include <QPoint>
#include <QList>
#include "src/CheckersMoves.h"
#include "src/Position.h"

#include <QDebug>

class MovesController
{
public:
    using Move = CheckersMoves::MovesManager::Move;
    using Coordinates = CheckersMoves::MovesManager::Coordinates;

    MovesController();
    static void initMove(Move& move, const QPoint &startPoint, const QPoint &endPoint);
    static void initMove(Move& move);
    static void copyMove(const Move& copyFrom, Move& copyTo);
    static QList<QPoint> getMoveChain(const Move& move);
    static Position::CellObjectType getObjTypeOfCheckerMoved(
        const Move& move,
        const Position& currentPos
        );
    static QPoint coordsToQPoint(Coordinates coords);
};

#endif // MOVESCONTROLLER_H
