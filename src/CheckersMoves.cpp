#include "CheckersMoves.h"

using namespace CheckersMoves;
using Move = MovesManager::Move;
using Coordinates = MovesManager::Coordinates;
using CheckerType = Position::CheckerType;
using CheckerColor = Position::CheckerColor;
using CellObjectType = Position::CellObjectType;

const int MovesManager::DIRECTIONS_SIZE = 4;
const MovesManager::Direction MovesManager::DIRECTIONS[] = {
    Direction::LEFT_TOP,
    Direction::RIGHT_TOP,
    Direction::LEFT_BOTTOM,
    Direction::RIGHT_BOTTOM
};

bool MovesManager::getMoves(
    const Position& position,
    CheckerColor checkerThatMovesColor,
    MovesList& movesList,
    CheckerColor checkerColorInBottom,
    bool searchTakenMoves
    )
{
    // кількість шашок того кольору, для яких шукаються ходи
    int checkersCount = (checkerThatMovesColor == Position::CheckerColor::BLACK) ?
                            position.getBlackCheckersCount() : position.getWhiteCheckersCount();

    for (int i = 0; i < 8; ++i)
    {
        // якщо i - непарне значення, то j починається з 0
        for (int j = ((i % 2) ? 0 : 1); j < 8; j += 2)
        {
            // якщо всі шашки шуканого кольору були перевірені - завершуємо метод
            if (checkersCount == 0)
                return !(movesList.isEmpty());

            CellObjectType currentObjectType = position.getCellObjectType(i, j);
            // якщо це пуста клітина
            if (Position::isEmptyCell(currentObjectType))
                continue;
            // якщо кольори НЕ співпали
            if (Position::getCheckerColor(currentObjectType) != checkerThatMovesColor)
                continue;

            --checkersCount;
            Position copy = position;
            if (searchTakenMoves)
            {
                MovesManager::getTakenMovesForCurrent(
                    Coordinates(i, j),
                    copy,
                    movesList,
                    checkerColorInBottom
                    );
            }
            else
            {
                MovesManager::getSimpleMovesForCurrent(
                    Coordinates(i, j),
                    currentObjectType,
                    copy,
                    movesList,
                    checkerColorInBottom
                    );
            }
        }
    }

    return !(movesList.isEmpty());
}

int MovesManager::countSpace(Coordinates coords, Direction direction, const Position& position)
{
    int counter = 0;
    while (true)
    {
        MovesManager::nextCoordinates(coords, direction);
        if (MovesManager::isOutOfRange(coords))
            break;

        if (!Position::isEmptyCell(position.getCellObjectType(coords.row, coords.column)))
            break;

        ++counter;
    }
    return counter;
}

MovesManager::SearchedData MovesManager::getNextCellObjectType(Coordinates coords, MovesManager::Direction direction, const Position& position)
{
    MovesManager::SearchedData searchedData;
    searchedData.isSearched = false;

    // coords передається за неконстантним посиланням
    MovesManager::nextCoordinates(coords, direction);
    if (MovesManager::isOutOfRange(coords))
        return searchedData;

    searchedData.isSearched = true;

    searchedData.coords = coords;
    searchedData.cellObjectType = position.getCellObjectType(coords.row, coords.column);
    return searchedData;
}

void MovesManager::setMove(Move& moveOut, int countMoves, Coordinates from, Coordinates* to)
{
    if (countMoves == 0)
        MovesManager::setSimpleMove(moveOut, from, to[0]);
    else
        MovesManager::setTakenMove(moveOut, countMoves, from, to);
}

void MovesManager::setTakenMove(Move& moveOut, int countMoves, Coordinates from, Structures::Stack<Coordinates>* to)
{
    moveOut.countMoves = countMoves;
    moveOut.from = from;
    moveOut.to = new Coordinates[countMoves];

    if (countMoves == 0)
        return;

    to->start();
    while (to->next())
    {
        moveOut.to[to->getCurrentIndex()] = to->getCurrent();
    }
}

