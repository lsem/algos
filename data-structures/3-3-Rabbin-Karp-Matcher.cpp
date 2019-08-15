#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void match(const string& text,
           const string& pattern,
           std::function<void(size_t pos)> match_cb) {
  const uint64_t PRIME = 1000000007;
  int64_t pattern_hash = 0, window_hash = 0, y = 1;

  // initialization
  for (uint i = 0; i < pattern.size(); ++i) {
    size_t index = i;
    pattern_hash = (pattern_hash * 263 + pattern[index]) % PRIME;
    window_hash = (window_hash * 263 + text[index]) % PRIME;
    y = y * 263 % PRIME;
  }

  for (size_t i = 0; i < text.size() - pattern.size() + 1; ++i) {
    if (pattern_hash == window_hash) {
      if (text.compare(i, pattern.size(), pattern, 0, pattern.size()) == 0) {
        match_cb(i);
      }
    }

    if (i < text.size() - pattern.size()) {
      uint8_t c0 = text[i], c1 = text[i + pattern.size()];
      window_hash = ((263 * window_hash % PRIME) + c1 - (y * c0 % PRIME));
      // handle negative window_hash
      window_hash = (window_hash % PRIME + PRIME) % PRIME;
    }
  }
}

int main() {
  string pattern, text;

  cin >> pattern;
  cin >> text;

  match(text, pattern, [&](size_t pos) { cout << pos << " "; });
  cout << endl;
}
