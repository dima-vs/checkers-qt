#include <cstdint>
#include "Position.h"

#define TYPE_FOR_ITERATION int32_t

using CellObjectType = Position::CellObjectType;

int Position::getIndex(int row, int column, bool& half) const
{
    int index = row * 8 + column;

    // обчислюємо номер байта і потрібну половину байта
    // перетворюємо індексацію 8х8 на 16х2
    int byteIndex = index / 4;
    half = (index % 4) >= 2;

    return byteIndex;
}

Position::CheckerColor Position::getCheckerColor(CellObjectType cellObjectType)
{
    if ((cellObjectType == CellObjectType::WHITE_CHECKER) ||
        (cellObjectType == CellObjectType::WHITE_QUEEN))
        return CheckerColor::WHITE;
    else if ((cellObjectType == CellObjectType::BLACK_CHECKER) ||
        (cellObjectType == CellObjectType::BLACK_QUEEN))
        return CheckerColor::BLACK;
    return CheckerColor::UNDEFINED_COLOR;
}

Position::CheckerType Position::getCheckerType(CellObjectType cellObjectType)
{
    if ((cellObjectType == CellObjectType::BLACK_QUEEN) ||
        (cellObjectType == CellObjectType::WHITE_QUEEN))
        return CheckerType::QUEEN;
    else if ((cellObjectType == CellObjectType::BLACK_CHECKER) ||
        (cellObjectType == CellObjectType::WHITE_CHECKER))
        return CheckerType::SIMPLE;
    return CheckerType::UNDEFINED_TYPE;
}

bool Position::isEmptyCell(CellObjectType cellObjectType)
{
    return cellObjectType == CellObjectType::EMPTY_CELL;
}

CellObjectType Position::getCellObjectType(int row, int column) const
{
    // true - якщо шукане значення знаходиться у
    // другій половині байта
    bool half;

    // half передається за посиланням
    int byteIndex = getIndex(row, column, half);

    // зберігаємо цілий байт
    int value = ((char*)position)[byteIndex];

    // якщо значення знаходиться у першій половині байта,
    // здвигаємо value на 4 біта вправо
    value >>= (half ? 0 : 4);

    value &= 0x0F;
    return static_cast<CellObjectType>(value);
}

void Position::setCellObjectType(CellObjectType cellObjectType, int row, int column)
{
    // true - якщо значення, яке потрібно установити
    // знаходиться у другій половині байта
    bool half;

    // half передається за посиланням
    int byteIndex = getIndex(row, column, half);

    // зберігаємо цілий байт
    int value = ((char*)position)[byteIndex];

    // обнуляємо ту половину байта, яка буде установлена
    if (half)
        value &= 0xF0;
    else
        value &= 0x0F;

    // установлюємо частину байта, яка нам потрібна
    value |= (cellObjectType << (half ? 0 : 4));

    ((char*)position)[byteIndex] = value;
}

// operators
bool operator==(const Position& pos1, const Position& pos2)
{
    for (int i = 0; i < POSITION_SIZE / sizeof(TYPE_FOR_ITERATION); ++i)
    {
        if (((TYPE_FOR_ITERATION*)pos1.position)[i] != ((TYPE_FOR_ITERATION*)pos2.position)[i])
            return false;
    }
    return true;
}

Position& Position::operator=(const Position& from)
{
    if (this == &from)
        return *this;

    for (int i = 0; i < POSITION_SIZE / sizeof(TYPE_FOR_ITERATION); ++i)
    {
        ((TYPE_FOR_ITERATION*)position)[i] = ((TYPE_FOR_ITERATION*)from.position)[i];
    }

    blackCheckersCount = from.blackCheckersCount;
    whiteCheckersCount = from.whiteCheckersCount;

    return *this;
}

Position::Position(const Position& other) :
    blackCheckersCount(12),
    whiteCheckersCount(12),
    position(new char[POSITION_SIZE])
{
    for (int i = 0; i < POSITION_SIZE / sizeof(TYPE_FOR_ITERATION); ++i)
    {
        ((TYPE_FOR_ITERATION*)position)[i] = ((TYPE_FOR_ITERATION*)other.position)[i];
    }

    blackCheckersCount = other.blackCheckersCount;
    whiteCheckersCount = other.whiteCheckersCount;
}