#ifndef CHECKERS_MOVES_H
#define CHECKERS_MOVES_H

#include <cstdint>
#include "Position.h"
#include "Structures.h"

namespace CheckersMoves
{
    class MovesValidator;
    class MovesList;
    class MovesManager
    {
    private:
        friend class MovesValidator;
        enum Direction
        {
            LEFT_TOP,
            LEFT_BOTTOM,
            RIGHT_TOP,
            RIGHT_BOTTOM,
        };
        
        static const Direction DIRECTIONS[];
        static const int DIRECTIONS_SIZE;
    public:
        struct Coordinates
        {
            int row;
            int column;
            Coordinates(int rowParam, int columnParam) :
                row(rowParam), column(columnParam) {}
            Coordinates() {}
            // Coordinates& operator=(const Coordinates coords)
            // {
            //     if (this == &coords)
            //         return *this;

            //     row = coords.row;
            //     column = coords.column;
            //     return *this;
            // }

            bool operator==(const Coordinates& other) const
            {
                return (row == other.row) && (column == other.column);
            }

            bool operator!=(const Coordinates& other) const
            {
                return !(*this == other);
            }
        };

        struct Move
        {
            Coordinates from;
            int8_t countMoves;
            Coordinates* to;

            Move() : countMoves(0), to(nullptr) { }

            Move(const Move& other)
                : from(other.from), countMoves(other.countMoves), to(nullptr)
            {
                int size = (countMoves == 0) ? 1 : countMoves;
                if (size > 0 && other.to) {
                    to = new Coordinates[size];
                    for (int i = 0; i < size; ++i)
                        to[i] = other.to[i];
                }
            }

            Move& operator=(const Move& other) {
                if (this == &other) return *this;

                from = other.from;
                countMoves = other.countMoves;

                delete[] to;
                to = nullptr;

                int size = (countMoves == 0) ? 1 : countMoves;
                if (size > 0 && other.to) {
                    to = new Coordinates[size];
                    for (int i = 0; i < size; ++i)
                        to[i] = other.to[i];
                }

                return *this;
            }

            ~Move() { delete[] to; }
        };

        struct MoveInfo
        {
            // Move move;
            Position::CellObjectType beginCheckerType;
            // чи змінювала шашка свій тип під час ходу
            // значення true, якщо шашка стала дамкою
            bool wasTransformed;
            Coordinates pointOfTransformation;
            Position::CellObjectType finalCheckerType;
        };

    private:
        // використовується в якості результату методів, які повертають якийся об'єкт на дошці
        // якщо об'єкт не був знайдений isSearched=false
        struct SearchedData
        {
            Coordinates coords;
            Position::CellObjectType cellObjectType;
            bool isSearched;
        };
    private:
        //                                              
        //          isOuterCall=true,                                       
        //                     .                                                    
        //                             
        static void getTakenMovesForCurrent(
            Coordinates coords,
            const Position& position,
            MovesList& movesList,
            Position::CheckerColor checkerColorInBottom,
            Coordinates from = Coordinates(),
            Structures::Stack<Coordinates>* takesBuffer = nullptr,
            bool isOuterCall = true
        );

        //                                                            
        static bool getTakenMoveCoordsForSimple(
            Coordinates coords,
            const Position& position,
            Direction direction,
            Coordinates& coordsOut
        );

        //                                                     
        // coordsMoves -                ,                               
        //        -                            .
        //                                -                   
        static int getTakenMoveCoordsForQueen(
            Coordinates coords,
            const Position& position,
            Direction direction,
            Coordinates*& coordsMoves
        );

        //                                         
        // checkerColorInBottom -           ,                     
        //        ,                                            (             )
        static void getSimpleMovesForCurrent(
            Coordinates coords,
            Position::CellObjectType currentObjectType,
            const Position& position,
            MovesList& movesList,
            Position::CheckerColor checkerColorInBottom
        );

        //                                                                      
        static bool getSimpleMoveCoordsForSimple(
            Coordinates coords,
            const Position& position,
            Direction direction,
            Coordinates& coordsOut
        );

