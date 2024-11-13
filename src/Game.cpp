//
// Created by fabian on 11/5/24.
//

#include <iostream>
#include "Game.h"

#include <strings.h>

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
    status = UNKNOWN;
    gameHistoryCounter++;
    gameHistory[gameHistoryCounter].enPassant = this->enPassant;
    gameHistory[gameHistoryCounter].castleRights = this->castleRights;
    gameHistory[gameHistoryCounter].eval = this->evaluation;
    gameHistory[gameHistoryCounter].move = move;
    pastHashes[gameHistoryCounter] = std::hash<Game>{}(*this);

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
std::vector<Move> Game::getAllPseudoLegalMoves() {
    std::vector<Move> pseudoLegalMoves;
    pseudoLegalMoves.reserve(100);

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
        if(!(ownHitmap & squareMask)){
            squareMask = squareMask << 1;
            continue;
        }

        //Getting the piece without its color
        piece p = getPiece(squareMask);
        p &= ~BLACK_PIECE;

        std::vector<Move> pm;
        pm.reserve(50);
        switch (p) {
            case PAWN:
                pm = getPawnMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            case KNIGHT:
                pm = getKnightMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            case BISHOP:
                pm = getBishopMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            case ROOK:
                pm = getRookMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            case QUEEN:
                pm = getQueenMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            case KING:
                pm = getKingMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
                break;
            default:
                break;
        }
        pseudoLegalMoves.insert(pseudoLegalMoves.end(), pm.begin(), pm.end());
        squareMask = squareMask << 1;
    }

    return pseudoLegalMoves;
}


