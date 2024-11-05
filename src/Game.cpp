//
// Created by fabian on 11/5/24.
//

#include <iostream>
#include "Game.h"

/*
 * This function will set up the typical chess starting position
 */
void Game::loadStartingPosition() {
    pieceBoards[WHITE_PAWN] = WHITE_PAWN_STARTING_POSITION;
    pieceBoards[WHITE_KNIGHT] = WHITE_KNIGHT_STARTING_POSITION;
    pieceBoards[WHITE_BISHOP] = WHITE_BISHOP_STARTING_POSITION;
    pieceBoards[WHITE_ROOK] = WHITE_ROOK_STARTING_POSITION;
    pieceBoards[WHITE_QUEEN] = WHITE_QUEEN_STARTING_POSITION;
    pieceBoards[WHITE_KING] = WHITE_KING_STARTING_POSITION;

    pieceBoards[BLACK_PAWN] = BLACK_PAWN_STARTING_POSITION;
    pieceBoards[BLACK_KNIGHT] = BLACK_KNIGHT_STARTING_POSITION;
    pieceBoards[BLACK_BISHOP] = BLACK_BISHOP_STARTING_POSITION;
    pieceBoards[BLACK_ROOK] = BLACK_ROOK_STARTING_POSITION;
    pieceBoards[BLACK_QUEEN] = BLACK_QUEEN_STARTING_POSITION;
    pieceBoards[BLACK_KING] = BLACK_KING_STARTING_POSITION;

    castleRights = WHITE_SHORT_CASTLE | WHITE_LONG_CASTLE | BLACK_SHORT_CASTLE | BLACK_LONG_CASTLE;
    enPassant = NO_EN_PASSANT;
    currentPlayer = WHITE;
}

/*
 * Print the current board and all other relevant data
 */
void Game::printGame() {

    std::cout << " --------------- " << std::endl;
    bitboard iterator = 1;
    iterator = iterator << 56;
    int counter = 0;
    while(iterator > 0){
        switch (getPiece(iterator)) {
            case WHITE_PAWN:
                std::cout << " P ";
                break;
            case WHITE_KNIGHT:
                std::cout << " N ";
                break;
            case WHITE_BISHOP:
                std::cout << " B ";
                break;
            case WHITE_ROOK:
                std::cout << " R ";
                break;
            case WHITE_QUEEN:
                std::cout << " Q ";
                break;
            case WHITE_KING:
                std::cout << " K ";
                break;
            case BLACK_PAWN:
                std::cout << " p ";
                break;
            case BLACK_KNIGHT:
                std::cout << " n ";
                break;
            case BLACK_BISHOP:
                std::cout << " b ";
                break;
            case BLACK_ROOK:
                std::cout << " r ";
                break;
            case BLACK_QUEEN:
                std::cout << " q ";
                break;
            case BLACK_KING:
                std::cout << " k ";
                break;
            default:
                std::cout << " . ";
        }
        counter++;
        if(counter % 8 == 0){
            std::cout << "\n";
            iterator = iterator >> 15;
        } else {
            iterator = iterator << 1;
        }
    }
}

/*
 * Returns the piece covered by the square,
 * only use it with bitboards that have only one bit true
 */
piece Game::getPiece(bitboard square) {
    int p = -1;
    bool pieceThere = false;

    for(int i = 0; i < 6; i++){
        p += (i + 1) * ((pieceBoards[i + WHITE_PIECE] & square) != 0);
        p += (i + 1 + BLACK_PIECE) * ((pieceBoards[i + BLACK_PIECE] & square) != 0);
    }

    p += (NO_PIECE + 1) * (p == -1);

    return p;
}