        //                                                               
        // coordsMoves -                ,                               
        //        -                                 .
        //                                -                   
        static int getSimpleMoveCoordsForQueen(
            Coordinates coords,
            const Position& position,
            Direction direction,
            Coordinates*& coordsOut
        );
        //                                                             -       '     ,
        //                                    .                                                 
        //      ,                      ,                       ,                                                  
        //                                       ,                                          
        static int countSpace(Coordinates coords, Direction direction, const Position& position);

        //                   ,                                    
        static SearchedData searchFirstChecker(Coordinates coords, Direction direction, const Position& position);
        static SearchedData getNextCellObjectType(Coordinates coords, Direction direction, const Position& position);

        static bool isOutOfRange(Coordinates coords);
        //             
        static Position::CellObjectType becomeQueen(Position::CellObjectType currentObjectType);
        static void nextCoordinates(Coordinates& coords, Direction direction);
        static Direction getDirection(Coordinates start, Coordinates end);
        static void setTakenMove(Move& moveOut, int countMoves, Coordinates from, Structures::Stack<Coordinates>* to);
        static void setTakenMove(Move& moveOut, int countMoves, Coordinates from, Coordinates* to);
        static void setSimpleMove(Move& moveOut, Coordinates from, Coordinates to);
    public:
        static void setMove(Move& moveOut, int countMoves, Coordinates from, Coordinates* to);

        //                                      
        //      searchTakenMoves=true,                             ,
        //                              
        static bool getMoves(
            const Position& position,
            Position::CheckerColor checkerThatMovesColor,
            MovesList& movesList,
            Position::CheckerColor checkerColorInBottom,
            bool searchTakenMoves=true
        );

        static void result(
            Position& position,
            const Move& move,
            Position::CheckerColor checkerColorInBottom,
            bool enableTempValues =false
        );

        static void result(
            Position& position,
            const Move& move,
            Position::CheckerColor checkerColorInBottom,
            MoveInfo& outMoveInfo,
            bool enableTempValues =false
            );
    };

    class MovesList
    {
    private:
        using Move = MovesManager::Move;
        Structures::LinkedList<Move> moves;
    private:
        static bool compareMoves(const Move& userMove, const Move& move);
        static bool complexCompareMoves(const Move& userMove, const Move& move);
    public:
        MovesList() : moves() {}
        void append(Move* move) { moves.append(move); }
        void forEach(void (*callback)(Move* move)) const { moves.forEach(callback); }
        bool isEmpty() const { return moves.isEmpty(); }
        bool exists(const Move& move);
        int getSize() const { return moves.getSize(); }
        void start() { moves.start(); }
        bool next() { return moves.next(); }
        Move* getCurrent() { return moves.getCurrent(); }
        //                       -                         ,                                   
        int getCurrentIndex() { return moves.getCurrentIndex(); }

        ~MovesList() {};
    };

    class MovesValidator
    {
    public:
        enum MoveState
        {
            SUCCESS,
            IMPOSSIBLE_MOVE_ERROR,
            MUST_TAKE_ERROR,
            //                '         
            MUST_TAKE_TO_END_ERROR
        };
    private:
        using Coordinates = MovesManager::Coordinates;
        const Position::CheckerColor checkerColorInBottom;
    private:
        static bool isDiagonalMove(Coordinates start, Coordinates end);
        static bool isMoveValid(Coordinates start, Coordinates end);
    public:
        static const MovesManager::Coordinates coordinatesErrorValue;
    public:
        //                             
        static bool hasTakenMovesForCurrent(
            const Position& position,
            Coordinates currentCoords,
            Position::CheckerType currentCheckerType=Position::CheckerType::UNDEFINED_TYPE
        );
        //                       
        static bool hasTakenMoves(
            const Position& position,
            Position::CheckerColor checkerThatMovesColor
        );

        MovesValidator(Position::CheckerColor colorInBottom): 
            checkerColorInBottom(colorInBottom) {}
        MoveState validateSimpleMove(const Position& position, const MovesManager::Move& move);
        MoveState validateTakenMove(
            const Position& position,
            const MovesManager::Move& move,
            Coordinates currentCanTake
        );
    };
}

#endif
