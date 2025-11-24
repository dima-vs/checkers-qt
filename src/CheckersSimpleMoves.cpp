#include "CheckersMoves.h"

using namespace CheckersMoves;
using Move = MovesManager::Move;
using Coordinates = MovesManager::Coordinates;
using CheckerType = Position::CheckerType;
using CheckerColor = Position::CheckerColor;
using CellObjectType = Position::CellObjectType;

void MovesManager::getSimpleMovesForCurrent(
    Coordinates coords,
    CellObjectType currentObjectType,
    const Position& position,
    MovesList& movesList,
    CheckerColor checkerColorInBottom
)
{
    CheckerType currentType = Position::getCheckerType(currentObjectType);
    CheckerColor currentColor = Position::getCheckerColor(currentObjectType);
    if (currentType == CheckerType::SIMPLE)
    {
        /* стартовий індекс для ітерації по масиву
            MovesManager::DIRECTIONS[] = {
                Direction::LEFT_TOP,
                Direction::RIGHT_TOP,
                Direction::LEFT_BOTTOM,
                Direction::RIGHT_BOTTOM
            };
        якщо поточний колір співпадає з checkerColorInBottom - шашка ходе вгору */
        int startIndex = (checkerColorInBottom == currentColor) ?
            0 : 2;

        for (int i = startIndex; i < startIndex + 2; ++i)
        {
            Coordinates coordsOut;
            bool canMove = MovesManager::getSimpleMoveCoordsForSimple(
                coords,
                position,
                MovesManager::DIRECTIONS[i],
                coordsOut
            );

            if (canMove)
            {
                Move* move = new Move;
                MovesManager::setSimpleMove(*move, coords, coordsOut);
                movesList.append(move);
            }
        }
    }
    else
    {
        for (int i = 0; i < MovesManager::DIRECTIONS_SIZE; ++i)
        {
            Coordinates* coordsOut = nullptr;
            int movesSize = MovesManager::getSimpleMoveCoordsForQueen(
                coords,
                position,
                MovesManager::DIRECTIONS[i],
                coordsOut
            );

            if (movesSize)
            {
                for (int j = 0; j < movesSize; ++j)
                {
                    Move* move = new Move;
                    MovesManager::setSimpleMove(*move, coords, coordsOut[j]);
                    movesList.append(move);
                }
            }

            delete[] coordsOut;
        }
    }
}

bool MovesManager::getSimpleMoveCoordsForSimple(
    Coordinates coords,
    const Position& position,
    Direction direction,
    Coordinates& coordsOut
)
{
    // для здійснення ходу наступна клітина має бути пустою
    MovesManager::SearchedData emptyCell = MovesManager::getNextCellObjectType(coords, direction, position);
    // якщо нічого не знайдено АБО наступна клітина НЕ пуста - завершуємо метод
    if (!emptyCell.isSearched || (!Position::isEmptyCell(emptyCell.cellObjectType)))
        return false;

    coordsOut = emptyCell.coords;
    return true;
}

int MovesManager::getSimpleMoveCoordsForQueen(
    Coordinates coords,
    const Position& position,
    Direction direction,
    Coordinates*& coordsOut
)
{
    // простір між самою дамкою та першою перешкодою
    int size = MovesManager::countSpace(coords, direction, position);
    if (!size)
        return size;

    coordsOut = new Coordinates[size];
    for (int i = 0; i < size; ++i)
    {
        MovesManager::nextCoordinates(coords, direction);
        coordsOut[i] = coords;
    }

    return size;
}