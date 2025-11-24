#include "Game.h"
#include <limits>
#include <chrono>

void Game::switchPlayer()
{
    nextPlayer = (nextPlayer == Position::CheckerColor::BLACK) ?
        Position::CheckerColor::WHITE : Position::CheckerColor::BLACK;
}

void Game::initialPosition()
{
    // ініціалізуємо тип шашки гравця та опонента, використовуючи колір гравця
    Position::CellObjectType opponentObjectType = (playerColor == Position::CheckerColor::WHITE) ?
        Position::CellObjectType::BLACK_CHECKER : Position::CellObjectType::WHITE_CHECKER;
    Position::CellObjectType userObjectType = (playerColor == Position::CheckerColor::WHITE) ?
        Position::CellObjectType::WHITE_CHECKER : Position::CellObjectType::BLACK_CHECKER;
    for (int i = 0; i < 8; ++i)
    {
        // якщо i - непарне значення, то j починається з 0
        for (int j = ((i % 2) ? 0 : 1); j < 8; j += 2)
        {
            // в перших трьох рядках (зверху) шашки опонента
            if (i < 3)
                currentPosition.setCellObjectType(opponentObjectType, i, j);
            // в останніх трьох рядках (знизу) шашки гравця
            else if (i > 4)
                currentPosition.setCellObjectType(userObjectType, i, j);
            // всі останні рядки - пусті клітини
            else
                currentPosition.setCellObjectType(Position::EMPTY_CELL, i, j);
        }
    }
    // for (int i = 0; i < 8; ++i)
    // {
    //     // якщо i - непарне значення, то j починається з 0
    //     for (int j = ((i % 2) ? 0 : 1); j < 8; j += 2)
    //     {
    //         // в перших трьох рядках (зверху) шашки опонента
    //         // if (i < 3)
    //         // {
    //         //     currentPosition.setCellObjectType(Position::EMPTY_CELL, i, j);
    //         // }
    //         //     // currentPosition.setCellObjectType(opponentObjectType, i, j);
    //         // // в останніх трьох рядках (знизу) шашки гравця
    //         // else if (i > 4)
    //         //     currentPosition.setCellObjectType(userObjectType, i, j);
    //         // // всі останні рядки - пусті клітини
    //         // else
    //             currentPosition.setCellObjectType(Position::EMPTY_CELL, i, j);
    //     }
    // }
    // // currentPosition.setCellObjectType(opponentObjectType, 0, 1);
    // currentPosition.setCellObjectType(Position::BLACK_QUEEN, 0, 1);
    // currentPosition.setCellObjectType(Position::WHITE_QUEEN, 0, 6);
}


MovesValidator::MoveState Game::tryPlayerMove(
    MovesManager::Move& move,
    MovesManager::MoveInfo& moveInfo
    )
{
    // чи є поточна клітина саме шашкою гравця
    Position::CellObjectType cellObjectType = currentPosition.getCellObjectType(move.from.row, move.from.column);
    if (Position::getCheckerColor(cellObjectType) != playerColor)
        return MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;

    bool hasTakenMoves = movesValidator.hasTakenMoves(currentPosition, playerColor);

    MovesValidator::MoveState moveState;
    if (hasTakenMoves)
        moveState = movesValidator.validateTakenMove(currentPosition, move, currentCanTake);
    else
    {
        move.countMoves = 0;
        moveState = movesValidator.validateSimpleMove(currentPosition, move);
    }

    if (moveState == MovesValidator::MoveState::SUCCESS)
    {
        updateQueenMovesInARow(move);
        MovesManager::result(currentPosition, move, playerColor, moveInfo);
        // переходимо на позицію, де знаходиться шашка вже після ходу
        // і перевіряємо звідти чи може вона ще побити
        int index = (move.countMoves) ? (move.countMoves - 1) : 0;
        hasTakenMoves = false;
        // якщо попередній хід був із взяттям
        if (move.countMoves != 0)
            hasTakenMoves = movesValidator.hasTakenMovesForCurrent(currentPosition, move.to[index]);
        // якщо шашка НЕ може побити, то наступним ходе опонент
        // інакше ланцюг ходів із взяттям ще не закінчився і гравець має продовжувати бити
        if (!hasTakenMoves)
        {
            switchPlayer();
            updateGameState();
            currentCanTake = MovesValidator::coordinatesErrorValue;
        }
        else
        {
            // присвоюємо координати шашки, яка може побити ще
            currentCanTake = move.to[index];
        }
    }

    return moveState;
}


