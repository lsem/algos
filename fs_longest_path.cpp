
#include <iostream>
#include <string>

using namespace std;

// Suppose we represent our file system by a string in the following manner:

// The string "dir\n\tsubdir1\n\tsubdir2\n\t\tfile.ext" represents:

// dir
//     subdir1
//     subdir2
//         file.ext
// The directory dir contains an empty sub-directory subdir1 and a sub-directory
// subdir2 containing a file file.ext.

// The string
// "dir\n\tsubdir1\n\t\tfile1.ext\n\t\tsubsubdir1\n\tsubdir2\n\t\tsubsubdir2\n\t\t\tfile2.ext"
// represents:

// dir
//     subdir1
//         file1.ext
//         subsubdir1
//     subdir2
//         subsubdir2
//             file2.ext
// The directory dir contains two sub-directories subdir1 and subdir2. subdir1
// contains a file file1.ext and an empty second-level sub-directory subsubdir1.
// subdir2 contains a second-level sub-directory subsubdir2 containing a file
// file2.ext.

// We are interested in finding the longest (number of characters) absolute path
// to a file within our file system. For example, in the second example above,
// the longest absolute path is "dir/subdir2/subsubdir2/file2.ext", and its
// length is 32 (not including the double quotes).

uint max_total_length(string s) {
  int total_length = 0, prev_current_length = 0, current_length = 0,
      max_total_length = 0, depth = 0, prev_depth = -1;

  for (int i = 0; i < s.size(); ++i) {
    if (s[i] == '\n') {
      if (depth > prev_depth) {
        // depth increased
        cout << "adding current_length: " << current_length << endl;
        total_length += current_length + 1;
      } else if (depth < prev_depth) {
        cout << "substrucing prev_current_length: " << prev_current_length
             << endl;
        cout << "adding current_length: " << current_length << endl;
        total_length -= prev_current_length + 1;
      } else if (depth == prev_depth) {
        total_length -= prev_current_length + 1;
        total_length += current_length;
      }

      if (total_length > max_total_length) {
        max_total_length = total_length;
      }

      cout << string(s, i - current_length, current_length)
           << "  (level: " << depth << ", prev_depth: " << prev_depth
           << ", total_length: " << total_length << ")" << endl;

      prev_depth = depth;
      depth = 0;
      prev_current_length = current_length;
      current_length = 0;
    } else if (s[i] == '\t') {
      depth += 1;
    } else {
      current_length += 1;
    }
  }

  return max_total_length;
}

int main() {
  // string s =
  // "dir\n\tsubdir1\n\t\tfile1.ext\n\t\tsubsubdir1\n\tsubdir2\n\t\tsubsubdir2\n\t\t\tfile2.ext";
  string s = "dir\n\tsubdir1\n\tsubdir2\n\t\tfile.ext";
  max_total_length(s + "\n");
}