void MovesManager::setTakenMove(Move& moveOut, int countMoves, Coordinates from, Coordinates* to)
{
    moveOut.countMoves = countMoves;
    moveOut.from = from;
    moveOut.to = new Coordinates[countMoves];

    if (countMoves == 0)
        return;

    for (int i = 0; i < countMoves; ++i)
        moveOut.to[i] = to[i];
}

void MovesManager::setSimpleMove(Move& moveOut, Coordinates from, Coordinates to)
{
    moveOut.countMoves = 0;
    moveOut.from = from;
    moveOut.to = new Coordinates(to);
}

MovesManager::SearchedData MovesManager::searchFirstChecker(
    Coordinates coords,
    MovesManager::Direction direction,
    const Position& position
    )
{
    MovesManager::SearchedData searchedData;
    searchedData.isSearched = false;

    while (true)
    {
        // coords передається за неконстантним посиланням
        MovesManager::nextCoordinates(coords, direction);
        if (MovesManager::isOutOfRange(coords))
            break;

        Position::CellObjectType cellObjectType = position.getCellObjectType(coords.row, coords.column);
        // якщо наступна клітина пуста - переходимо на наступну ітерацію
        if (Position::isEmptyCell(cellObjectType))
            continue;
        else
        {
            searchedData.coords = coords;
            searchedData.cellObjectType = cellObjectType;
            searchedData.isSearched = true;
            break;
        }
    }

    return searchedData;
}

void MovesManager::nextCoordinates(Coordinates& coords, MovesManager::Direction direction)
{
    switch (direction)
    {
    case MovesManager::Direction::LEFT_TOP:
        --coords.row;
        --coords.column;
        break;
    case MovesManager::Direction::RIGHT_TOP:
        --coords.row;
        ++coords.column;
        break;
    case MovesManager::Direction::LEFT_BOTTOM:
        ++coords.row;
        --coords.column;
        break;
    case MovesManager::Direction::RIGHT_BOTTOM:
        ++coords.row;
        ++coords.column;
        break;
    }
}

MovesManager::Direction MovesManager::getDirection(Coordinates start, Coordinates end)
{
    if ((start.row > end.row) && (start.column > end.column))
    {
        return MovesManager::Direction::LEFT_TOP;
    }
    else if ((start.row > end.row) && (start.column < end.column))
    {
        return MovesManager::Direction::RIGHT_TOP;
    }
    else if ((start.row < end.row) && (start.column > end.column))
    {
        return MovesManager::Direction::LEFT_BOTTOM;
    }
    else if ((start.row < end.row) && (start.column < end.column))
    {
        return MovesManager::Direction::RIGHT_BOTTOM;
    }
}

bool MovesManager::isOutOfRange(Coordinates coords)
{
    return !(
        (coords.column < 8) &&
        (coords.column >= 0) &&
        (coords.row < 8) &&
        (coords.row >= 0)
        );
}

Position::CellObjectType MovesManager::becomeQueen(Position::CellObjectType currentObjectType)
{
    CheckerColor currentColor = Position::getCheckerColor(currentObjectType);
    if (currentColor == Position::CheckerColor::BLACK)
        return Position::CellObjectType::BLACK_QUEEN;
    else if (currentColor == Position::CheckerColor::WHITE)
        return Position::CellObjectType::WHITE_QUEEN;

    throw "currentObjectType must be a checker";
}


void MovesManager::result(
    Position& position,
    const Move& move,
    CheckerColor checkerColorInBottom,
    bool enableTempValues
    )
{
    MoveInfo unused;
    result(position, move, checkerColorInBottom, unused, enableTempValues);
}


