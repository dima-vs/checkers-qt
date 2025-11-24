#include "CheckersMoves.h"

using namespace CheckersMoves;
using Move = MovesManager::Move;
using Coordinates = MovesManager::Coordinates;
using CheckerType = Position::CheckerType;
using CheckerColor = Position::CheckerColor;
using CellObjectType = Position::CellObjectType;

void MovesManager::getTakenMovesForCurrent(
    Coordinates coords,
    const Position& position,
    MovesList& movesList,
    CheckerColor checkerColorInBottom,
    Coordinates from,
    Structures::Stack<Coordinates>* takesBuffer,
    bool isOuterCall
    )
{
    if (isOuterCall)
    {
        from = coords;
        takesBuffer = new Structures::Stack<Coordinates>();
    }
    CellObjectType currentObjectType = position.getCellObjectType(coords.row, coords.column);
    CheckerType currentType = Position::getCheckerType(currentObjectType);
    if (currentType == CheckerType::SIMPLE)
    {
        bool hasMove, isAnyMoveFinded = false;
        // для кожного напрямку
        for (int i = 0; i < MovesManager::DIRECTIONS_SIZE; ++i)
        {
            Coordinates outMoveCoords;
            hasMove = MovesManager::getTakenMoveCoordsForSimple(
                coords,
                position,
                MovesManager::DIRECTIONS[i],
                outMoveCoords
                );
            if (hasMove)
            {
                isAnyMoveFinded = true;
                takesBuffer->push(new Coordinates(outMoveCoords));

                // створюємо проміжний хід, щоб отримати результат після взяття фігури
                Move* tempMove = new Move;
                Structures::Stack<Coordinates> tempToCoords;
                tempToCoords.push(new Coordinates(outMoveCoords));
                MovesManager::setTakenMove(*tempMove, 1, coords, &tempToCoords);
                // в copy буде записаний результуюча позиція після взяття фігури
                // щоб уникнути циклічного взяття однієї і тієї ж фігури
                Position copy = position;

                // copy передається за неконстантним посиланням
                MovesManager::result(
                    copy,
                    *tempMove,
                    checkerColorInBottom,
                    true
                    );

                MovesManager::getTakenMovesForCurrent(
                    outMoveCoords,
                    copy,
                    movesList,
                    checkerColorInBottom,
                    from,
                    takesBuffer,
                    false
                    );

                delete tempMove;
            }
            // якщо буфер НЕ пустий і це остання ітерація
            if ((!takesBuffer->isEmpty()) && (i == (MovesManager::DIRECTIONS_SIZE - 1)))
            {
                // отримуємо розмір буферу до видалення останнього елементу,
                // щоб залишити для нього місце
                int bufferSize = takesBuffer->getSize();
                Coordinates lastCoords = takesBuffer->pop();
                // якщо до цього хоч один хід був знайдений, то поточний хід зберігати
                // не будемо, тому що цей хід є проміжним (шашка завжди має бити до кінця)
                if (isAnyMoveFinded)
                    continue;

                Move* move = new Move;
                MovesManager::setTakenMove(*move, bufferSize, from, takesBuffer);
                // додаємо останній елемент
                move->to[bufferSize - 1] = lastCoords;
                movesList.append(move);
            }
        }

        if (isOuterCall)
        {
            delete takesBuffer;
        }
    }
    else
    {
        bool isAnyMoveFinded = false;
        // для кожного напрямку
        for (int i = 0; i < MovesManager::DIRECTIONS_SIZE; ++i)
        {
            Coordinates* coordsMoves = nullptr;
            // якщо size=0, ходів не знайдено
            int size = MovesManager::getTakenMoveCoordsForQueen(
                coords,
                position,
                MovesManager::DIRECTIONS[i],
                coordsMoves
                );
            if (size)
            {
                isAnyMoveFinded = true;
                for (int j = 0; j < size; ++j)
                {
                    takesBuffer->push(new Coordinates(coordsMoves[j]));

                    Move* tempMove = new Move;
                    Structures::Stack<Coordinates> tempToCoords;
                    tempToCoords.push(new Coordinates(coordsMoves[j]));
                    MovesManager::setTakenMove(*tempMove, 1, coords, &tempToCoords);

                    Position copy = position;
                    // copy передається за неконстантним посиланням
                    MovesManager::result(copy, *tempMove, checkerColorInBottom, true);

                    MovesManager::getTakenMovesForCurrent(
                        coordsMoves[j],
                        copy,
                        movesList,
                        checkerColorInBottom,
                        from,
                        takesBuffer,
                        false
                        );
                    delete tempMove;
                }

                delete[] coordsMoves;
            }
            // якщо буфер НЕ пустий і це остання ітерація
            if ((!takesBuffer->isEmpty()) && (i == (MovesManager::DIRECTIONS_SIZE - 1)))
            {
                // отримуємо розмір буферу до видалення останнього елементу,
                // щоб залишити для нього місце
                int bufferSize = takesBuffer->getSize();
                Coordinates lastCoords = takesBuffer->pop();
                // якщо хоч один хід був знайдений, то пропускаємо ітерацію тому, що
                // цей хід є проміжним
                if (isAnyMoveFinded)
                    continue;

                Move* move = new Move;
                MovesManager::setTakenMove(*move, bufferSize, from, takesBuffer);
                // додаємо останній елемент
                move->to[bufferSize - 1] = lastCoords;
                movesList.append(move);
            }

        }

        if (isOuterCall)
        {
            delete takesBuffer;
        }
    }
}

