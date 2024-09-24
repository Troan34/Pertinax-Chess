#include "Board.h"
#include "Piece.h"
Board::Board()
{
	Piece piece;
	BoardSquare[2] = piece.Bishop + piece.Black;
	BoardSquare[6] = piece.Bishop + piece.Black;
	BoardSquare[0] = piece.Rook | piece.Black;
	BoardSquare[7] = piece.Rook | piece.Black;
	BoardSquare[1] = piece.Knight | piece.Black;
	BoardSquare[6] = piece.Knight | piece.Black;
	BoardSquare[3] = piece.Queen | piece.Black;
	BoardSquare[4] = piece.King | piece.Black;
	BoardSquare[8] = piece.Pawn | piece.Black;
	BoardSquare[9] = piece.Pawn | piece.Black;
	BoardSquare[10] = piece.Pawn | piece.Black;
	BoardSquare[11] = piece.Pawn | piece.Black;
	BoardSquare[12] = piece.Pawn | piece.Black;
	BoardSquare[13] = piece.Pawn | piece.Black;
	BoardSquare[14] = piece.Pawn | piece.Black;
	BoardSquare[15] = piece.Pawn | piece.Black;
	BoardSquare[63] = piece.Rook | piece.White;
	BoardSquare[56] = piece.Rook | piece.White;
	BoardSquare[62] = piece.Knight | piece.White;
	BoardSquare[57] = piece.Knight | piece.White;
	BoardSquare[61] = piece.Bishop | piece.White;
	BoardSquare[58] = piece.Bishop | piece.White;
	BoardSquare[59] = piece.Queen | piece.White;
	BoardSquare[60] = piece.King | piece.White;
	BoardSquare[55] = piece.Pawn | piece.Black;
	BoardSquare[54] = piece.Pawn | piece.Black;
	BoardSquare[53] = piece.Pawn | piece.Black;
	BoardSquare[52] = piece.Pawn | piece.Black;
	BoardSquare[51] = piece.Pawn | piece.Black;
	BoardSquare[50] = piece.Pawn | piece.Black;
	BoardSquare[49] = piece.Pawn | piece.Black;
	BoardSquare[48] = piece.Pawn | piece.Black;
}