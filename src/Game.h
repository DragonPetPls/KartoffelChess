//
// Created by fabian on 11/5/24.
//

#ifndef SRC_GAME_H
#define SRC_GAME_H


#include <vector>
#include "constants.h"

/*
 * This struct encodes everything necessary to represent a move
 */
struct Move{
    bitboard fromSquare;
    bitboard toSquare;
    piece startingPiece;
    piece endingPiece; //ending piece is the same as starting piece unless promotion
};

/*
 * This struct stores everything necessary to undo the move later.
 * capturedPiece has to remain NO_PIECE if a capture was done using en passant
 */
struct LastMove{
    Move move;
    int eval;
    int enPassant;
    int castleRights;
    piece capturedPiece;
};

class Game {
private:
    //Attributes

    //The game history array keeps track of last moves in order to be able to undo them, the game historyCounter points to the last move.
    //If the gameHistoryCounter = -1 no move can be undone
    LastMove gameHistory[MAX_GAME_LENGTH];
    int gameHistoryCounter;

public:
    //Attributes
    int evaluation; //This is an evaluation updated after every move
    bitboard pieceBoards[14];
    color currentPlayer;
    uint8_t enPassant; // stores the row in which an en passant is playable in a way a bitboard does
    uint8_t castleRights; //Stores castle rights in the first for bits

private:
    //Functions

public:
    //Functions
    Game() = default;
    void loadStartingPosition();
    void printGame();
    piece getPiece(bitboard square);
    void doMove(Move move);
    void undoMove();
    void doMoveAsString(std::string moveStr);

    int getGameHistoryCounter() const;
};


#endif //SRC_GAME_H
