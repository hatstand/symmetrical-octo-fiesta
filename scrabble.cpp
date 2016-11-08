#include "scrabble.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>

#include <dawgdic/completer.h>
#include <dawgdic/dawg-builder.h>
#include <dawgdic/dawg.h>
#include <dawgdic/dictionary-builder.h>
#include <dawgdic/dictionary.h>
#include <dawgdic/guide-builder.h>
#include <dawgdic/guide.h>

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;
using std::string;
using std::find_if;
using std::find;
using std::map;

namespace {

static map<char, int> kScoreMap = {
    {'a', 1}, {'b', 4}, {'c', 4},  {'d', 2}, {'e', 1},  {'f', 4}, {'g', 3},
    {'h', 3}, {'i', 1}, {'j', 10}, {'k', 5}, {'l', 2},  {'m', 4}, {'n', 2},
    {'o', 1}, {'p', 4}, {'q', 10}, {'r', 1}, {'s', 1},  {'t', 1}, {'u', 2},
    {'v', 5}, {'w', 4}, {'x', 8},  {'y', 3}, {'z', 10},
};

dawgdic::Dawg* BuildDawg() {
  dawgdic::DawgBuilder builder;
  std::string line;
  std::ifstream infile("/usr/share/dict/words");
  while (std::getline(infile, line)) {
    bool only_lower = std::all_of(line.begin(), line.end(),
                                  [](char c) { return c >= 'a' && c <= 'z'; });
    if (only_lower) {
      builder.Insert(line.c_str());
    }
  }
  dawgdic::Dawg* dawg = new dawgdic::Dawg;
  builder.Finish(dawg);
  std::cout << "DAWG size: " << dawg->size() << std::endl;
  return dawg;
}

dawgdic::Dictionary* BuildDictionary(const dawgdic::Dawg& dawg) {
  dawgdic::Dictionary* dictionary = new dawgdic::Dictionary;
  dawgdic::DictionaryBuilder::Build(dawg, dictionary);
  cout << "Dictionary size: " << dictionary->size() << endl;
  return dictionary;
}

dawgdic::Guide* BuildGuide(const dawgdic::Dawg& dawg,
                           const dawgdic::Dictionary& dictionary) {
  dawgdic::Guide* guide = new dawgdic::Guide;
  dawgdic::GuideBuilder::Build(dawg, dictionary, guide);
  return guide;
}

void FindPrefixKeys(const dawgdic::Dictionary& dictionary,
                    const std::string& s) {
  dawgdic::BaseType index = dictionary.root();
  for (std::size_t i = 0; i < s.length(); ++i) {
    if (!dictionary.Follow(s[i], &index)) {
      break;
    }

    if (dictionary.has_value(index)) {
      cout << ' ';
      cout.write(s.c_str(), i + 1);
      cout << " = " << dictionary.value(index) << ";";
    }
  }
  cout << endl;
}

vector<string> CompleteKeys(const dawgdic::Dictionary& dictionary,
                            const dawgdic::Guide& guide, const std::string& s) {
  dawgdic::Completer completer(dictionary, guide);
  dawgdic::BaseType index = dictionary.root();
  vector<string> options;
  if (dictionary.Follow(s.c_str(), s.length(), &index)) {
    completer.Start(index);
    while (completer.Next()) {
      options.push_back(completer.key());
    }
  }
  return options;
}

bool IsRealCharacter(char c) { return c >= 'a' && c <= 'z'; }

static const int kGridSize = 15;

vector<char> Transpose(const vector<char> board) {
  vector<char> transposed(kGridSize * kGridSize);
  for (int x = 0; x < kGridSize; ++x) {
    for (int y = 0; y < kGridSize; ++y) {
      transposed[x + y * kGridSize] = board[y + x * kGridSize];
    }
  }
  assert(transposed.size() == kGridSize * kGridSize);
  return transposed;
}
}

Scrabble::Scrabble(const char* board)
    : board_(board, board + kGridSize * kGridSize),
      dawg_(BuildDawg()),
      dictionary_(BuildDictionary(*dawg_)),
      guide_(BuildGuide(*dawg_, *dictionary_)) {}

Scrabble::~Scrabble() {  // free(board_);
}

void Scrabble::FindBestMove(const std::vector<char>& rack) {
  // vector<pair<int, int>> anchors = FindAnchors();
  vector<pair<int, int>> empty_tiles = FindEmptyTiles();
  for (const auto& tile : empty_tiles) {
    TryPosition(tile, rack);
  }

  cout << "Trying transposed" << endl;
  vector<char> transposed = Transpose(board_);
  board_ = transposed;
  empty_tiles = FindEmptyTiles();
  for (const auto& tile : empty_tiles) {
    TryPosition(tile, rack);
  }
}