MovesValidator::MoveState Game::tryPlayerMove(MovesManager::Move& move)
{
    // чи є поточна клітина саме шашкою гравця
    Position::CellObjectType cellObjectType = currentPosition.getCellObjectType(move.from.row, move.from.column);
    if (Position::getCheckerColor(cellObjectType) != playerColor)
        return MovesValidator::MoveState::IMPOSSIBLE_MOVE_ERROR;

    bool hasTakenMoves = movesValidator.hasTakenMoves(currentPosition, playerColor);

    MovesValidator::MoveState moveState;
    if (hasTakenMoves)
        moveState = movesValidator.validateTakenMove(currentPosition, move, currentCanTake);
    else
    {
        move.countMoves = 0;
        moveState = movesValidator.validateSimpleMove(currentPosition, move);
    }

    if (moveState == MovesValidator::MoveState::SUCCESS)
    {
        updateQueenMovesInARow(move);
        MovesManager::result(currentPosition, move, playerColor);
        // переходимо на позицію, де знаходиться шашка вже після ходу
        // і перевіряємо звідти чи може вона ще побити
        int index = (move.countMoves) ? (move.countMoves - 1) : 0;
        hasTakenMoves = false;
        // якщо попередній хід був із взяттям
        if (move.countMoves != 0)
            hasTakenMoves = movesValidator.hasTakenMovesForCurrent(currentPosition, move.to[index]);
        // якщо шашка НЕ може побити, то наступним ходе опонент
        // інакше ланцюг ходів із взяттям ще не закінчився і гравець має продовжувати бити
        if (!hasTakenMoves)
        {
            switchPlayer();
            updateGameState();
            currentCanTake = MovesValidator::coordinatesErrorValue;
        }
        else
        {
            // присвоюємо координати шашки, яка може побити ще
            currentCanTake = move.to[index];
        }
    }

    return moveState;
}


void Game::opponentMove(
    MovesManager::Move& move,
    MovesManager::MoveInfo& moveInfo
    )
{
    MovesManager::Move tempMove;
    bool hasMoves = false, tempHasMoves = false, stopDeepening = false;
    currentMaxDepth = (maxDepth >=3)? 3: maxDepth;
    isExceededLimit = false;
    isFirstDeeping = true;
    while (true)
    {
        start = std::chrono::high_resolution_clock::now();
        tempHasMoves = getBestMove(tempMove, 1, stopDeepening);
        if (stopDeepening)
        {
            MovesManager::setMove(move, tempMove.countMoves, tempMove.from, tempMove.to);
            hasMoves = tempHasMoves;
            break;
        }
        // якщо це перше поглиблення обов'язково ініціалізуємо хід
        else if (isFirstDeeping || (!isExceededLimit && currentMaxDepth <= maxDepth))
        {
            MovesManager::setMove(move, tempMove.countMoves, tempMove.from, tempMove.to);
            hasMoves = tempHasMoves;
            ++currentMaxDepth;
            isFirstDeeping = false;
        }
        else
            break;

        if (!(currentMaxDepth <= maxDepth))
            break;
    }

    if (hasMoves)
    {
        updateQueenMovesInARow(move);
        MovesManager::result(currentPosition, move, playerColor, moveInfo);
        switchPlayer();
    }
    updateGameState();
}

void Game::opponentMove(MovesManager::Move& move)
{
    MovesManager::Move tempMove;
    bool hasMoves = false, tempHasMoves = false, stopDeepening = false;
    currentMaxDepth = (maxDepth >=3)? 3: maxDepth;
    isExceededLimit = false;
    isFirstDeeping = true;
    while (true)
    {
        start = std::chrono::high_resolution_clock::now();
        tempHasMoves = getBestMove(tempMove, 1, stopDeepening);
        if (stopDeepening)
        {
            MovesManager::setMove(move, tempMove.countMoves, tempMove.from, tempMove.to);
            hasMoves = tempHasMoves;
            break;
        }
        // якщо це перше поглиблення обов'язково ініціалізуємо хід
        else if (isFirstDeeping || (!isExceededLimit && currentMaxDepth <= maxDepth))
        {
            MovesManager::setMove(move, tempMove.countMoves, tempMove.from, tempMove.to);
            hasMoves = tempHasMoves;
            ++currentMaxDepth;
            isFirstDeeping = false;
        }
        else
            break;

        if (!(currentMaxDepth <= maxDepth))
            break;
    }
    
    if (hasMoves)
    {
        updateQueenMovesInARow(move);
        MovesManager::result(currentPosition, move, playerColor);
        switchPlayer();
    }
    updateGameState();
}

