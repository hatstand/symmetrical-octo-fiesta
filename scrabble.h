#ifndef SCRABBLE_H
#define SCRABBLE_H

#include <stdlib.h>
#include <string.h>

#include <memory>
#include <vector>

enum SpecialSquare {
  TL = 0,
  DL = 1,
  TW = 2,
  DW = 3,
  BLANK = 4,  // Blank tile in rack.
};

namespace dawgdic {
class Dawg;
class Dictionary;
class Guide;
}

class Scrabble {
 public:
  explicit Scrabble(const std::vector<char>& board);
  ~Scrabble();

  void FindBestMove(const std::vector<char>& tablet);

  void PrintBoard() const;

 private:
  class Solution {
   public:
    Solution(int x, int y, const std::string& word);
    Solution(std::pair<int, int> pos, const std::string& word);

    int x() const { return x_; }
    int y() const { return y_; }

    const std::string& word() const { return word_; }

   private:
    int x_;
    int y_;
    std::string word_;
  };

  class Rack {
   public:
    explicit Rack(const std::vector<char>& rack);

    bool Contains(char c) const;
    bool Take(char c);
    // Whether using this character would take a blank.
    bool UsesBlank(char c) const;

   private:
    std::vector<char> rack_;
  };

  bool HasNeighbours(int i, int j) const;
  bool HasPlacedTile(int i, int j) const;
  bool HasPlacedTile(char c) const;
  bool IsAnchor(int x, int y) const;
  bool RowHasAnchors(int y) const;
  std::vector<std::pair<int, int>> FindAnchors() const;
  std::vector<std::pair<int, int>> FindEmptyTiles() const;
  bool IsValidPlacement(char c, std::pair<int, int> pos) const;
  std::string GetLeftConnectingCharacters(std::pair<int, int> pos) const;
  std::string GetRightConnectingCharacters(std::pair<int, int> pos) const;
  std::string GetUpConnectingCharacters(std::pair<int, int> pos) const;
  std::string GetDownConnectingCharacters(std::pair<int, int> pos) const;
  void ExpandLeft(std::string s, std::pair<int, int> pos,
                  const std::vector<char>& tiles) const;
  bool CrossCheck(const std::string& s, std::pair<int, int> pos) const;
  bool CrossCheck(const Solution& solution) const;
  char get(int x, int y) const;

  std::vector<Solution> TryPosition(std::pair<int, int> position,
                                    const std::vector<char>& rack) const;
  std::vector<Solution> TryPositions(
      const std::vector<std::pair<int, int>>& position,
      const std::vector<char>& rack) const;
  bool TryPosition(const Solution& solution,
                   const std::vector<char>& rack) const;
  int Score(const Solution& solution, const std::vector<char>& rack) const;

  std::vector<char> board_;

  std::unique_ptr<dawgdic::Dawg> dawg_;
  std::unique_ptr<dawgdic::Dictionary> dictionary_;
  std::unique_ptr<dawgdic::Guide> guide_;
};

#endif  // SCRABBLE_H
