#ifndef GAME_H
#define GAME_H

#include "Position.h"
#include "CheckersMoves.h"
#include <chrono>

using namespace CheckersMoves;
class Game
{
public:
    enum GameState
    {
        GAME_CONTINUES,
        DRAW,
        PLAYER_WON,
        PLAYER_LOST
    };
private:
    std::chrono::steady_clock::time_point start;
    bool isExceededLimit;
    // використовується в функціяї maxValue, minValue
    // якщо значення true, то перебір не припиняється навіть
    // якщо перевищено часовий ліміт
    bool isFirstDeeping;
    int timeLimit;
    // колір шашок гравця, вони завжди будуть знаходитися знизу позиції
    const Position::CheckerColor playerColor;
    // хто ходе наступний
    Position::CheckerColor nextPlayer;
    Position currentPosition;
    MovesValidator movesValidator;
    int currentMaxDepth;
    int maxDepth;
    int hardLevel;
    GameState currentState;
    // ходи дамкою підряд для фіксування нічиї
    // нічия зізнається у випадку, якщо останні 15 ходів були зроблені дамками без
    // пересування простих шашок та взяття фішок супротивника
    int queenMovesInARow;
    // координати шашки, яка може побити ще,
    // тобто ланцюг ходів ще не закінчився
    // якщо currentCanTake == coordinatesErrorValue,
    // то ланцюга ходів немає
    MovesManager::Coordinates currentCanTake;
private:
    void initGame();
    void initialPosition();
    void updateQueenMovesInARow(const MovesManager::Move& move);
    void switchPlayer();
    Position::CheckerColor getOpponentColor();
    bool getMoves(const Position& position, MovesList& movesList, bool isCallerMaxValue);
    void updateGameState();
    // minimax
    // гравець - maxValue, бот - minValue
    bool getBestMove(
        MovesManager::Move& move,
        int startDepth,
        // припинити поглиблення пошуку, використовується, якщо
        // існує тільки один можливий хід
        bool& stopDeepening
    );

    float minValue(Position position, int depth, float alpha, float beta);
    float maxValue(Position position, int depth, float alpha, float beta);
    float utility(const Position& position);
public:
    Game(Position::CheckerColor playerColor, int hardLevel);
    MovesValidator::MoveState tryPlayerMove(MovesManager::Move& move);
    void opponentMove(MovesManager::Move& move);

    MovesValidator::MoveState tryPlayerMove(
        MovesManager::Move& move,
        MovesManager::MoveInfo& moveInfo
        );
    void opponentMove(
        MovesManager::Move& move,
        MovesManager::MoveInfo& moveInfo
        );

    const Position& getCurrentPosition() { return currentPosition; }
    GameState getCurrentState() { return currentState; }
    Position::CheckerColor whoIsNext() { return nextPlayer; }
};

#endif