void MovesManager::result(
    Position& position,
    const Move& move,
    Position::CheckerColor checkerColorInBottom,
    MoveInfo& outMoveInfo,
    bool enableTempValues
    )
{
    CellObjectType currentObjectType = position.getCellObjectType(move.from.row, move.from.column);
    outMoveInfo.beginCheckerType = currentObjectType;
    outMoveInfo.wasTransformed = false;

    // останній рядок для поточної шашки, де вона може стати дамкою
    int lastRow = (checkerColorInBottom == Position::getCheckerColor(currentObjectType)) ?
                      0 : 7;
    // якщо це звичайний хід
    if (move.countMoves == 0)
    {
        position.setCellObjectType(CellObjectType::EMPTY_CELL, move.from.row, move.from.column);
        // якщо шашка стала на останній рядок, то вона стане дамкою
        currentObjectType = (move.to[0].row == lastRow) ?
                                MovesManager::becomeQueen(currentObjectType) : currentObjectType;
        position.setCellObjectType(currentObjectType, move.to[0].row, move.to[0].column);
    }
    // якщо це хід із взяттям фігури
    else
    {
        Coordinates from = move.from;
        CheckerColor currentColor = Position::getCheckerColor(position.getCellObjectType(from.row, from.column));
        for (int i = 0; i < move.countMoves; ++i)
        {
            MovesManager::Direction direction = MovesManager::getDirection(from, move.to[i]);
            MovesManager::SearchedData opponentChecker = MovesManager::searchFirstChecker(
                from,
                direction,
                position
                );
            // замінюємо клітину з поточною шашкою на пусту клітину
            // тому, що вона перемістилась
            position.setCellObjectType(CellObjectType::EMPTY_CELL, from.row, from.column);
            if (enableTempValues)
            {
                // замінюємо клітину з опонентом на тимчасовий маркер
                position.setCellObjectType(
                    CellObjectType::TEMP_VALUE,
                    opponentChecker.coords.row,
                    opponentChecker.coords.column
                    );
            }
            else
            {
                // замінюємо клітину з опонентом на пусту клітину
                position.setCellObjectType(
                    CellObjectType::EMPTY_CELL,
                    opponentChecker.coords.row,
                    opponentChecker.coords.column
                    );
            }
            // якщо шашка стала на останній рядок, то вона стане дамкою
            if (move.to[i].row == lastRow && !outMoveInfo.wasTransformed)
            {
                currentObjectType = MovesManager::becomeQueen(currentObjectType);
                outMoveInfo.finalCheckerType = currentObjectType;
                outMoveInfo.pointOfTransformation = move.to[i];
                outMoveInfo.wasTransformed = true;
            }
            // currentObjectType = (move.to[i].row == lastRow) ?
            //                         MovesManager::becomeQueen(currentObjectType) :
            //                         currentObjectType;

            // нова позиція поточної шашки після взяття опонента
            position.setCellObjectType(currentObjectType, move.to[i].row, move.to[i].column);

            // зменшуємо кількість шашок опонента
            if (currentColor == Position::CheckerColor::BLACK)
                position.decreaseWhiteCheckersCount();
            else
                position.decreaseBlackCheckersCount();

            from = move.to[i];
        }
    }
}

// MovesList
bool MovesList::exists(const Move& move)
{
    if (moves.isEmpty())
        return false;
    // ініціалізуємо ітератор
    moves.start();
    while (moves.next())
    {
        if (MovesList::compareMoves(move, *moves.getCurrent()))
            return true;
    }
    return false;
}

bool MovesList::compareMoves(const Move& userMove, const Move& move)
{
    if (userMove.from != move.from)
        return MovesList::complexCompareMoves(userMove, move);

    if (userMove.countMoves > move.countMoves)
        return false;

    if (userMove.countMoves == 0)
        return userMove.to[0] == move.to[0];

    for (int i = 0; i < (int)userMove.countMoves; ++i)
    {
        if (userMove.to[i] != move.to[i])
            return false;
    }
    return true;
}

bool MovesList::complexCompareMoves(const Move& userMove, const Move& move)
{
    int userCountMoves = userMove.countMoves;
    int countMoves = move.countMoves;

    int index = 0;
    Coordinates from;
    while (index < countMoves)
    {
        Coordinates userCoordsFrom = move.to[index];
        if (userCoordsFrom != userMove.from)
            continue;

        from = userCoordsFrom;
        for (int i = 0; i < userCountMoves; ++i)
        {
            ++index;
            if (index < countMoves)
                return false;
            if (userMove.to[i] != move.to[index])
                return false;
        }
        return true;
    }
}
