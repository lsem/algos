#include <string>
#include <vector>

using namespace std;
vector<string> circular_shifts(string s) {
  vector<string> shifts;
  int N = s.length();
  s += s;
  for (int i = 0; i < N; ++i) {
    shifts.emplace_back(string(s, i, N));
  }
  return shifts;
}

string bwt(string s) {
  string bwt;
  auto shifts = circular_shifts(s);
  std::sort(shifts.begin(), shifts.end());
  for (int i = 0; i < shifts.size(); ++i) {
    bwt.push_back(shifts[i][s.size() - 1]);
  }
  return bwt;
}

string inverse_bwt(string s) {
  int N = s.length();
  // prepare empty table
  vector<string> inverse_bwt(N, string(N, '1'));
  for (int k = 0; k < N; ++k) {
    // insert BWT column at front
    for (int j = 0; j < N; ++j) {
      inverse_bwt[j][N - 1 - k] = s[j];
    }
    std::sort(inverse_bwt.begin(), inverse_bwt.end());
  }
  // return string with terminator at right place.
  auto it = std::find_if(
      inverse_bwt.begin(), inverse_bwt.end(),
      [](const std::string &s) { return s[s.length() - 1] == '$'; });

  return *it;
}

// Performs inversing BWT using First-Last property. Works only for ACTG symbols.
string fast_inverse_bwt(string bwt) {
  // following first-last rule.

  int N = bwt.size();
  string last_column = bwt;
  string first_column = std::move(bwt);

  // We know that first column contains the same symbols as last column of BWT
  // matrix (which is BWT itself) just sorted.
  std::sort(first_column.begin(), first_column.end());

  string ibwt(N, 0);

  // For last column, we just need to know the number of occurence at given position,
  // so we can precompute it in O(N) time so then it will be O(1) operation.
  vector<unsigned> last_column_numbers(N, 0);
  int a_cnt = 0, c_cnt = 0, t_cnt = 0, g_cnt = 0;
  for (size_t i = 0; i < N; ++i) {
    switch (last_column[i]) {
    case 'A':
      last_column_numbers[i] = a_cnt++;
      break;
    case 'C':
      last_column_numbers[i] = c_cnt++;
      break;
    case 'T':
      last_column_numbers[i] = t_cnt++;
      break;
    case 'G':
      last_column_numbers[i] = g_cnt++;
      break;
    }
  }

  auto last_col_symbol_number = [&last_column_numbers](size_t pos) {
    return last_column_numbers[pos];
  };

  // e.g. find 20-th occurence of symbol 'T'.
  // Assuming we know that there will be n-tn occurence. Just find beginning of series of this symbol
  // with built-in lower_bound algorithm and then return n-th occurence.
  auto first_col_nth_symbol = [&first_column](int symbol, size_t n) -> size_t {
    auto lb =
        std::lower_bound(first_column.begin(), first_column.end(), symbol);
    return lb - first_column.begin() + n;
  };

  // Do the job.
  size_t row_index = 0;
  ibwt[ibwt.size() - 1] = first_column[row_index];
  for (int i = 1; i < N; ++i) {
    auto x = last_column[row_index];
    ibwt[N - 1 - i] = x;
    // Get number of symbol at position row_index. 
    auto snum = last_col_symbol_number(row_index);
    // Get position of snum's symbol in first columnt. 
    size_t pos = first_col_nth_symbol(x, snum);
    // And this position is our new starting point.
    row_index = pos;
  }

  return ibwt;
}

int main() {

  string text =
      "The Burrows–Wheeler transform (BWT, also called block-sorting "
      "compression) rearranges a character string into runs of similar "
      "characters. This is useful for compression, since it tends to be easy "
      "to compress a string that has runs of repeated characters by techniques "
      "such as move-to-front transform and run-length encoding. More "
      "importantly, the transformation is reversible, without needing to store "
      "any additional data except the position of the first original "
      "character. The BWT is thus a free method of improving the efficiency of "
      "text compression algorithms, costing only some extra computation. To "
      "understand why this creates more-easily-compressible data, consider "
      "transforming a long English text frequently containing the word the. "
      "Sorting the rotations of this text will group rotations starting with "
      "he  together, and the last character of that rotation (which is also "
      "the character before the he ) will usually be t, so the result of the "
      "transform would contain a number of t characters along with the perhaps "
      "less-common exceptions (such as if it contains Brahe ) mixed in. So it "
      "can be seen that the success of this transform depends upon one value "
      "having a high probability of occurring before a sequence, so that in "
      "general it needs fairly long samples (a few kilobytes at least) of "
      "appropriate data (such as text). The inverse can be understood this "
      "way. Take the final table in the BWT algorithm, and erase all but the "
      "last column. Given only this information, you can easily reconstruct "
      "the first column. The last column tells you all the characters in the "
      "text, so just sort these characters alphabetically to get the first "
      "column. Then, the first and last columns (of each row) together give "
      "you all pairs of successive characters in the document, where pairs are "
      "taken cyclically so that the last and first character form a pair. "
      "Sorting the list of pairs gives the first and second columns. "
      "Continuing in this manner, you can reconstruct the entire list. Then, "
      "the row with the end of file character at the end is the original text. "
      "Reversing the example above is done like this. A number of "
      "optimizations can make these algorithms run more efficiently without "
      "changing the output. There is no need to represent the table in either "
      "the encoder or decoder. In the encoder, each row of the table can be "
      "represented by a single pointer into the strings, and the sort "
      "performed using the indices. Some care must be taken to ensure that the "
      "sort does not exhibit bad worst-case behavior: Standard library sort "
      "functions are unlikely to be appropriate. In the decoder, there is also "
      "no need to store the table, and in fact no sort is needed at all. In "
      "time proportional to the alphabet size and string length, the decoded "
      "string may be generated one character at a time from right to left. A "
      "character in the algorithm can be a byte, or a bit, or any other "
      "convenient size. One may also make the observation that mathematically, "
      "the encoded string can be computed as a simple modification of the "
      "suffix array, and suffix arrays can be computed with linear time and "
      "memory. The BWT can be defined with regards to the suffix array SA of "
      "text T as (1-based indexing). Since any rotation of the input string "
      "will lead to the same transformed string, the BWT cannot be inverted "
      "without adding an EOF marker to the end of the input or doing something "
      "equivalent, making it possible to distinguish the input string from all "
      "its rotations. Increasing the size of the alphabet (by appending the "
      "EOF character) makes later compression steps awkward.$";

  printf("bwt is: %s\n", bwt(text).c_str());
  printf("inverse bwt: %s\n", inverse_bwt(bwt(text)).c_str());
}
