// Good morning! Here's your coding interview problem for today.
// This problem was asked by Epic.
// look and say
// The "look and say" sequence is defined as follows: beginning with the term 1,
// each subsequent term visually describes the digits appearing in the previous
// term. The first few terms are as follows:

// 1
// 11
// 21
// 1211
// 111221
// As an example, the fourth term is 1211, since the third term consists of one
// 2 and one 1.

// Given an integer N, print the Nth term of this sequence.
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string look_and_say(string s) {
  stringstream ss;
  // count subsequent digits of the same value
  char curr = 'z';
  int cnt = 0;
  size_t i = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    if (curr != s[i]) {
      if (curr != 'z') {
        // cout << "dbg: " << curr << " series has ended, count: " << cnt <<
        // "\n";
        ss << cnt << curr;
      }
      curr = s[i];
      cnt = 0;
    }
    cnt++;
  }
  // cout << "dbg: " << curr << " series has ended, count: " << cnt << "\n";
  ss << cnt << curr;
  return ss.str();
}

int main() {
  int N;
  cin >> N;

  string s = "1";
  for (int i = 0; i < N; ++i) {
    cout << s << "\n";
    s = look_and_say(s);
  }
}