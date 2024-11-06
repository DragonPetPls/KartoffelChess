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

    castleRights = WHITE_SHORT_CASTLE_RIGHT | WHITE_LONG_CASTLE_RIGHT | BLACK_SHORT_CASTLE_RIGHT | BLACK_LONG_CASTLE_RIGHT;
    enPassant = NO_EN_PASSANT;
    currentPlayer = WHITE;

    gameHistoryCounter = -1;
}

/*
 * Print the current board and all other relevant data
 */
void Game::printGame() {

    std::string currentPlayerStr;
    if(currentPlayer == WHITE){
        currentPlayerStr = "white";
    } else {
        currentPlayerStr = "black";
    }

    std::cout << "------------------------" << std::endl;
    std::cout << currentPlayerStr << " to move" << std::endl;
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
    std::cout << "En passant " << (int) enPassant << std::endl;
    std::cout << "Castle Rights: ";
    if(castleRights & WHITE_SHORT_CASTLE_RIGHT){
        std::cout << "ws ";
    }
    if(castleRights & WHITE_LONG_CASTLE_RIGHT){
        std::cout << "wl ";
    }
    if(castleRights & BLACK_SHORT_CASTLE_RIGHT){
        std::cout << "bs ";
    }
    if(castleRights & BLACK_LONG_CASTLE_RIGHT){
        std::cout << "bl ";
    }
    std::cout << std::endl;
}

/*
 * Returns the piece covered by the square,
 * only use it with bitboards that have only one bit true
 */
piece Game::getPiece(bitboard square) {
    int p = -1;

    for(int i = 0; i < 6; i++){
        p += (i + 1) * ((pieceBoards[i + WHITE_PIECE] & square) != 0);
        p += (i + 1 + BLACK_PIECE) * ((pieceBoards[i + BLACK_PIECE] & square) != 0);
    }

    p += (NO_PIECE + 1) * (p == -1);

    return p;
}

/*
 * This function executes the move
 */
void Game::doMove(Move move) {

    //Storing everything to undo this move later
    gameHistoryCounter++;
    gameHistory[gameHistoryCounter].enPassant = this->enPassant;
    gameHistory[gameHistoryCounter].castleRights = this->castleRights;
    gameHistory[gameHistoryCounter].eval = this->evaluation;
    gameHistory[gameHistoryCounter].move = move;

    //En passant
    bitboard enPassantCaptureSquare = ((move.fromSquare << 1) | (move.fromSquare >> 1)) & ((move.toSquare >> 8) | (move.toSquare << 8));
    pieceBoards[PAWN | COLOR_TO_PIECE[1 - currentPlayer]] &= ~(enPassantCaptureSquare * ((move.startingPiece == PAWN) && (getPiece(move.toSquare) == NO_PIECE)));

    //Transfering the piece
    pieceBoards[move.startingPiece | COLOR_TO_PIECE[currentPlayer]] &= ~move.fromSquare;
    pieceBoards[move.endingPiece | COLOR_TO_PIECE[currentPlayer]] |= move.toSquare;

    //Removing captured pieces
    piece capturedPiece = NO_PIECE;
    for(int i = 0; i < 6; i++){
        capturedPiece = capturedPiece * !(pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] & move.toSquare) + i * ((pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] & move.toSquare) != 0);
        pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] &= ~move.toSquare;
    }
    gameHistory[gameHistoryCounter].capturedPiece = capturedPiece;

    //Castling
    //short castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= SHORT_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & SHORT_CASTLE_KING) && (move.fromSquare & SHORT_CASTLE_KING));
    //Long castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= LONG_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & LONG_CASTLE_KING) && (move.fromSquare & LONG_CASTLE_KING));

    //Setting castle rights.
    bitboard fromTo = move.fromSquare | move.toSquare;
    castleRights &= ~(WHITE_SHORT_CASTLE_RIGHT * ((fromTo & WHITE_SHORT_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(WHITE_LONG_CASTLE_RIGHT * ((fromTo & WHITE_LONG_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(BLACK_SHORT_CASTLE_RIGHT * ((fromTo & BLACK_SHORT_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(BLACK_LONG_CASTLE_RIGHT * ((fromTo & BLACK_LONG_CASTLE_RIGHTS_MASK) != 0));

    //Setting en passant rights
    enPassant = ((move.fromSquare >> 8) * ((move.startingPiece == PAWN) && (move.fromSquare & WHITE_EN_PASSANT_ROWS) &&  (move.toSquare & WHITE_EN_PASSANT_ROWS)));
    enPassant |= ((move.fromSquare >> 48) * ((move.startingPiece == PAWN) && (move.fromSquare & BLACK_EN_PASSANT_ROWS) &&  (move.toSquare & BLACK_EN_PASSANT_ROWS)));

    //Changing who to move
    currentPlayer = BLACK * (currentPlayer == WHITE);
}

/*
 * Takes in an input such as "e2e4" and executes it
 */
void Game::doMoveAsString(std::string moveStr) {
    Move move;
    int fromX = moveStr[0] - 'a';
    int toX = moveStr[2] - 'a';
    int fromY = moveStr[1] - '0' - 1;
    int toY = moveStr[3] - '0' - 1;

    move.fromSquare = 1;
    move.fromSquare = move.fromSquare << (fromX + 8 * fromY);
    move.toSquare = 1;
    move.toSquare = move.toSquare << (toX + 8 * toY);

    move.startingPiece = getPiece(move.fromSquare) - BLACK_PIECE * (currentPlayer == BLACK);
    if(moveStr.size() == 4){
        move.endingPiece = move.startingPiece;
    } else {
        for(int i = 0; i < 6; i++){
            if(INT_TO_CHAR[i] == moveStr[4]){
                move.endingPiece = i;
            }
        }
    }

    doMove(move);
}

/*
 * This function undoes the last move restoring the previous position
 */
void Game::undoMove() {
    //Restoring saved data
    this->evaluation = gameHistory[gameHistoryCounter].eval;
    this->castleRights = gameHistory[gameHistoryCounter].castleRights;
    this->enPassant = gameHistory[gameHistoryCounter].enPassant;
    piece &capturedPiece = gameHistory[gameHistoryCounter].capturedPiece;
    Move &move = gameHistory[gameHistoryCounter].move;

    //En passant
    bitboard enPassantCaptureSquare = ((move.fromSquare << 1) | (move.fromSquare >> 1)) & ((move.toSquare >> 8) | (move.toSquare << 8));
    pieceBoards[PAWN | COLOR_TO_PIECE[currentPlayer]] |= (enPassantCaptureSquare * ((move.startingPiece == PAWN) && (capturedPiece == NO_PIECE) && (enPassantCaptureSquare != move.fromSquare)));

    //Transfering the piece
    pieceBoards[move.startingPiece | COLOR_TO_PIECE[1 - currentPlayer]] |= move.fromSquare;
    pieceBoards[move.endingPiece | COLOR_TO_PIECE[1 - currentPlayer]] &= ~move.toSquare;

    //Restoring captured pieces
    pieceBoards[capturedPiece | COLOR_TO_PIECE[currentPlayer]] |= move.toSquare * (capturedPiece != NO_PIECE);

    //Changing who to move
    currentPlayer = BLACK * (currentPlayer == WHITE);

    //Castling
    //short castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= SHORT_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & SHORT_CASTLE_KING) && (move.fromSquare & SHORT_CASTLE_KING));
    //Long castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= LONG_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & LONG_CASTLE_KING) && (move.fromSquare & LONG_CASTLE_KING));

    gameHistoryCounter--;
}

int Game::getGameHistoryCounter() const {
    return gameHistoryCounter;
}
