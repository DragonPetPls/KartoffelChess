//
// Created by fabian on 11/5/24.
//

#ifndef SRC_GAME_H
#define SRC_GAME_H


#include <vector>
#include "constants.h"
#include "MagicBitboards.h"
#include <iostream>

/*
 * Instead of storing a Game object for hashtables, storing a GameKey struct is faster since we do not care about the game history
 */
struct GameKey {
    bitboard pieceBoards[4]; //We compress the boards to reduce the size by overlaying different pieces onto single boards
    uint8_t enPassant;
    uint8_t castleRights;
    color currentPlayer;
    uint64_t hashValue;

    bool operator==(const GameKey& other) const {
        return this->pieceBoards[0] == other.pieceBoards[0] &&
            this->pieceBoards[1] == other.pieceBoards[1] &&
                this->pieceBoards[2] == other.pieceBoards[2] &&
                    this->pieceBoards[3] == other.pieceBoards[3] &&
                        this->enPassant == other.enPassant &&
                            this->castleRights == other.castleRights &&
                                this->currentPlayer == other.currentPlayer;
    }
};
namespace std {
    template<>
    struct hash<GameKey> {
        size_t operator()(const GameKey& key) const {
            return key.hashValue;
        }
    };
}


/*
 * This struct stores everything necessary to undo the move later.
 * capturedPiece has to remain NO_PIECE if a capture was done using en passant
 */
struct LastMove{
    Move move;
    int movesWithoutProgress;
    uint8_t enPassant;
    uint8_t castleRights;
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
    bitboard pieceBoards[14];
    color currentPlayer;
    uint8_t enPassant; // stores the row in which an en passant is playable in a way a bitboard does
    uint8_t castleRights; //Stores castle rights in the first for bits

    //We store the hash value here and update it after every move
    uint64_t hashValue;
    void setHashValue();

private:
    //Functions
    static int getZobristIndex(piece piece, color pieceColor, int index);

    void appendPawnMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const;
    void appendKnightMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendBishopMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendRookMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendQueenMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendKingMoves(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;

    void appendPawnCapturesAndPromotions(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const;
    void appendKnightCaptures(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendBishopCaptures(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendRookCaptures(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendQueenCaptures(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;
    void appendKingCaptures(bitboard square, int index, const bitboard& ownHitmap, const bitboard& enemyHitmap, const bitboard& hitmap, Moves &moves) const;

public:
    //Functions
    Game();
    void loadStartingPosition();
    void printGame() const;
    void doMove(const Move &move);
    void doNullMove();
    void undoMove();
    void doMoveAsString(std::string moveStr);
    Moves getAllPseudoLegalMoves() const;
    Moves getAllPseudoLegalCaptures() const;
    bool isSquareUnderAttack(int index, color attackingColor) const;
    bool isPositionLegal() const;
    void loadFen(const std::string& fen);
    static std::string moveToString(Move move);
    char getStatus();
    bool areMovesStillPlayable();
    GameKey key() const;
    bool checkForRepetition();
    bool isKingInCheck(color kingColor) const;
    static int getIndex(const bitboard& board);
    static void fastForwardIndex(int &index, bitboard& square, bitboard& board);
    LastMove getLastMove() const;

    //Getter and Setter functions
    [[nodiscard]] int getGameHistoryCounter() const;
    piece getPiece(bitboard square) const;
};

#endif //SRC_GAME_H
