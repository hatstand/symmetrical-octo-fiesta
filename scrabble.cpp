#include "scrabble.h"

#include <iostream>

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;

void Scrabble::FindBestMove(const std::vector<char>& tablet) {
  vector<pair<int, int>> anchors = FindAnchors();
  for (const auto& anchor : anchors) {
    board_[anchor.first + anchor.second * kGridSize] = 'A';
  }
}

namespace {}

// Find valid points on the board to start a word from.
vector<pair<int, int>> Scrabble::FindAnchors() const {
  vector<pair<int, int>> anchors;
  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      // Cannot start a word if there already is a tile there.
      if (HasPlacedTile(i, j)) {
        continue;
      }
      if (!HasNeighbours(i, j)) {
        continue;
      }
      anchors.push_back(make_pair(i, j));
    }
  }
  return anchors;
}

bool Scrabble::HasNeighbours(int i, int j) const {
  return HasPlacedTile(i - 1, j) || HasPlacedTile(i + 1, j) ||
         HasPlacedTile(i, j - 1) || HasPlacedTile(i, j + 1);
}

bool Scrabble::HasPlacedTile(int i, int j) const {
  if (i < 0 || j < 0 || i >= kGridSize || j >= kGridSize) {
    return false;
  }
  return HasPlacedTile(board_[i + j * kGridSize]);
}

bool Scrabble::HasPlacedTile(char tile) const {
  return tile >= 'a' && tile <= 'z';
}

void Scrabble::PrintBoard() const {
  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      cout << board_[i * kGridSize + j];
    }
    cout << endl;
  }
}