void Scrabble::TryPosition(pair<int, int> position,
                           const vector<char>& rack) const {
  for (char c : rack) {
    if (!CrossCheck(string(c, 1), position)) {
      continue;
    }

    string left = GetLeftConnectingCharacters(position);
    string current = left + c + GetRightConnectingCharacters(position);
    Solution solution(position.first - left.size(), position.second, current);
    if (TryPosition(solution, rack)) {
      cout << "Solution: " << current << " at: " << solution.x() << ", "
           << solution.y() << " -> " << Score(solution) << endl;

      vector<string> options = CompleteKeys(*dictionary_, *guide_, current);
      for (const string& s : options) {
        if (s.empty()) {
          continue;
        }
        Solution sol(position.first - left.size(), position.second,
                     current + s);
        if (TryPosition(sol, rack)) {
          cout << "Solution: " << sol.word() << " at: " << sol.x() << ", "
               << sol.y() << " -> " << Score(sol) << endl;
        }
      }
    }
  }
}

Scrabble::Solution::Solution(int x, int y, const string& word)
    : x_(x), y_(y), word_(word) {}

Scrabble::Solution::Solution(pair<int, int> pos, const string& word)
    : Scrabble::Solution::Solution(pos.first, pos.second, word) {}

Scrabble::Rack::Rack(const vector<char>& rack) : rack_(rack) {}

bool Scrabble::Rack::Contains(char c) const {
  auto it = find_if(rack_.begin(), rack_.end(),
                    [c](char d) { return d == c || d == '4'; });
  return it != rack_.end();
}

bool Scrabble::Rack::Take(char c) {
  // Try to find an actual character first.
  auto it = find(rack_.begin(), rack_.end(), c);
  if (it != rack_.end()) {
    rack_.erase(it);
    return true;
  }
  auto blank = find(rack_.begin(), rack_.end(), '4');
  if (blank != rack_.end()) {
    rack_.erase(blank);
    return true;
  }
  return false;
}

bool Scrabble::TryPosition(const Solution& solution,
                           const vector<char>& r) const {
  if (!CrossCheck(solution)) {
    return false;
  }

  if (!dictionary_->Contains(solution.word().c_str())) {
    return false;
  }

  Rack rack(r);

  bool has_anchor = false;
  for (int i = 0; i < solution.word().size(); ++i) {
    char c = solution.word()[i];
    int x = solution.x() + i;
    int y = solution.y();
    // Tile already placed, great!
    if (get(x, y) == c) {
      continue;
    }
    // We've gone off the edge of the board :-(
    if (get(x, y) == '\0') {
      return false;
    }
    if (!rack.Take(c)) {
      return false;
    }
    if (IsAnchor(x, y)) {
      has_anchor = true;
    }
  }
  return has_anchor;
}

// Assumes valid solution.
// TODO: Score blanks correctly.
int Scrabble::Score(const Solution& solution) const {
  int score = 0;
  int word_multiplier = 1;
  for (int i = 0; i < solution.word().size(); ++i) {
    char board = get(solution.x() + i, solution.y());
    int multiplier = 1;
    switch (board) {
      case TL:
        multiplier = 3;
        break;
      case DL:
        multiplier = 2;
        break;
      case DW:
        word_multiplier = 2;
        break;
      case TW:
        word_multiplier = 3;
        break;
      default:
        break;
    }
    score += kScoreMap[solution.word()[i]] * multiplier;
  }
  return score * word_multiplier;
}

void Scrabble::ExpandLeft(string s, pair<int, int> pos,
                          const vector<char>& tiles) const {
  pair<int, int> left_pos = make_pair(pos.first - 1, pos.second);
  if (left_pos.first < 0 || HasPlacedTile(left_pos.first, left_pos.second)) {
    return;
  }

  string left = GetLeftConnectingCharacters(left_pos);

  for (char tile : tiles) {
    string new_word = left + tile + s + GetRightConnectingCharacters(make_pair(
                                            pos.first + s.size(), pos.second));
    vector<string> options = CompleteKeys(*dictionary_, *guide_, new_word);
    if (!options.empty() && CrossCheck(new_word, left_pos)) {
      cout << new_word << " valid at: " << left_pos.first << ","
           << left_pos.second << endl;

      std::vector<char> remaining_tiles(tiles);
      remaining_tiles.erase(
          find(remaining_tiles.begin(), remaining_tiles.end(), tile));
      if (!remaining_tiles.empty()) {
        ExpandLeft(new_word, left_pos, remaining_tiles);
      }
    }
  }
}