/*
 * Returns all pseudolegal pawn moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getPawnMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(20);

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
            moves.push_back(m);
            m.endingPiece = ROOK;
            moves.push_back(m);
            m.endingPiece = BISHOP;
            moves.push_back(m);
            m.endingPiece = KNIGHT;
            moves.push_back(m);
        } else {
            m.endingPiece = PAWN;
            moves.push_back(m);
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
        moves.push_back(m);
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
            moves.push_back(m);
            m.endingPiece = ROOK;
            moves.push_back(m);
            m.endingPiece = BISHOP;
            moves.push_back(m);
            m.endingPiece = KNIGHT;
            moves.push_back(m);
        } else {
            m.endingPiece = PAWN;
            moves.push_back(m);
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
            moves.push_back(m);
            m.endingPiece = ROOK;
            moves.push_back(m);
            m.endingPiece = BISHOP;
            moves.push_back(m);
            m.endingPiece = KNIGHT;
            moves.push_back(m);
        } else {
            m.endingPiece = PAWN;
            moves.push_back(m);
        }
    }

    return moves;
}

/*
 * Returns all pseudolegal knight moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getKnightMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(8);

    int x = index % 8;
    int y = index / 8;

    for(int i = 0; i < 8; i++){
        //Going over directions
        int vx = 2 * (i == 0 || i == 1) + 1 * (i == 2 || i == 3) - 1 * (i == 4 || i == 5) - 2 * (i == 6 || i == 7);
        int vy = 2 * (i == 2 || i == 4) + 1 * (i == 0 || i == 6) - 1 * (i == 1 || i == 7) - 2 * (i == 3 || i == 5);

        //Generating target square
        int tx = x + vx;
        int ty = y + vy;
        if(tx < 0 || tx > 7 || ty < 0 || ty > 7){
            continue;
        }

        bitboard targetSquare = 1;
        targetSquare = targetSquare << (tx + 8 * ty);
        if(!(targetSquare & ownHitmap)){
            Move m;
            m.toSquare = targetSquare;
            m.fromSquare = square;
            m.startingPiece = KNIGHT;
            m.endingPiece = KNIGHT;
            moves.push_back(m);
        }
    }

    return moves;
}

/*
 * Returns all pseudolegal bishop moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getBishopMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(16);

    int x = index % 8;
    int y = index / 8;
    //Going over all 4 directions
    for(int i = 0; i < 4; i++){
        int vx = 1 * (i < 2) - 1 * (i >= 2);
        int vy = 1 * (i == 0 || i == 2) - 1 * (i == 1 || i == 3);

        //Going over distances
        for (int distance = 1; distance < 8; distance++){
            int tx = x + vx * distance;
            int ty = y + vy * distance;

            bitboard targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);

            //Collisions and out of bounds
            if((targetSquare & ownHitmap) || tx < 0 || tx > 7 || ty < 0 || ty > 7){
                break;
            }

            //Adding the move
            Move m;
            m.toSquare = targetSquare;
            m.fromSquare = square;
            m.startingPiece = BISHOP;
            m.endingPiece = BISHOP;
            moves.push_back(m);

            //Checking for capture
            if(targetSquare & enemyHitmap){
                break;
            }
        }
    }

    return moves;
}

/*
 * Returns all pseudolegal rook moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getRookMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(16);

    int x = index % 8;
    int y = index / 8;
    //Going over all 4 directions
    for(int i = 0; i < 4; i++){
        int vx = 1 * (i == 0) - 1 * (i == 2);
        int vy = 1 * (i == 1) - 1 * (i == 3);

        //Going over distances
        for (int distance = 1; distance < 8; distance++){
            int tx = x + vx * distance;
            int ty = y + vy * distance;

            bitboard targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);

            //Collisions and out of bounds
            if((targetSquare & ownHitmap) || tx < 0 || tx > 7 || ty < 0 || ty > 7){
                break;
            }

            //Adding the move
            Move m;
            m.toSquare = targetSquare;
            m.fromSquare = square;
            m.startingPiece = ROOK;
            m.endingPiece = ROOK;
            moves.push_back(m);

            //Checking for capture
            if(targetSquare & enemyHitmap){
                break;
            }
        }
    }

    return moves;
}

/*
 * Returns all pseudolegal queen moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getQueenMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(50);

    auto diagonal = getBishopMoves(square, index, ownHitmap, enemyHitmap, hitmap);
    for(Move &m: diagonal) {
        m.startingPiece = QUEEN;
        m.endingPiece = QUEEN;
    }
    moves.insert(moves.end(), diagonal.begin(), diagonal.end());

    auto straight = getRookMoves(square, index, ownHitmap, enemyHitmap, hitmap);
    for(Move &m: straight) {
        m.startingPiece = QUEEN;
        m.endingPiece = QUEEN;
    }
    moves.insert(moves.end(), straight.begin(), straight.end());

    return moves;
}

/*
 * Returns all pseudolegal king moves from the given square, used by the getAllPseudoLegalMoves function
 */
