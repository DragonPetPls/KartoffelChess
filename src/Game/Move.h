//
// Created by fabian on 11/16/24.
//

#ifndef MOVE_H
#define MOVE_H
#include "constants.h"

/*
 * This struct encodes everything necessary to represent a move
 */
struct Move{
    bitboard fromSquare;
    bitboard toSquare;
    piece startingPiece;
    piece endingPiece; //ending piece is the same as starting piece unless promotion

    // Overload the == operator
    bool operator==(const Move& other) const {
        return fromSquare == other.fromSquare && toSquare == other.toSquare && startingPiece == other.startingPiece && endingPiece == other.endingPiece;
    }
};

constexpr Move nullMove{0, 0, 0, 0};

/*
 * Used to represent multiple moves stored together
 */
class Moves {
public:
    static constexpr uint8_t NONE = 255;
    Move moves[218];
    int moveCount = 0;

    Moves() = default;

    /*
     * Erases the move with the given index
     */
    void eraseMove(int index) {
        for(int i = index; i < moveCount; i++) {
            moves[i] = moves[i+1];
        }
        moveCount--;
    };

    /*
     * adds the moves to the back of this object
     */
    void appendMoves(Moves &m) {
        for(int i = 0; i < m.moveCount; i++) {
            this->moves[this->moveCount++] = m.moves[i];
        }
    }

    /*
    * adds the moves to the back of this object if they dont collide with the collisions bitboard
    */
    void appendMovesWithoutCollision(Moves &m, bitboard collisions) {
        for(int i = 0; i < m.moveCount; i++) {
            if(!(m.moves[i].toSquare & collisions)){
                this->moves[this->moveCount++] = m.moves[i];
            }
        }
    }

    /*
    * adds the moves to the back of this object if they dont collide with the collisions bitboard
    */
    void appendMovesWithoutCollision(Moves &m, bitboard collisions, int checkLastN) {
        for(int i = 0; i < m.moveCount - checkLastN; i++) {
            this->moves[this->moveCount++] = m.moves[i];
        }
        for(int i = std::max(0, m.moveCount - checkLastN); i < m.moveCount; i++) {
            if(!(m.moves[i].toSquare & collisions)){
                this->moves[this->moveCount++] = m.moves[i];
            }
        }
    }

    /*
    * adds the moves to the back of this object if they do collide with the collisions bitboard
   */
    void appendMovesWithCollision(Moves &m, bitboard collisions, int checkLastN) {
        for(int i = std::max(0, m.moveCount - checkLastN); i < m.moveCount; i++) {
            if((m.moves[i].toSquare & collisions)){
                this->moves[this->moveCount++] = m.moves[i];
            }
        }
    }


    /*
     * Copies values of other Moves object
     */
    void copy(Moves &m) {
        moveCount = m.moveCount;
        for(int i = 0; i < moveCount; i++) {
            moves[i] = m.moves[i];
        }
    }

    /*
     * Copy constructor
     */
    Moves(const Moves& m) {
        this->moveCount = m.moveCount;
        for (int i = 0; i < m.moveCount; i++) {
            moves[i] = m.moves[i]; // Copy each element
        }
    }

    /*
     * Same as moveCount(), not the actual size of the internal array
     */
    int size() const {
        return moveCount;
    }

    /*
     * begin of array, useful for for loops
     */
    Move* begin() {
        return moves;
    }

    /*
     * end of array with actual values
     */
    Move* end() {
        return moves + moveCount;
    }

    /*
     * Overloading the [] operator to access moves
    */
    Move& operator[](size_t index) {
        return moves[index];
    }

    // Overload for const objects
    const Move& operator[](size_t index) const {
        return moves[index];
    }
};

#endif //MOVE_H
