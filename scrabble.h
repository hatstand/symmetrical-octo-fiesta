#ifndef SCRABBLE_H
#define SCRABBLE_H

#include <stdlib.h>
#include <string.h>

#include <memory>
#include <vector>

namespace dawgdic {
class Dawg;
class Dictionary;
class Guide;
}

class Scrabble {
 public:
  explicit Scrabble(const char* board);
  ~Scrabble();

  void FindBestMove(const std::vector<char>& tablet);

  void PrintBoard() const;

 private:
  bool HasNeighbours(int i, int j) const;
  bool HasPlacedTile(int i, int j) const;
  bool HasPlacedTile(char c) const;
  std::vector<std::pair<int, int>> FindAnchors() const;
  bool IsValidPlacement(char c, std::pair<int, int> pos) const;
  std::string GetLeftConnectingCharacters(std::pair<int, int> pos) const;
  std::string GetRightConnectingCharacters(std::pair<int, int> pos) const;

  static const int kGridSize = 15;
  char* board_;

  std::unique_ptr<dawgdic::Dawg> dawg_;
  std::unique_ptr<dawgdic::Dictionary> dictionary_;
  std::unique_ptr<dawgdic::Guide> guide_;
};

#endif  // SCRABBLE_H