void Game::updateQueenMovesInARow(const MovesManager::Move& move)
{
    Position::CellObjectType cellObjectType = currentPosition.getCellObjectType(move.from.row, move.from.column);
    if (
        Position::getCheckerType(cellObjectType) == Position::CheckerType::QUEEN &&
        move.countMoves == 0
        )
    {
        ++queenMovesInARow;
    }
    else
        queenMovesInARow = 0;
}

void Game::updateGameState()
{
    MovesList movesList;
    bool hasMoves = MovesManager::getMoves(
        currentPosition,
        nextPlayer,
        movesList,
        playerColor
    );
    if (!hasMoves)
    {
        hasMoves = MovesManager::getMoves(
            currentPosition,
            nextPlayer,
            movesList,
            playerColor,
            false
        );
    }

    if (!hasMoves)
    {
        currentState = (playerColor == nextPlayer) ?
            GameState::PLAYER_LOST : GameState::PLAYER_WON;
    }

    if (queenMovesInARow == 15)
        currentState = GameState::DRAW;
}

Position::CheckerColor Game::getOpponentColor()
{
    return (playerColor == Position::CheckerColor::BLACK) ?
        Position::CheckerColor::WHITE : Position::CheckerColor::BLACK;
}

bool Game::getMoves(const Position& position, MovesList& movesList, bool isCallerMaxValue)
{
    Position::CheckerColor currentColor = (isCallerMaxValue) ?
        playerColor : getOpponentColor();

    // спочатку шукаємо ходи із взяттям
    bool hasMoves = MovesManager::getMoves(
        position,
        currentColor,
        movesList,
        playerColor
    );
    // якщо ходів із взяттям немає - шукаємо звичайні
    if (!hasMoves)
    {
        hasMoves = MovesManager::getMoves(
            position,
            currentColor,
            movesList,
            playerColor,
            false
        );
    }
    return hasMoves;
}

bool Game::getBestMove(MovesManager::Move& move, int startDepth, bool& stopDeepening)
{
    MovesList movesList;
    MovesManager::Move* bestMove = nullptr;
    bool hasMoves = getMoves(currentPosition, movesList, false);
    // якщо ходів немає, то min-гравець програв
    if (!hasMoves)
        return false;

    float bestValue = std::numeric_limits<float>::max();
    movesList.start();
    while (movesList.next())
    {
        // якщо час вичерпано
        if (isExceededLimit)
            return false;
        // якщо знайдений тільки один хід - отримуємо його та
        // виходимо з циклу
        if (movesList.getSize() == 1)
        {
            // вказуємо, що більше не потрібно збільшувати глибину пошуку
            stopDeepening = true;
            bestMove = movesList.getCurrent();
            break;
        }

        Position resultPosition;
        resultPosition = currentPosition;
        MovesManager::result(resultPosition, *movesList.getCurrent(), playerColor);

        // на початку depth дорівнює 1
        float value = maxValue(
            resultPosition,
            startDepth,
            -std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );

        if (value <= bestValue)
        {
            bestMove = movesList.getCurrent();
            bestValue = value;
        }
    }
    if (bestMove == nullptr)
        return false;

    if (bestValue > 1.0e30 || bestValue < -1.0e30)
        stopDeepening = true;
    MovesManager::setMove(move, bestMove->countMoves, bestMove->from, bestMove->to);
    return true;
}

float Game::minValue(Position position, int depth, float alpha, float beta)
{
    MovesList movesList;
    bool hasMoves = getMoves(position, movesList, false);
    // якщо ходів немає, то min-гравець програв, тому
    // повертаємо максимальне значення float
    if (!hasMoves)
        // враховуємо depth для того, щоб враховувався найшвидший спосіб перемоги
        return std::numeric_limits<float>::max() - depth * 1.0e35f;
    if (depth > currentMaxDepth)
        return utility(position);

    float bestValue = std::numeric_limits<float>::max();
    movesList.start();
    while (movesList.next())
    {
        std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsedTime = end - start;
        // якщо час пошуку перевищено
        if (elapsedTime.count() >= timeLimit && !isFirstDeeping)
        {
            isExceededLimit = true;
            return 0.;
        }

        Position resultPosition;
        resultPosition = position;
        MovesManager::result(resultPosition, *movesList.getCurrent(), playerColor);

        float value = maxValue(resultPosition, depth, alpha, beta);;
        bestValue = (bestValue < value) ?
            bestValue : value;

        beta = (beta < bestValue) ?
            beta : bestValue;

        if (beta <= alpha)
            break;
    }

    return bestValue;
}

