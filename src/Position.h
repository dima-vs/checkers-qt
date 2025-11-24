/*  Для зберігання позиції буде використовуватися
    бітова маска. 32 клітини - гральні клітини.
    Для кожної клітини буде виділено 4 біта,
    оскільки число 3 не кратне 8 (байту),
    отже всього знадобиться 32 * 4 = 128 біт
    або 16 байт
*/

#ifndef POSITION_H
#define POSITION_H

#define POSITION_SIZE 16

#include <cstdint>

class Position
{
public:
    enum CheckerColor
    {
        BLACK,
        WHITE,
        UNDEFINED_COLOR
    };
    enum CheckerType
    {
        QUEEN,
        SIMPLE,
        UNDEFINED_TYPE
    };
    enum CellObjectType
    {
        EMPTY_CELL,
        BLACK_CHECKER,
        WHITE_CHECKER,
        BLACK_QUEEN,
        WHITE_QUEEN,
        TEMP_VALUE
    };
private:
    int8_t blackCheckersCount;
    int8_t whiteCheckersCount;
    void* position;

    int getIndex(int row, int column, bool& half) const;
public:
    static CheckerColor getCheckerColor(CellObjectType cellObjectType);
    static CheckerType getCheckerType(CellObjectType cellObjectType);
    static bool isEmptyCell(CellObjectType cellObjectType);

    friend bool operator==(const Position& pos1, const Position& pos2);
    Position& operator=(const Position& from);

    Position() :
        blackCheckersCount(12),
        whiteCheckersCount(12),
        position(new char[POSITION_SIZE]) {}

    Position(const Position& other);

    CellObjectType getCellObjectType(int row, int column) const;
    void setCellObjectType(CellObjectType cellObjectType, int row, int column);

    int getBlackCheckersCount() const { return blackCheckersCount; }
    int getWhiteCheckersCount() const { return whiteCheckersCount; }
    void decreaseBlackCheckersCount() { --blackCheckersCount; }
    void decreaseWhiteCheckersCount() { --whiteCheckersCount; }

    ~Position() { delete[] position; }
};

#endif
