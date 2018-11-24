#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;

const char DEFAULT_NUMS[10] = {'9', '8', '7', '6', '5', '4', '3', '2', '1', '0'};

class NumberHolder {
 public:

  NumberHolder() = default;
  ~NumberHolder() = default;

  explicit NumberHolder(char N) : my_letter_(N) {
  };

  // only called when increase number returned to 0 // return false;
  void setIgnoredNumber(char ignored_number) {

    if (my_letter_ == 0) return; // not used

    bool changed = false;

    for (int i = 0; i < end_pos_; i++) {

      if (nums_[i] == ignored_number) {
        nums_[i] = nums_[end_pos_--];
        //end_pos_--;
        break;
      }

    }
    // remove one, re init char to pos
  }

  void reset() {

    cur_pos_ = 0;
    end_pos_ = 9;
    std::memcpy(nums_, DEFAULT_NUMS, sizeof(nums_));

    if (ignore_zero_) cleanZero();

  }

  // true if success -- not end
  // false if increase failed -- end
  bool increaseNumber() {

    if (cur_pos_ == end_pos_) {
      ended_ = true;
      reset();
      return false;
    } else {
      cur_pos_++;
      return true;
    }

  }

  void replaceLetterToNum(char *input) {

    *input = nums_[cur_pos_];

  }

  void setIgnoreZero() {
    ignore_zero_ = true;

    cleanZero();
  }

  void cleanZero() {
    if (nums_[end_pos_] == '0') {

      nums_[end_pos_--] = 0;
    }

  }

  bool operator==(const NumberHolder &rhs) {

    return (my_letter_ == rhs.my_letter_);
  }

  NumberHolder &operator=(char c) {

    my_letter_ = c;
    return *this;
  };

  char number() { return nums_[cur_pos_]; }
  char letter() { return my_letter_; }

  bool ended() { return ended_; }
 private:

  bool ended_ = false;
  char my_letter_ = 0; // 0 indicate not used.

  bool ignore_zero_ = false;
  int end_pos_ = 9; // staticlly point to end
  int cur_pos_ = 0; // current number position
  char nums_[10] = {'9', '8', '7', '6', '5', '4', '3', '2', '1', '0'};

};

class LettersContainer {

 public:

  LettersContainer() = default;
  ~LettersContainer() = default;

  LettersContainer(string s) {

  }

  NumberHolder &operator[](char c) {

    int i = c - 'A';

    if (i < 0 || i > MAX_LETTERS)
      return holder[0];
    else {
      return holder[i];
    }
  }

  void narrowInternalIndex() {

    for (int i = 0; i < MAX_LETTERS; i++) {

      if (holder[i].letter() != 0) {
        the_first_char_index_ = holder[i].letter();
        break;
      };
    }

    for (int i = MAX_LETTERS - 1; i >= 0; i--) {

      if (holder[i].letter() != 0) {
        the_last_char_index_ = holder[i].letter();
        break;
      };
    }
  }

  int begin() {
    return the_first_char_index_;
  }

  int end() {
    return the_last_char_index_;
  }

  static const int MAX_LETTERS = 26;

 private:

  //  0   1
  // 'A' 'B'
  NumberHolder holder[MAX_LETTERS];
  int the_first_char_index_ = 0;
  int the_last_char_index_ = MAX_LETTERS;
};

class GenerateNumber {

 public:
  GenerateNumber(const string &s1, const string &s2, const string &s3) : s1_(s1), s2_(s2), s3_(s3) {

    addStringToLetterTable(s1);
    addStringToLetterTable(s2);
    addStringToLetterTable(s3);

    letters1_.narrowInternalIndex();
    // stable not changing.
    SetIgnoredNumbers();
    //
  }

  void SetIgnoredNumbers(int num_of_changed = LettersContainer::MAX_LETTERS) {

    for (auto index = letters1_.begin(); index <= letters1_.end(); ++index) {

      // it - > second is NumberHolder
      int count = 0;
      for (auto index1 = letters1_.end();
           count < num_of_changed && letters1_[index1].letter() != letters1_[index].letter(); --index1, ++count) {

        letters1_[index1].setIgnoredNumber(letters1_[index].number());
      }

    }
  }

  void getNewNumber(int *n1, int *n2, int *n3) {

    // every time gen one number
    string t_s1 = s1_;
    string t_s2 = s2_;
    string t_s3 = s3_;

    try {

      for (int i = 0; i < t_s1.size(); i++) {
        letters1_[s1_[i]].replaceLetterToNum(&t_s1[i]);
      }

      for (int i = 0; i < t_s2.size(); i++) {
        letters1_[s2_[i]].replaceLetterToNum(&t_s2[i]);
      }
      for (int i = 0; i < t_s3.size(); i++) {
        letters1_[s3_[i]].replaceLetterToNum(&t_s3[i]);
      }

      *n1 = stoi(t_s1);
      *n2 = stoi(t_s2);
      *n3 = stoi(t_s3);

    } catch (exception &e) {
      *n1 = 0;
      *n2 = 0;
      *n3 = 1;  // 0 * 0 != 1
    }

  }

  // increase from end to begin
  void increaseNumbers() {

    int changed_count = 0;

    for (auto i = letters1_.end(); i >= 0; --i) {

      if (letters1_[i].letter() != 0) {
        // 0 is init value, not initialized.
        changed_count++;
        if (letters1_[i].increaseNumber()) {

          break;
          //this one have already
        }

      }

    }

    SetIgnoredNumbers(changed_count);

  }

  void addStringToLetterTable(const std::string &monogram) {

    for (auto letter : monogram) {

      letters1_[letter] = letter;
    }

    letters1_[monogram[0]].setIgnoreZero();

  }

  bool endOfGen() {
    return letters1_[letters1_.begin()].ended();
  }

 private:

  string s1_;
  string s2_;
  string s3_;

  LettersContainer letters1_;

};

void FinishMyPuzzle(const string &s1, const string &s2, const string &s3, stringstream &out) {
  GenerateNumber gen(s1, s2, s3);

  int n1 = 0;
  int n2 = 0;
  int n3 = 0;
  while (!gen.endOfGen()) {
    gen.getNewNumber(&n1, &n2, &n3);

    if (n1 * n2 == n3) {

      out << n1 << " * " << n2 << " = " << n3 << endl;

    }
    gen.increaseNumbers();
  }
}

int main(int argc, char **argv) {

  std::ifstream ft(argv[1]);
  std::stringstream buff;
  buff << ft.rdbuf();

  string symbol1;
  string symbol2;
  string s1;
  string s2;
  string s3;

  stringstream outstream;
  while (true) {

    buff >> s1 >> symbol1 >> s2 >> symbol2 >> s3;
    if (buff.fail()) break;

    // can use s1 s2 s3 safely.
    FinishMyPuzzle(s1, s2, s3, outstream);

  }

  cout << outstream.str();

  return 0;
}