float Game::maxValue(Position position, int depth, float alpha, float beta)
{
    MovesList movesList;
    bool hasMoves = getMoves(position, movesList, true);
    // якщо ходів немає, то max-гравець програв, тому
    // повертаємо мінімальне значення float
    if (!hasMoves)
    {
        // враховуємо depth для того, щоб враховувався найшвидший спосіб перемоги
        return -std::numeric_limits<float>::max() + depth * 1.0e35f;
    }    
    if (depth > currentMaxDepth)
        return utility(position);

    float bestValue = -std::numeric_limits<float>::max();
    movesList.start();
    while (movesList.next())
    {
        std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsedTime = end - start;
        // якщо час пошуку перевищено
        if (elapsedTime.count() >= timeLimit && !isFirstDeeping)
        {
            isExceededLimit = true;
            return 0.;
        }

        Position resultPosition;
        resultPosition = position;
        MovesManager::result(resultPosition, *movesList.getCurrent(), playerColor);

        float value = minValue(resultPosition, depth + 1, alpha, beta);
        bestValue = (value > bestValue) ?
            value : bestValue;

        alpha = (alpha > bestValue) ?
            alpha : bestValue;
        if (beta <= alpha)
            break;
    }

    return bestValue;
}

float Game::utility(const Position& position)
{
    // чим ближче шашка до дамок, тим більший бонус
    float yBonus[8] = { 1.0f, 1.01f, 1.02f, 1.03f, 1.04f, 1.05f, 1.06f, 1.07f };
    float grade = 0;
    float checkerValue = 0;
    for (int i = 0; i < 8; ++i)
    {
        // якщо i - непарне значення, то j починається з 0
        for (int j = ((i % 2) ? 0 : 1); j < 8; j += 2)
        {
            Position::CellObjectType currentObjectType = position.getCellObjectType(i, j);
            if (Position::isEmptyCell(currentObjectType))
                continue;

            Position::CheckerType currentCheckerType = Position::getCheckerType(currentObjectType);
            Position::CheckerColor currentCheckerColor = Position::getCheckerColor(currentObjectType);
            if (currentCheckerType == Position::CheckerType::QUEEN)
                checkerValue = 250;
            else
            {
                checkerValue = 100;
                if (currentCheckerColor == playerColor)
                    checkerValue *= yBonus[7 - j];
                else
                    checkerValue *= yBonus[j];
            }

            if (currentCheckerColor == playerColor)
                grade += checkerValue;
            else
                grade -= checkerValue;

        }
    }
    // чим більша кількість шашок на полі, тим краще, щоб зменшити кількість нічиїх
    grade -= ((float)((playerColor == Position::CheckerColor::WHITE) ? position.getBlackCheckersCount() :
        position.getWhiteCheckersCount())) * 5;

    return grade;
}

void Game::initGame()
{
    switch (hardLevel)
    {
    case 1:
        maxDepth = 1;
        timeLimit = 0;
        break;
    case 2:
        maxDepth = 2;
        timeLimit = 0;
        break;
    case 3:
        maxDepth = 3;
        timeLimit = 0;
        break;
    case 4:
        maxDepth = 4;
        timeLimit = 300;
        break;
    case 5:
        maxDepth = 4;
        timeLimit = 500;
        break;
    case 6:
        maxDepth = 4;
        timeLimit = 2000;
        break;
    case 7:
        maxDepth = 4;
        timeLimit = 3000;
        break;
    case 8:
        maxDepth = 5;
        timeLimit = 4000;
        break;
    case 9:
        maxDepth = 5;
        timeLimit = 7000;
        break;
    case 10:
        maxDepth = 6;
        timeLimit = 9000;
        break;
    }
}

Game::Game(Position::CheckerColor playerColor, int hardLevel) :
    playerColor(playerColor),
    // білі ходять першими
    nextPlayer(Position::CheckerColor::WHITE),
    movesValidator(playerColor),
    hardLevel(hardLevel),
    currentState(GameState::GAME_CONTINUES),
    queenMovesInARow(0),
    currentCanTake(MovesValidator::coordinatesErrorValue)
{
    initGame();
    initialPosition();
}
