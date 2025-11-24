#include <cmath> // для abs
#include "CheckersMoves.h"

using namespace CheckersMoves;
using Move = MovesManager::Move;
using Coordinates = MovesManager::Coordinates;
using CheckerType = Position::CheckerType;
using CheckerColor = Position::CheckerColor;
using CellObjectType = Position::CellObjectType;

const MovesManager::Coordinates MovesValidator::coordinatesErrorValue(-1, -1);
MovesValidator::MoveState MovesValidator::validateSimpleMove(
    const Position& position,
    const MovesManager::Move& move
)
{
    if (!MovesValidator::isMoveValid(move.from, move.to[0]))
        return MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;

    CellObjectType currentObjectType = position.getCellObjectType(move.from.row, move.from.column);
    MovesList simpleMoves;
    MovesManager::getSimpleMovesForCurrent(
        move.from,
        currentObjectType,
        position,
        simpleMoves,
        checkerColorInBottom
    );
    // чи існує поточний хід в списку всіх доступних простих ходів
    bool isCurrentInSimpleMoves = simpleMoves.exists(move);
    // якщо хід присутній серед всіх доступних простих ходів
    // для поточної шашки
    return (isCurrentInSimpleMoves) ?
        MovesValidator::MoveState::SUCCESS :
        MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;
}

MovesValidator::MoveState MovesValidator::validateTakenMove(
    const Position& position,
    const MovesManager::Move& move,
    Coordinates currentCanTake
)
{
    if (!MovesValidator::isMoveValid(move.from, move.to[0]))
        return MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;

    CellObjectType currentObjectType = position.getCellObjectType(move.from.row, move.from.column);
    MovesList* takenMoves = new MovesList;
    MovesManager::getTakenMovesForCurrent(
        move.from,
        position,
        *takenMoves,
        checkerColorInBottom
    );

    // чи існує поточний хід в ланцюгу всіх доступних ходів із взяттям для поточної шашки
    bool isCurrentInTakenMoves = takenMoves->exists(move);
    delete takenMoves;
    takenMoves = nullptr;
    // якщо хід присутній серед всіх доступних ходів для поточної шашки
    // АЛЕ попередня шашка не добила до кінця
    if (isCurrentInTakenMoves && currentCanTake != MovesValidator::coordinatesErrorValue &&
        move.from != currentCanTake
    )
    {
        return MovesValidator::MoveState::MUST_TAKE_TO_END_ERROR;
    }
    else if (isCurrentInTakenMoves)
        return MovesValidator::MoveState::SUCCESS;

    // перевіримо чи це звичайний хід
    MovesList simpleMoves;
    MovesManager::getSimpleMovesForCurrent(
        move.from,
        currentObjectType,
        position,
        simpleMoves,
        checkerColorInBottom
    );
    
    // створюємо копію ходу, але з countMoves=0
    MovesManager::Move simpleMoveCopy;
    simpleMoveCopy.countMoves = 0;
    simpleMoveCopy.from = move.from;
    simpleMoveCopy.to = new MovesManager::Coordinates[1];
    simpleMoveCopy.to[0] = move.to[0];
    // чи існує поточний хід в списку всіх доступних простих ходів
    bool isCurrentInSimpleMoves = simpleMoves.exists(simpleMoveCopy);
    // якщо цей хід звичайний, але гравець може побити
    // повертаємо помилку MUST_TAKE_ERROR
    if (validateSimpleMove(position, simpleMoveCopy) == MovesValidator::MoveState::SUCCESS)
        return MovesValidator::MoveState::MUST_TAKE_ERROR;

    // все інше - це неможливий хід
    return MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;
}

bool MovesValidator::isMoveValid(Coordinates start, Coordinates end)
{
    return !MovesManager::isOutOfRange(start) &&
        !MovesManager::isOutOfRange(end) &&
        MovesValidator::isDiagonalMove(start, end);
}

bool MovesValidator::isDiagonalMove(Coordinates start, Coordinates end)
{
    int rowDifference = std::abs(start.row - end.row);
    int columnDifference = std::abs(start.column - end.column);

    return (rowDifference == columnDifference);
}

bool MovesValidator::hasTakenMoves(
    const Position& position,
    Position::CheckerColor checkerThatMovesColor
)
{
    for (int i = 0; i < 8; ++i)
    {
        // якщо i - непарне значення, то j починається з 0
        for (int j = ((i % 2) ? 0 : 1); j < 8; j += 2)
        {
            Coordinates currentCoords(i, j);
            CellObjectType currentObjectType = position.getCellObjectType(i, j);
            if (Position::getCheckerColor(currentObjectType) != checkerThatMovesColor)
                continue;
            
            bool canTake = MovesValidator::hasTakenMovesForCurrent(
                position,
                currentCoords,
                Position::getCheckerType(currentObjectType)
            );
        
            if (canTake)
                return true;
        }
    }
    return false;
}

bool MovesValidator::hasTakenMovesForCurrent(
    const Position& position,
    Coordinates currentCoords,
    CheckerType currentCheckerType
)
{
    if (currentCheckerType == CheckerType::UNDEFINED_TYPE)
    {
        CellObjectType currentObjectType = position.getCellObjectType(currentCoords.row, currentCoords.column);
        currentCheckerType = Position::getCheckerType(currentObjectType);
    }

    bool hasTakenMove = false;
    for (int i = 0; i < MovesManager::DIRECTIONS_SIZE; ++i)
    {
        if (currentCheckerType == CheckerType::SIMPLE)
        {
            Coordinates unusedOut;
            hasTakenMove = MovesManager::getTakenMoveCoordsForSimple(
                currentCoords,
                position,
                MovesManager::DIRECTIONS[i],
                unusedOut
            );
        }
        else if (currentCheckerType == CheckerType::QUEEN)
        {
            Coordinates* unusedOut = nullptr;
            hasTakenMove = MovesManager::getTakenMoveCoordsForQueen(
                currentCoords,
                position,
                MovesManager::DIRECTIONS[i],
                unusedOut
            );
            delete[] unusedOut;
        }
        if (hasTakenMove)
            return true;
    }
    return false;
}