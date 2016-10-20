#include "scrabble.h"

#include <algorithm>
#include <fstream>
#include <iostream>

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

namespace {

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

void CompleteKeys(const dawgdic::Dictionary& dictionary,
                  const dawgdic::Guide& guide, const std::string& s) {
  dawgdic::Completer completer(dictionary, guide);
  dawgdic::BaseType index = dictionary.root();
  if (dictionary.Follow(s.c_str(), s.length(), &index)) {
    completer.Start(index);
    while (completer.Next()) {
      cout << ' ' << s << completer.key() << " = " << completer.value();
    }
  }
  cout << endl;
}
}

Scrabble::Scrabble(const char* board)
    : board_(static_cast<char*>(malloc(kGridSize * kGridSize))),
      dawg_(BuildDawg()),
      dictionary_(BuildDictionary(*dawg_)),
      guide_(BuildGuide(*dawg_, *dictionary_)) {
  memcpy(board_, board, kGridSize * kGridSize);
}

Scrabble::~Scrabble() { free(board_); }

void Scrabble::FindBestMove(const std::vector<char>& tablet) {
  CompleteKeys(*dictionary_, *guide_, "nua");
  vector<pair<int, int>> anchors = FindAnchors();
  for (const auto& anchor : anchors) {
    for (char c : tablet) {
      cout << "Trying: " << c << " at: " << anchor.first << "," << anchor.second
           << endl;
      cout << IsValidPlacement(c, anchor) << endl;
    }
  }
}

bool Scrabble::IsValidPlacement(char c, pair<int, int> pos) const {
  string left = GetLeftConnectingCharacters(pos);
  string right = GetRightConnectingCharacters(pos);

  string word = left + c + right;
  cout << word << endl;

  return dictionary_->Contains(word.c_str());
}

namespace {
bool IsRealCharacter(char c) { return c >= 'a' && c <= 'z'; }
}

string Scrabble::GetLeftConnectingCharacters(pair<int, int> pos) const {
  std::string ret;
  int x = pos.first - 1;
  int y = pos.second;
  for (int i = x; i >= 0; --i) {
    char c = board_[i + y * kGridSize];
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
    char c = board_[i + y * kGridSize];
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
