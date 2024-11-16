//
// Created by fabian on 11/5/24.
//

#ifndef SRC_GAME_H
#define SRC_GAME_H


#include <vector>
#include "constants.h"
#include "MagicBitboards.h"



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
    static uint64_t zobristKeys[NUMBER_OF_ZOBRIST_KEYS];
    static bool isZobristInit;

    //The game history array keeps track of last moves in order to be able to undo them, the game historyCounter points to the last move.
    //If the gameHistoryCounter = -1 no move can be undone
    LastMove gameHistory[MAX_GAME_LENGTH];
    uint64_t pastHashes[MAX_GAME_LENGTH];
    int gameHistoryCounter;
    char status;
    int movesWithoutProgess;

public:
    //Attributes
    int evaluation; //This is an evaluation updated after every move
    bitboard pieceBoards[14];
    color currentPlayer;
    uint8_t enPassant; // stores the row in which an en passant is playable in a way a bitboard does
    uint8_t castleRights; //Stores castle rights in the first for bits

    //We store the hash value here and update it after every move
    uint64_t hashValue;
    void setHashValue();

private:
    //Functions
    static int getIndex(const bitboard& board);
    static int getZobristIndex(piece piece, color pieceColor, int index);

    [[nodiscard]] Moves getPawnMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const;
    [[nodiscard]] Moves getKnightMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] Moves getBishopMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] Moves getRookMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] Moves getQueenMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;
    [[nodiscard]] Moves getKingMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap) const;

public:
    //Functions
    Game();
    void loadStartingPosition();
    void printGame() const;
    void doMove(const Move &move);
    void undoMove();
    void doMoveAsString(std::string moveStr);
    Moves getAllPseudoLegalMoves() const;
    bool isSquareUnderAttack(bitboard square, int index, color attackingColor) const;
    bool isPositionLegal() const;
    void loadFen(const std::string& fen);
    static std::string moveToString(Move move);
    char getStatus();
    bool areMovesStillPlayable();

    //Getter and Setter functions
    [[nodiscard]] int getGameHistoryCounter() const;
    piece getPiece(bitboard square) const;
};

#endif //SRC_GAME_H
