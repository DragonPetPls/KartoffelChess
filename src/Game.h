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
    Game();
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
    bool areMovesStillPlayable();

    //Getter and Setter functions
    [[nodiscard]] int getGameHistoryCounter() const;
    piece getPiece(bitboard square);
};

namespace std {
    template <>
    struct hash<Game> {
        std::size_t operator()(const Game& game) const noexcept {
            std::size_t hashValue = 0;

            // Combine the hash of each member, weighted by `std::hash_combine` pattern
            hashValue ^= std::hash<int>{}(game.evaluation) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= std::hash<color>{}(game.currentPlayer) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= std::hash<uint8_t>{}(game.enPassant) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= std::hash<uint8_t>{}(game.castleRights) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);

            // Hash each element in arrays
            for (const auto& board : game.pieceBoards) {
                hashValue ^= std::hash<bitboard>{}(board) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            }

            return hashValue;
        }
    };
}

#endif //SRC_GAME_H
