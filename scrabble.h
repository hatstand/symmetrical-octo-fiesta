#ifndef SCRABBLE_H
#define SCRABBLE_H

#include <vector>

class Scrabble {
 public:
  Scrabble(const char* board) : board_(board) {}

  void FindBestMove(const std::vector<char>& tablet);

  void PrintBoard() const;

 private:
  bool HasNeighbours(int i, int j) const;
  bool HasPlacedTile(int i, int j) const;
  bool HasPlacedTile(char c) const;
  std::vector<std::pair<int, int>> FindAnchors() const;

  static const int kGridSize = 15;
  const char* board_;
};

#endif  // SCRABBLE_H
