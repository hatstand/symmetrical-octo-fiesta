#ifndef SCRABBLE_H
#define SCRABBLE_H

#include <stdlib.h>
#include <string.h>

#include <vector>

class Scrabble {
 public:
  Scrabble(const char* board) {
    board_ = static_cast<char*>(malloc(kGridSize * kGridSize));
    memcpy(board_, board, kGridSize * kGridSize);
  }

  ~Scrabble() { free(board_); }

  void FindBestMove(const std::vector<char>& tablet);

  void PrintBoard() const;

 private:
  bool HasNeighbours(int i, int j) const;
  bool HasPlacedTile(int i, int j) const;
  bool HasPlacedTile(char c) const;
  std::vector<std::pair<int, int>> FindAnchors() const;

  static const int kGridSize = 15;
  char* board_;
};

#endif  // SCRABBLE_H
