//
// Created by fabian on 11/5/24.
//

#include <iostream>
#include "Game.h"

#include <random>
#include <strings.h>

bool Game::isZobristInit = false;
uint64_t Game::zobristKeys[NUMBER_OF_ZOBRIST_KEYS];

/*
 * This function will set up the typical chess starting position
 */
void Game::loadStartingPosition() {
    status = ON_GOING;
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
    setHashValue();
}

/*
 * Print the current board and all other relevant data
 */
void Game::printGame() const {

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
piece Game::getPiece(bitboard square) const {
    int p = -1;

    for(int i = 0; i < 6; i++){
        p += (i + 1) * ((pieceBoards[i | WHITE_PIECE] & square) != 0);
        p += (i + 1 + BLACK_PIECE) * ((pieceBoards[i | BLACK_PIECE] & square) != 0);
    }

    p += (NO_PIECE + 1) * (p == -1);

    return p;
}

/*
 * This function executes the move
 */
void Game::doMove(const Move &move) {

    //Storing everything to undo this move later
    status = UNKNOWN;
    gameHistoryCounter++;
    gameHistory[gameHistoryCounter].enPassant = this->enPassant;
    gameHistory[gameHistoryCounter].castleRights = this->castleRights;
    gameHistory[gameHistoryCounter].eval = this->evaluation;
    gameHistory[gameHistoryCounter].move = move;
    pastHashes[gameHistoryCounter] = hashValue;

    int fromIndex = getIndex(move.fromSquare);
    int toIndex = getIndex(move.toSquare);

    //En passant
    bitboard enPassantCaptureSquare = ((move.fromSquare << 1) | (move.fromSquare >> 1)) & ((move.toSquare >> 8) | (move.toSquare << 8));
    hashValue ^= zobristKeys[getZobristIndex(PAWN, 1 - currentPlayer, getIndex(enPassantCaptureSquare))] * ((enPassantCaptureSquare & pieceBoards[PAWN | COLOR_TO_PIECE[1 - currentPlayer]]) != 0) * ((move.startingPiece == PAWN) && (getPiece(move.toSquare) == NO_PIECE));
    pieceBoards[PAWN | COLOR_TO_PIECE[1 - currentPlayer]] &= ~(enPassantCaptureSquare * ((move.startingPiece == PAWN) && (getPiece(move.toSquare) == NO_PIECE)));

    //Transfering the piece
    pieceBoards[move.startingPiece | COLOR_TO_PIECE[currentPlayer]] &= ~move.fromSquare;
    pieceBoards[move.endingPiece | COLOR_TO_PIECE[currentPlayer]] |= move.toSquare;

    hashValue ^= zobristKeys[getZobristIndex(move.startingPiece, currentPlayer, fromIndex)];
    hashValue ^= zobristKeys[getZobristIndex(move.endingPiece, currentPlayer, toIndex)];

    //Removing captured pieces
    piece capturedPiece = NO_PIECE;
    for(int i = 0; i < 6; i++){
        capturedPiece = capturedPiece * !(pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] & move.toSquare) + i * ((pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] & move.toSquare) != 0);
        pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]] &= ~move.toSquare;
    }
    gameHistory[gameHistoryCounter].capturedPiece = capturedPiece;
    hashValue ^= zobristKeys[getZobristIndex(capturedPiece, 1 - currentPlayer, toIndex)] * (capturedPiece != NO_PIECE);

    //Castling
    //short castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= SHORT_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & SHORT_CASTLE_KING) && (move.fromSquare & SHORT_CASTLE_KING));
    hashValue ^= zobristKeys[getZobristIndex(ROOK, currentPlayer, SHORT_CASTLE_ROOK_STARTING_INDEX[currentPlayer])] * ((move.toSquare & SHORT_CASTLE_KING) && (move.fromSquare & SHORT_CASTLE_KING));
    hashValue ^= zobristKeys[getZobristIndex(ROOK, currentPlayer, SHORT_CASTLE_ROOK_ENDING_INDEX[currentPlayer])] * ((move.toSquare & SHORT_CASTLE_KING) && (move.fromSquare & SHORT_CASTLE_KING));
    //Long castle
    pieceBoards[ROOK | COLOR_TO_PIECE[currentPlayer]] ^= LONG_CASTLE_ROOK[currentPlayer] * (move.startingPiece == KING) * ((move.toSquare & LONG_CASTLE_KING) && (move.fromSquare & LONG_CASTLE_KING));
    hashValue ^= zobristKeys[getZobristIndex(ROOK, currentPlayer, LONG_CASTLE_ROOK_STARTING_INDEX[currentPlayer])] * ((move.toSquare & LONG_CASTLE_KING) && (move.fromSquare & LONG_CASTLE_KING));
    hashValue ^= zobristKeys[getZobristIndex(ROOK, currentPlayer, LONG_CASTLE_ROOK_ENDING_INDEX[currentPlayer])] * ((move.toSquare & LONG_CASTLE_KING) && (move.fromSquare & LONG_CASTLE_KING));

    //Setting castle rights.
    bitboard fromTo = move.fromSquare | move.toSquare;
    castleRights &= ~(WHITE_SHORT_CASTLE_RIGHT * ((fromTo & WHITE_SHORT_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(WHITE_LONG_CASTLE_RIGHT * ((fromTo & WHITE_LONG_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(BLACK_SHORT_CASTLE_RIGHT * ((fromTo & BLACK_SHORT_CASTLE_RIGHTS_MASK) != 0));
    castleRights &= ~(BLACK_LONG_CASTLE_RIGHT * ((fromTo & BLACK_LONG_CASTLE_RIGHTS_MASK) != 0));

    hashValue ^= zobristKeys[ZOBRIST_WHITE_SHORT_CASTLE_INDEX] * ((fromTo & WHITE_SHORT_CASTLE_RIGHTS_MASK) != 0 && (gameHistory[gameHistoryCounter].castleRights & WHITE_SHORT_CASTLE_RIGHT));
    hashValue ^= zobristKeys[ZOBRIST_WHITE_LONG_CASTLE_INDEX] * ((fromTo & WHITE_LONG_CASTLE_RIGHTS_MASK) != 0 && (gameHistory[gameHistoryCounter].castleRights & WHITE_LONG_CASTLE_RIGHT));
    hashValue ^= zobristKeys[ZOBRIST_BLACK_SHORT_CASTLE_INDEX] * ((fromTo & BLACK_SHORT_CASTLE_RIGHTS_MASK) != 0 && (gameHistory[gameHistoryCounter].castleRights & BLACK_SHORT_CASTLE_RIGHT));
    hashValue ^= zobristKeys[ZOBRIST_BLACK_LONG_CASTLE_INDEX] * ((fromTo & BLACK_LONG_CASTLE_RIGHTS_MASK) != 0 && (gameHistory[gameHistoryCounter].castleRights & BLACK_LONG_CASTLE_RIGHT));

    //Setting en passant rights
    int enPassantIndex = 0;
    for(int i = 0; i < 8; i++) {
        enPassantIndex += i * ((enPassant >> i) & 1);
    }
    hashValue ^= zobristKeys[ZOBRIST_EN_PASSANT_INDEX + enPassantIndex] * (enPassant != 0);
    enPassant = ((move.fromSquare >> 8) * ((move.startingPiece == PAWN) && (move.fromSquare & WHITE_EN_PASSANT_ROWS) &&  (move.toSquare & WHITE_EN_PASSANT_ROWS)));
    enPassant |= ((move.fromSquare >> 48) * ((move.startingPiece == PAWN) && (move.fromSquare & BLACK_EN_PASSANT_ROWS) &&  (move.toSquare & BLACK_EN_PASSANT_ROWS)));
    enPassantIndex = 0;
    for(int i = 0; i < 8; i++) {
        enPassantIndex += i * ((enPassant >> i) & 1);
    }
    hashValue ^= zobristKeys[ZOBRIST_EN_PASSANT_INDEX + enPassantIndex] * (enPassant != 0);

    //Changing who to move
    currentPlayer = BLACK * (currentPlayer == WHITE);
    hashValue ^= zobristKeys[ZOBRIST_COLOR_INDEX];

    //Adjusting moves without progress
    movesWithoutProgess = (movesWithoutProgess + 1) * (move.startingPiece == PAWN || gameHistory[gameHistoryCounter].capturedPiece != NO_PIECE);
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
    move.endingPiece = move.startingPiece;

    if(moveStr.length() > 4) {
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
    status = UNKNOWN;
    //Restoring saved data
    this->evaluation = gameHistory[gameHistoryCounter].eval;
    this->castleRights = gameHistory[gameHistoryCounter].castleRights;
    this->enPassant = gameHistory[gameHistoryCounter].enPassant;
    piece &capturedPiece = gameHistory[gameHistoryCounter].capturedPiece;
    Move &move = gameHistory[gameHistoryCounter].move;
    hashValue = pastHashes[gameHistoryCounter];

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
    movesWithoutProgess = (movesWithoutProgess - 1) * (movesWithoutProgess > 0);
}

/*
 * The history counter is the length of the game
 */
int Game::getGameHistoryCounter() const {
    return gameHistoryCounter;
}

/*
 * Returns all pseudolegal moves that could be played next
 */
Moves Game::getAllPseudoLegalMoves() const {
    Moves moves;

    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    for(int i = 0; i < 6; i++){
        ownHitmap |= pieceBoards[i | COLOR_TO_PIECE[currentPlayer]];
        enemyHitmap |= pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]];
    }
    bitboard hitmap = ownHitmap | enemyHitmap;



    bitboard squareMask = 1;
    for(int i = 0; i < 64; i++){
        //Skipping if there is no own piece
       int skip = 32 * !((NEXT_INDEX_BOARDS[0] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 16 * !((NEXT_INDEX_BOARDS[1] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 8 * !((NEXT_INDEX_BOARDS[2] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 4 * !((NEXT_INDEX_BOARDS[3] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 2 * !((NEXT_INDEX_BOARDS[4] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 1 * !((NEXT_INDEX_BOARDS[5] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;

        //Getting the piece without its color
        piece p = getPiece(squareMask);
        p ^= COLOR_TO_PIECE[currentPlayer];

        switch (p) {
            case PAWN:
                appendPawnMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            case KNIGHT:
                appendKnightMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            case BISHOP:
                appendBishopMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            case ROOK:
                appendRookMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            case QUEEN:
                appendQueenMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            case KING:
                appendKingMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, moves);
                break;
            default:
                break;
        }

        squareMask = squareMask << 1;
    }

    return moves;
}


/*
 * Returns all pseudolegal pawn moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendPawnMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {
    int verticalDirection = 1 * (currentPlayer == WHITE) - 1 * (currentPlayer == BLACK);

    //Advancing 1 square
    bitboard targetSquare = square >> 8;
    targetSquare = targetSquare << (8 * (verticalDirection + 1));

    if(targetSquare & ~hitmap){
        Move m{};
        m.fromSquare = square;
        m.toSquare = targetSquare;
        m.startingPiece = PAWN;
        //Checking for promotion
        if(targetSquare & BACK_ROWS){
            //Promotion
            m.endingPiece = QUEEN;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = ROOK;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = BISHOP;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = KNIGHT;
            moves.moves[moves.moveCount++] = m;
        } else {
            m.endingPiece = PAWN;
            moves.moves[moves.moveCount++] = m;
        }
    }

    //Advancing 2 square
    bitboard intermediateSquare = targetSquare;
    targetSquare = intermediateSquare >> 8;
    targetSquare = targetSquare << (8 * (verticalDirection + 1));
    if(!((targetSquare | intermediateSquare) & hitmap) //No piece in the way
    && (((currentPlayer == WHITE) && (square & WHITE_PAWN_STARTING_POSITION)) || ((currentPlayer == BLACK) && (square & BLACK_PAWN_STARTING_POSITION))) ){ //is pawn in starting ppos
        Move m{};
        m.fromSquare = square;
        m.toSquare = targetSquare;
        m.startingPiece = PAWN;
        m.endingPiece = PAWN;
        moves.moves[moves.moveCount++] = m;
    }

    //Capture to the left
    targetSquare = 1;
    targetSquare = targetSquare << (index + verticalDirection * 8 - 1);
    if(((targetSquare & enemyHitmap) || ((currentPlayer == WHITE && ((targetSquare >> 40) & enPassant)) || (currentPlayer == BLACK && ((targetSquare >> 16) & enPassant))))
        && (index % 8 != 0)){
        Move m{};
        m.fromSquare = square;
        m.toSquare = targetSquare;
        m.startingPiece = PAWN;
        if(targetSquare & BACK_ROWS){
            //Promotion
            m.endingPiece = QUEEN;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = ROOK;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = BISHOP;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = KNIGHT;
            moves.moves[moves.moveCount++] = m;
        } else {
            m.endingPiece = PAWN;
            moves.moves[moves.moveCount++] = m;
        }
    }

    //Capture to the right
    targetSquare = 1;
    targetSquare = targetSquare << (index + verticalDirection * 8 + 1);
    if(((targetSquare & enemyHitmap) || ((currentPlayer == WHITE && ((targetSquare >> 40) & enPassant)) || (currentPlayer == BLACK && ((targetSquare >> 16) & enPassant))))
        && (index % 8 != 7)){
        Move m{};
        m.fromSquare = square;
        m.toSquare = targetSquare;
        m.startingPiece = PAWN;
        if(targetSquare & BACK_ROWS){
            //Promotion
            m.endingPiece = QUEEN;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = ROOK;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = BISHOP;
            moves.moves[moves.moveCount++] = m;
            m.endingPiece = KNIGHT;
            moves.moves[moves.moveCount++] = m;
        } else {
            m.endingPiece = PAWN;
            moves.moves[moves.moveCount++] = m;
        }
    }
}

/*
 * Returns all pseudolegal knight moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendKnightMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {
    MagicBitboards::appendKnightMoves(index, moves);
    for(int i = 0; i < moves.moveCount; i++) {
        if(moves.moves[i].toSquare & ownHitmap) {
            moves.eraseMove(i);
            i--;
        }
    }
}

/*
 * Returns all pseudolegal bishop moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendBishopMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {
    MagicBitboards::appendBishopMoves(hitmap, index, moves);
    //We only check the last for moves for collisions cause we generate our vectors in a way that this works
    for(int i = std::max(0, moves.moveCount - 4); i < moves.moveCount; i++) {
        if(moves.moves[i].toSquare & ownHitmap) {
            moves.eraseMove(i);
            i--;
        }
    }
}

/*
 * Returns all pseudolegal rook moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendRookMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {
    MagicBitboards::appendRookMoves(hitmap, index, moves);
    //We only check the last for moves for collisions cause we generate our vectors in a way that this works;
    for(int i = std::max(0, moves.moveCount - 4); i < moves.moveCount; i++) {
        if(moves.moves[i].toSquare & ownHitmap) {
            moves.eraseMove(i);
            i--;
        }
    }
}

/*
 * Returns all pseudolegal queen moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendQueenMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {
    int oldMoveCount = moves.moveCount;
    appendBishopMoves(square, index, ownHitmap, enemyHitmap, hitmap, moves);

    Moves straight;
    appendRookMoves(square, index, ownHitmap, enemyHitmap, hitmap, straight);
    moves.appendMoves(straight);
    for(int i = oldMoveCount; i < moves.moveCount; i++) {
        moves.moves[i].startingPiece = QUEEN;
        moves.moves[i].endingPiece = QUEEN;
    }
}

/*
 * Returns all pseudolegal king moves from the given square, used by the getAllPseudoLegalMoves function
 */
void Game::appendKingMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap, Moves &moves) const {

    MagicBitboards::appendKingMoves(index, moves);
    for(int i = std::max(0, moves.moveCount - 8); i < moves.moveCount; i++) {
        if(moves.moves[i].toSquare & ownHitmap) {
            moves.eraseMove(i);
            i--;
        }
    }

    //Castle
    uint8_t shortCastleRights[2] = {WHITE_SHORT_CASTLE_RIGHT, BLACK_SHORT_CASTLE_RIGHT};
    uint8_t longCastleRights[2] = {WHITE_LONG_CASTLE_RIGHT, BLACK_LONG_CASTLE_RIGHT};

    //Short Castle
    if((castleRights & shortCastleRights[currentPlayer])
    && !isSquareUnderAttack(square, index, 1 - currentPlayer) //King not in check
    && !isSquareUnderAttack(square << 1, index + 1, 1 - currentPlayer)
    && !(hitmap & (square << 1))
    && !(hitmap & (square << 2))){
        Move m;
        m.fromSquare = square;
        m.toSquare = square << 2;
        m.startingPiece = KING;
        m.endingPiece = KING;
        moves.moves[moves.moveCount++] = m;
    }

    //Long Castle
    if((castleRights & longCastleRights[currentPlayer])
       && !isSquareUnderAttack(square, index, 1 - currentPlayer) //King not in check
       && !isSquareUnderAttack(square >> 1, index - 1, 1 - currentPlayer)
       && !(hitmap & (square >> 1))
       && !(hitmap & (square >> 2))
       && !(hitmap & (square >> 3))){
        Move m;
        m.fromSquare = square;
        m.toSquare = square >> 2;
        m.startingPiece = KING;
        m.endingPiece = KING;
        moves.moves[moves.moveCount++] = m;
    }
}

/*
 * Checks if the square is under attack, the index of the square and the square itself have to match up
 */
bool Game::isSquareUnderAttack(bitboard square, int index, color attackingColor) const {
    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    for(int i = 0; i < 6; i++){
        ownHitmap |= pieceBoards[i | COLOR_TO_PIECE[1 - attackingColor]];
        enemyHitmap |= pieceBoards[i | COLOR_TO_PIECE[attackingColor]];
    }
    bitboard hitmap = ownHitmap | enemyHitmap;

    //Knights
    bitboard dangerousSquares = MagicBitboards::getKnightReachableSquares(index);
    if(dangerousSquares & pieceBoards[COLOR_TO_PIECE[attackingColor] | KNIGHT]) {
        return true;
    }

    //Diagonal attacks
    bitboard diagonalPieces = pieceBoards[BISHOP | COLOR_TO_PIECE[attackingColor]] | pieceBoards[QUEEN | COLOR_TO_PIECE[attackingColor]];
    dangerousSquares = MagicBitboards::getBishopReachableSquares(hitmap, index);
    if(diagonalPieces & dangerousSquares) {
        return true;
    }

    //Straight attacks
    bitboard straightPieces = pieceBoards[ROOK | COLOR_TO_PIECE[attackingColor]] | pieceBoards[QUEEN | COLOR_TO_PIECE[attackingColor]];
    dangerousSquares = MagicBitboards::getRookReachableSquares(hitmap, index);
    if(straightPieces & dangerousSquares) {
        return true;
    }

    //King attacks
    dangerousSquares = MagicBitboards::getKingReachableSquares(index);
    if(pieceBoards[COLOR_TO_PIECE[attackingColor] | KING] & dangerousSquares) {
        return true;
    }

    //Pawn attacks
    int verticalDirection = -1 * (attackingColor == WHITE) + 1 * (attackingColor == BLACK);
    //Capture to the left
    bitboard targetSquare = 1;
    targetSquare = targetSquare << (index + verticalDirection * 8 + 1);
    if((targetSquare & pieceBoards[COLOR_TO_PIECE[attackingColor]]) && (index % 8 != 7)){
        return true;
    }

    //Capture to the right
    targetSquare = 1;
    targetSquare = targetSquare << (index + verticalDirection * 8 - 1);
    if((targetSquare & pieceBoards[COLOR_TO_PIECE[attackingColor]]) && (index % 8 != 0)){
       return true;
    }

    return false;
}

/*
 * Checks if the king of the player not to move can be captured.
 * After performing a pseudo legal move the position should be check via this function
 */
bool Game::isPositionLegal() const {
    int kingPosition = getIndex(pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | KING]);
    return !isSquareUnderAttack(pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | KING], kingPosition, currentPlayer);
}

/*
 * Takes in a board with exactly one piece and returns the index of it
 */
int Game::getIndex(const bitboard &board) {

    int index = 0;
    index += 32 * ((INDEX_BOARDS[0] & board) == 0);
    index += 16 * ((INDEX_BOARDS[1] & board) == 0);
    index += 8 * ((INDEX_BOARDS[2] & board) == 0);
    index += 4 * ((INDEX_BOARDS[3] & board) == 0);
    index += 2 * ((INDEX_BOARDS[4] & board) == 0);
    index += 1 * ((INDEX_BOARDS[5] & board) == 0);

    return index;
}

/*
 * Loads a chess position using a fen as an input
 */
void Game::loadFen(const std::string &fen) {
    gameHistoryCounter= -1;
    status = UNKNOWN;

    for (int i = 0; i < 14; i++) {
        pieceBoards[i] = 0;
    }
    castleRights = 0;
    enPassant = 0;
    currentPlayer = WHITE;

    //Setting up
    int pointer = 63;
    int counter = 0;

    while (pointer >= 0) {
        piece p = NO_PIECE;
        int skip = 0;
        char c = fen[counter];

        if(c > '0' &&  c <= '9') {
            skip = c - '0';
        } else {
            switch(c) {
                case 'P': p = PAWN | WHITE_PIECE; break;
                case 'N': p = KNIGHT | WHITE_PIECE; break;
                case 'B': p = BISHOP | WHITE_PIECE; break;
                case 'R': p = ROOK | WHITE_PIECE; break;
                case 'Q': p = QUEEN | WHITE_PIECE; break;
                case 'K': p = KING | WHITE_PIECE; break;

                case 'p': p = PAWN | BLACK_PIECE; break;
                case 'n': p = KNIGHT | BLACK_PIECE; break;
                case 'b': p = BISHOP | BLACK_PIECE; break;
                case 'r': p = ROOK | BLACK_PIECE; break;
                case 'q': p = QUEEN | BLACK_PIECE; break;
                case 'k': p = KING | BLACK_PIECE; break;
                default: p = NO_PIECE; break;
            }
        }


        if (p == NO_PIECE) {
            pointer -= skip;
        } else {
            bitboard b = 1;
            pieceBoards[p] |= b << 8 * (pointer / 8) + 7 - (pointer % 8);
            pointer--;
        }

        counter++;
    }

    bool castle = false;
    castleRights = 0;
    while (counter < fen.size()) {

        if (fen[counter] == 'w') {
            currentPlayer = WHITE;
        } else if (fen[counter] == 'b') {
            currentPlayer = BLACK;
        } else if (fen[counter] == 'K') {
            castleRights |= WHITE_SHORT_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'Q') {
            castleRights |= WHITE_LONG_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'k') {
            castleRights |= BLACK_SHORT_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'q') {
            castleRights |= BLACK_LONG_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == ' ' && castle) {
            break;
        }
        counter++;
    }
    while (counter < fen.size()) {
        char c = fen[counter];
        if (c >= 'a' && c <= 'h') {
            enPassant = 1 << (c - 'a');
        }
        counter++;
    }
    setHashValue();
}

/*
 * Returns the string such as "e2e4" which belongs to the move
 */
std::string Game::moveToString(Move move) {
    //Generating a hitmaps, hitmaps indicate if a square is occupied or not
    bitboard moveTo = move.toSquare;
    bitboard movedFrom = move.fromSquare;

    bitboard mb[2] = {movedFrom, moveTo};

    std::string str;
    for (int i = 0; i < 2; i++) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {

                if (mb[i] >> (7 - x + 8 * y) == 1) {
                    //Found the square
                    char c = 'a' + 7 - x;
                    str.push_back(c);
                    str.push_back(y + 1 + '0');
                }
            }
        }
    }
    return str;
}

/*
 * Returns the state of the game as WHITE_WON, BLACK_WON, DRAW or ON_GOING
 */
char Game::getStatus() {
    if(status != UNKNOWN) {
        return status;
    }

    //Checking threefold repetion
    int sameCounter = 1;
    for(int i = 0; i <= gameHistoryCounter; i++) {
        sameCounter += pastHashes[i] == hashValue;
    }
    if(sameCounter >= REPETITIONS_TILL_DRAW) {
        status = DRAW;
        return status;
    }

    //checking if there are legal moves left to be played
    bool movesLeft = areMovesStillPlayable();
    if (movesLeft == true) {
        status = ON_GOING;
        return ON_GOING;
    }

    //Checking if draw or checkmate
    int index = getIndex(pieceBoards[COLOR_TO_PIECE[currentPlayer] | KING]);
    bool isCheck = isSquareUnderAttack(pieceBoards[COLOR_TO_PIECE[currentPlayer] | KING], index, 1 - currentPlayer);

    if (isCheck == true && currentPlayer == WHITE) status = BLACK_WON;
    else if (isCheck == true && currentPlayer == BLACK) status = WHITE_WON;
    else status = DRAW;
    return status;
}

/*
 * Returns true if there is atleast one move that can still be played
 */
bool Game::areMovesStillPlayable() {
    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    for(int i = 0; i < 6; i++){
        ownHitmap |= pieceBoards[i | COLOR_TO_PIECE[currentPlayer]];
        enemyHitmap |= pieceBoards[i | COLOR_TO_PIECE[1 - currentPlayer]];
    }
    bitboard hitmap = ownHitmap | enemyHitmap;

    int kingIndex = getIndex(pieceBoards[COLOR_TO_PIECE[currentPlayer] | KING]);
    bitboard dangerousDiagonals = MagicBitboards::getBishopReachableSquares(enemyHitmap, kingIndex);
    bitboard dangerousStraights = MagicBitboards::getRookReachableSquares(enemyHitmap, kingIndex);

    bool rookThread = dangerousStraights & (pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | ROOK] | pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | QUEEN]);
    bool bishopThread = dangerousDiagonals & (pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | BISHOP] | pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | QUEEN]);
    bool isCheck = isSquareUnderAttack(((bitboard) 1) << kingIndex, kingIndex, 1 - currentPlayer);

    bitboard squareMask = 1;
    for(int i = 0; i < 64; i++){

        int skip = 32 * !((NEXT_INDEX_BOARDS[0] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 16 * !((NEXT_INDEX_BOARDS[1] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 8 * !((NEXT_INDEX_BOARDS[2] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 4 * !((NEXT_INDEX_BOARDS[3] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 2 * !((NEXT_INDEX_BOARDS[4] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;
        skip = 1 * !((NEXT_INDEX_BOARDS[5] << i) & ownHitmap);
        squareMask <<= skip;
        i += skip;

        //Getting the piece without its color
        piece p = getPiece(squareMask);
        p ^= COLOR_TO_PIECE[currentPlayer];
        Moves pm;
        switch (p) {
            case PAWN:
                appendPawnMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            case KNIGHT:
                appendKnightMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            case BISHOP:
                appendBishopMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            case ROOK:
                appendRookMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            case QUEEN:
                appendQueenMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            case KING:
                appendKingMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap, pm);
            break;
            default:
                return false;
        }

        for (int j = 0; j < pm.moveCount; j++) {
            bitboard enPassantCaptureSquare = ((pm.moves[j].fromSquare << 1) | (pm.moves[j].fromSquare >> 1)) & ((pm.moves[j].toSquare >> 8) | (pm.moves[j].toSquare << 8));
            if(!bishopThread && !rookThread
                && !(pm.moves[j].startingPiece == PAWN && enPassant != 0 && pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | PAWN] & enPassantCaptureSquare)
                && pm.moves[j].startingPiece != KING && !isCheck
                || (!isCheck
                    && !(pm.moves[j].startingPiece == PAWN && enPassant != 0 && pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | PAWN] & enPassantCaptureSquare)
                    && pm.moves[j].startingPiece != KING && !((dangerousDiagonals | dangerousStraights) & pm.moves[j].startingPiece))) {
                return true;
            }

            doMove(pm.moves[j]);
            if(isPositionLegal()) {
                undoMove();
                return true;
            }
            undoMove();
        }

        squareMask = squareMask << 1;
    }
    return false;
}


/*
 * Constructor
 */
Game::Game(){
    MagicBitboards::init();
    if(!isZobristInit) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
        for(int i = 0; i < NUMBER_OF_ZOBRIST_KEYS; i++) {
            zobristKeys[i] = dis(gen);
        }
        isZobristInit = true;
    }
}

/*
 * Sets the hashValue to the value that corrisponds to the current position
 */
void Game::setHashValue() {
    hashValue = 0;
    for(int i = 0; i < 64; i++) {
        for(int p = 0; p < 6; p++) {
            hashValue ^= zobristKeys[getZobristIndex(p, WHITE, i)] * ((pieceBoards[p | WHITE_PIECE] >> i) & 1);
            hashValue ^= zobristKeys[getZobristIndex(p, BLACK, i)] * ((pieceBoards[p | BLACK_PIECE] >> i) & 1);
        }
    }
    hashValue ^= zobristKeys[ZOBRIST_COLOR_INDEX] * (currentPlayer == BLACK);
    hashValue ^= zobristKeys[ZOBRIST_WHITE_SHORT_CASTLE_INDEX] * ((castleRights & WHITE_SHORT_CASTLE_RIGHT) != 0);
    hashValue ^= zobristKeys[ZOBRIST_WHITE_LONG_CASTLE_INDEX] * ((castleRights & WHITE_LONG_CASTLE_RIGHT) != 0);
    hashValue ^= zobristKeys[ZOBRIST_BLACK_SHORT_CASTLE_INDEX] * ((castleRights & BLACK_SHORT_CASTLE_RIGHT) != 0);
    hashValue ^= zobristKeys[ZOBRIST_BLACK_LONG_CASTLE_INDEX] * ((castleRights & BLACK_LONG_CASTLE_RIGHT) != 0);
    int enPassantIndex = 0;
    for(int i = 0; i < 8; i++) {
        enPassantIndex += i * ((enPassant >> i) & 1);
    }
    hashValue ^= zobristKeys[ZOBRIST_EN_PASSANT_INDEX + enPassantIndex] * (enPassant != 0);
}

/*
 * Returns the index of the zobrist key for a piece of a certain color on a given square
 */
int Game::getZobristIndex(piece piece, color pieceColor, int index) {
    int zobristIndex = 0;
    zobristIndex += index;
    zobristIndex = 6 * zobristIndex + piece;
    zobristIndex = 2 * zobristIndex + pieceColor;
    return zobristIndex;
}