std::vector<Move> Game::getKingMoves(bitboard square, int index, const bitboard &ownHitmap, const bitboard &enemyHitmap, const bitboard &hitmap) const {
    std::vector<Move> moves;
    moves.reserve(8);

    int x = index % 8;
    int y = index / 8;

    //Going over all directions
    for(int i = 0; i < 8; i++){
        int vx = 1 * (i <= 2) - 1 * ((i > 2) && (i <= 5));
        int vy = 1 * (i == 0 || i == 3 || i == 6) - 1 * (i == 1|| i == 4 || i == 7);

        //Generating target location
        int tx = x + vx;
        int ty = y + vy;
        bitboard targetSquare = 1;
        targetSquare = targetSquare << (tx + 8 * ty);
        if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8 && (targetSquare & ~ownHitmap)){
            Move m;
            m.fromSquare = square;
            m.toSquare = targetSquare;
            m.startingPiece = KING;
            m.endingPiece = KING;
            moves.push_back(m);
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
        moves.push_back(m);
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
        moves.push_back(m);
    }

    return moves;
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
    int x = index % 8;
    int y = index / 8;

    for(int i = 0; i < 8; i++){
        //Going over directions
        int vx = 2 * (i == 0 || i == 1) + 1 * (i == 2 || i == 3) - 1 * (i == 4 || i == 5) - 2 * (i == 6 || i == 7);
        int vy = 2 * (i == 2 || i == 4) + 1 * (i == 0 || i == 6) - 1 * (i == 1 || i == 7) - 2 * (i == 3 || i == 5);

        //Generating target square
        int tx = x + vx;
        int ty = y + vy;
        if(tx < 0 || tx > 7 || ty < 0 || ty > 7){
            continue;
        }

        bitboard targetSquare = 1;
        targetSquare = targetSquare << (tx + 8 * ty);
        if(targetSquare & pieceBoards[COLOR_TO_PIECE[attackingColor] | KNIGHT]){
            return true;
        }
    }

    //Straight and diagonal and king
    bitboard diagonalPieces = pieceBoards[BISHOP | COLOR_TO_PIECE[attackingColor]] | pieceBoards[QUEEN | COLOR_TO_PIECE[attackingColor]];
    bitboard straightPieces = pieceBoards[ROOK | COLOR_TO_PIECE[attackingColor]] | pieceBoards[QUEEN | COLOR_TO_PIECE[attackingColor]];
    for(int i = 0; i < 8; i++){
        //Going over directions
        int vx = 1 * (i <= 2) - 1 * ((i > 2) && (i <= 5));
        int vy = 1 * (i == 0 || i == 3 || i == 6) - 1 * (i == 1|| i == 4 || i == 7);


        //checking king checks
        int tx = x + vx;
        int ty = y + vy;
        bitboard targetSquare = 1;
        targetSquare = targetSquare << (tx + 8 * ty);
        if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8 && (targetSquare & pieceBoards[KING | COLOR_TO_PIECE[attackingColor]])){
            return true;
        }

        //Going over distances
        for(int distance = 1; distance < 8; distance++){
            tx = x + vx * distance;
            ty = y + vy * distance;
            targetSquare = 1;
            targetSquare = targetSquare << (tx + 8 * ty);
            bool straight = (vx == 0) || ( vy == 0);
            bool diagonal = !straight;
            if (straight && tx >= 0 && tx < 8 && ty >= 0 && ty < 8 && (targetSquare & straightPieces)){
                return true;
            }
            if (diagonal && tx >= 0 && tx < 8 && ty >= 0 && ty < 8 && (targetSquare & diagonalPieces)){
                return true;
            }
            if(tx < 0 || ty < 0 || tx > 7 || ty > 7 || (targetSquare & hitmap)){
                break;
            }
        }
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
bool Game::isPositionLegal() {
    int kingPosition = getIndex(pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | KING]);
    return !isSquareUnderAttack(pieceBoards[COLOR_TO_PIECE[1 - currentPlayer] | KING], kingPosition, currentPlayer);
}

/*
 * Takes in a board with exactly one piece and returns the index of it
 */
int Game::getIndex(const bitboard &board) {

    int index = 0;
    for(int i = 0; i < 64; i++) {
        index += i * ((board >> i) & 1);
    }

    return index;
}

/*
 * Loads a chess position using a fen as an input
 */
void Game::loadFen(const std::string &fen) {
    gameHistoryCounter= 0;
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
    uint64_t hash = std::hash<Game>{}(*this);
    int sameCounter = 0;
    for(int i = 0; i <= gameHistoryCounter; i++) {
        sameCounter += pastHashes[i] == hash;
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

    bitboard squareMask = 1;
    for(int i = 0; i < 64; i++){
        //Skipping if there is no own piece
        if(!(ownHitmap & squareMask)){
            squareMask = squareMask << 1;
            continue;
        }

        //Getting the piece without its color
        piece p = getPiece(squareMask);
        p &= ~BLACK_PIECE;

        std::vector<Move> pm;
        pm.reserve(50);
        switch (p) {
            case PAWN:
                pm = getPawnMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            case KNIGHT:
                pm = getKnightMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            case BISHOP:
                pm = getBishopMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            case ROOK:
                pm = getRookMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            case QUEEN:
                pm = getQueenMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            case KING:
                pm = getKingMoves(squareMask, i, ownHitmap, enemyHitmap, hitmap);
            break;
            default:
                break;
        }

        for (Move move : pm) {
            doMove(move);
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