bool MovesManager::getTakenMoveCoordsForSimple(
    Coordinates coords,
    const Position& position,
    MovesManager::Direction direction,
    Coordinates& coordsOut
    )
{
    CellObjectType currentObjectType = position.getCellObjectType(coords.row, coords.column);
    CheckerColor currentColor = Position::getCheckerColor(currentObjectType);

    // для бою наступна шашка має бути опонентом
    MovesManager::SearchedData opponentChecker = MovesManager::getNextCellObjectType(coords, direction, position);
    // якщо нічого не знайдено АБО наступна клітина пуста - завершуємо метод
    if (!opponentChecker.isSearched || (Position::isEmptyCell(opponentChecker.cellObjectType)))
        return false;

    CheckerColor checkerColor = Position::getCheckerColor(opponentChecker.cellObjectType);
    // якщо колір наступної шашки співпав з поточним - побити неможливо
    if (checkerColor == currentColor)
        return 0;
    // якщо в цьому місті вже була побита шашка опонента, через нього бити не можна
    if (opponentChecker.cellObjectType == Position::TEMP_VALUE)
        return 0;

    // для бою клітина за опонентом має бути пустою
    MovesManager::SearchedData emptyCell = MovesManager::getNextCellObjectType(opponentChecker.coords, direction, position);
    // якщо нічого не знайдено АБО наступна клітина НЕ пуста - завершуємо метод
    if (!emptyCell.isSearched || !(Position::isEmptyCell(emptyCell.cellObjectType)))
        return false;

    coordsOut = emptyCell.coords;
    return true;
}

int MovesManager::getTakenMoveCoordsForQueen(
    Coordinates coords,
    const Position& position,
    Direction direction,
    Coordinates*& coordsMoves
    )
{
    CellObjectType currentObjectType = position.getCellObjectType(coords.row, coords.column);
    CheckerColor currentColor = Position::getCheckerColor(currentObjectType);
    MovesManager::SearchedData opponentChecker = MovesManager::searchFirstChecker(coords, direction, position);
    if (!opponentChecker.isSearched)
        return 0;

    CheckerColor checkerColor = Position::getCheckerColor(opponentChecker.cellObjectType);

    // якщо колір наступної шашки співпав з поточним - побити неможливо
    if (checkerColor == currentColor)
        return 0;
    // якщо в цьому місті вже була побита шашка опонента, через нього бити не можна
    if (opponentChecker.cellObjectType == Position::TEMP_VALUE)
        return 0;

    int size = MovesManager::countSpace(opponentChecker.coords, direction, position);
    if (!size)
        return size;

    coordsMoves = new Coordinates[size];
    for (int i = 0; i < size; ++i)
    {
        MovesManager::nextCoordinates(opponentChecker.coords, direction);
        coordsMoves[i] = opponentChecker.coords;
    }

    return size;
}