bool Scrabble::CrossCheck(const Solution& solution) const {
  return CrossCheck(solution.word(), make_pair(solution.x(), solution.y()));
}

bool Scrabble::CrossCheck(string s, pair<int, int> start_pos) const {
  for (int x = start_pos.first; x < start_pos.first + s.size(); ++x) {
    pair<int, int> current = make_pair(x, start_pos.second);
    string down_word = GetUpConnectingCharacters(current) +
                       s[x - start_pos.first] +
                       GetDownConnectingCharacters(current);
    if (down_word.size() > 1 && !dictionary_->Contains(down_word.c_str())) {
      return false;
    }
  }
  return true;
}

bool Scrabble::IsValidPlacement(char c, pair<int, int> pos) const {
  string left = GetLeftConnectingCharacters(pos);
  string right = GetRightConnectingCharacters(pos);

  string word = left + c + right;

  string up = GetUpConnectingCharacters(pos);
  string down = GetDownConnectingCharacters(pos);

  string down_word = up + c + down;

  return dictionary_->Contains(word.c_str()) &&
         dictionary_->Contains(down_word.c_str());
}

string Scrabble::GetLeftConnectingCharacters(pair<int, int> pos) const {
  std::string ret;
  int x = pos.first - 1;
  int y = pos.second;
  for (int i = x; i >= 0; --i) {
    char c = get(i, y);
    if (IsRealCharacter(c)) {
      ret.insert(ret.begin(), c);
    } else {
      return ret;
    }
  }
  return ret;
}

string Scrabble::GetRightConnectingCharacters(pair<int, int> pos) const {
  string ret;
  int x = pos.first + 1;
  int y = pos.second;
  for (int i = x; i < kGridSize; ++i) {
    char c = get(i, y);
    if (IsRealCharacter(c)) {
      ret.push_back(c);
    } else {
      return ret;
    }
  }
  return ret;
}

string Scrabble::GetUpConnectingCharacters(pair<int, int> pos) const {
  string ret;
  int x = pos.first;
  int y = pos.second - 1;
  for (int i = y; i >= 0; ++i) {
    char c = get(x, i);
    if (IsRealCharacter(c)) {
      ret.insert(ret.begin(), c);
    } else {
      return ret;
    }
  }
  return ret;
}

string Scrabble::GetDownConnectingCharacters(pair<int, int> pos) const {
  string ret;
  int x = pos.first;
  int y = pos.second + 1;
  for (int i = y; i < kGridSize; ++i) {
    char c = get(x, i);
    if (IsRealCharacter(c)) {
      ret.push_back(c);
    } else {
      return ret;
    }
  }
  return ret;
}

// Find valid points on the board to start a word from.
vector<pair<int, int>> Scrabble::FindAnchors() const {
  vector<pair<int, int>> anchors;
  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      if (IsAnchor(i, j)) {
        anchors.push_back(make_pair(i, j));
      }
    }
  }
  return anchors;
}

bool Scrabble::IsAnchor(int x, int y) const {
  return !HasPlacedTile(x, y) && HasNeighbours(x, y);
}

vector<pair<int, int>> Scrabble::FindEmptyTiles() const {
  vector<pair<int, int>> empty_tiles;
  for (int i = 0; i < kGridSize; ++i) {
    for (int j = 0; j < kGridSize; ++j) {
      if (!HasPlacedTile(i, j)) {
        empty_tiles.push_back(make_pair(i, j));
      }
    }
  }
  return empty_tiles;
}

bool Scrabble::HasNeighbours(int i, int j) const {
  return HasPlacedTile(i - 1, j) || HasPlacedTile(i + 1, j) ||
         HasPlacedTile(i, j - 1) || HasPlacedTile(i, j + 1);
}

bool Scrabble::HasPlacedTile(int i, int j) const {
  if (i < 0 || j < 0 || i >= kGridSize || j >= kGridSize) {
    return false;
  }
  return HasPlacedTile(get(i, j));
}

bool Scrabble::HasPlacedTile(char tile) const {
  return tile >= 'a' && tile <= 'z';
}

void Scrabble::PrintBoard() const {
  for (int j = 0; j < kGridSize; ++j) {
    for (int i = 0; i < kGridSize; ++i) {
      cout << get(i, j);
    }
    cout << endl;
  }
}

char Scrabble::get(int x, int y) const {
  if (x < 0 || x >= kGridSize || y < 0 || y > kGridSize) {
    return '\0';
  }
  return board_[x + y * kGridSize];
}
