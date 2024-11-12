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
    char status;

public:
    //Attributes
    int evaluation; //This is an evaluation updated after every move
    bitboard pieceBoards[14];
    color currentPlayer;
    uint8_t enPassant; // stores the row in which an en passant is playable in a way a bitboard does
    uint8_t castleRights; //Stores castle rights in the first for bits

private:
    //Functions
    static int getIndex(const bitboard& board);

    [[nodiscard]] std::vector<Move> getPawnMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const;
    [[nodiscard]] std::vector<Move> getKnightMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] std::vector<Move> getBishopMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] std::vector<Move> getRookMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] std::vector<Move> getQueenMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] std::vector<Move> getKingMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;

public:
    //Functions
    Game() = default;
    void loadStartingPosition();
    void printGame();
    void doMove(Move move);
    void undoMove();
    void doMoveAsString(std::string moveStr);
    std::vector<Move> getAllPseudoLegalMoves();
    bool isSquareUnderAttack(bitboard square, int index, color attackingColor) const;
    bool isPositionLegal();
    void loadFen(const std::string& fen);
    static std::string moveToString(Move move);
    char getStatus();

    //Getter and Setter functions
    [[nodiscard]] int getGameHistoryCounter() const;
    piece getPiece(bitboard square);
};


#endif //SRC_GAME_H
