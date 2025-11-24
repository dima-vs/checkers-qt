#include "MovesController.h"

MovesController::MovesController() {}

void MovesController::initMove(Move& move)
{
    move.countMoves = 1;
    move.to = new Coordinates[1];
}

void MovesController::initMove(
    Move& move,
    const QPoint& startPoint,
    const QPoint& endPoint)
{
    move.from = Coordinates(
        startPoint.y(),
        startPoint.x()
        );
    MovesController::initMove(move);
    move.to[0] = Coordinates(endPoint.y(), endPoint.x());
}

void MovesController::copyMove(const Move &copyFrom, Move& copyTo)
{
    copyTo.from = copyFrom.from;
    copyTo.to[0] = copyFrom.to[0];
}

QList<QPoint> MovesController::getMoveChain(const Move& move)
{
    qDebug() << "Info: enter MovesController::getMoveChain()";
    QList<QPoint> result;
    result.append(QPoint(move.from.column, move.from.row));
    int size = move.countMoves ? move.countMoves : 1;
    for (int i = 0; i < size; ++i)
        result.append(QPoint(move.to[i].column, move.to[i].row));

    qDebug() << "Info: leave MovesController::getMoveChain()";
    return result;
}

Position::CellObjectType MovesController::getObjTypeOfCheckerMoved(
    const Move& move,
    const Position& currentPos
    )
{
    qDebug() << "Info: enter MovesController::getObjTypeOfCheckerMoved()";
    int size = move.countMoves ? move.countMoves : 1;
    Coordinates coords = move.to[size - 1];
    qDebug() << "Info: leaves MovesController::getObjTypeOfCheckerMoved()";
    return currentPos.getCellObjectType(coords.row, coords.column);
}

QPoint MovesController::coordsToQPoint(Coordinates coords)
{
    return QPoint(coords.column, coords.row);
}
