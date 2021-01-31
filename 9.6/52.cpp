#include <conio.h>

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

stack<string> savestr;  // 字符串优先级堆栈
stack<double> number;   // 中间值堆栈

static const string punct = "0123456789+-*/().";
static const string puct1 = "+-*/.";
static const string puct2 = "0123456789";

void bracketlogic(string &);       // 括号优先级判断
double calulation(void);           // 计算
void cheek(const string &in);      // 检查
inline void basicset(string &in);  // 字符串规格化
inline void menu(void);            // 信息
bool cmp(string &in);              // 判断负号和减号

int main(void) {
  system("color 0A");
  menu();
  cout << "\n=========== 输入无间隔表达式,只支持无符号数,负数用(0-x)代替 "
          "(EOF退出) ==========="
       << endl;
  string input;
  char ch;
  double result;
  while ((getline(cin, input))) {
    try {
      basicset(input);
      cheek(input);
      cout << "---------括号优先级判断过程---------" << endl;
      bracketlogic(input);
      result = calulation();
      cout << "\n>>>>>>[计算结果] :  " << result << endl;
    } catch (const runtime_error &e) {
      cout << "\n>>>>>>>>>>>>> 错误信息 <<<<<<<<<<<<<" << endl;
      std::cerr << ' ' << e.what() << endl;
      cout << "\n\n**********键入Y-继续**********" << endl;
      ch = toupper(getch());
      switch (ch) {
        case 'Y':
          cout << "\n=========== "
                  "输入无间隔表达式,只支持无符号数,负数用(0-x)代替 (EOF退出) "
                  "==========="
               << endl;
          continue;
          break;
        default:
          system("pause");
          return 0;
          break;
      }
    }
    cout << "\n=========== 输入无间隔表达式,只支持无符号数,负数用(0-x)代替 "
            "(EOF退出) ==========="
         << endl;
  }
  system("pause");
  return 0;
}

void bracketlogic(string &in) {
  string input;
  string::size_type R = 0;
  string::size_type L = 0;
  string::size_type count;
  for (count = 0; count < in.size(); ++count) {
    if (in[count] == '(') ++L;
    if (in[count] == ')') ++R;
    if (L == R && L != 0) {
      R = 0;
      while (L != 0) {
        R = in.find_first_of(')', R);
        ++R;
        --L;
      }
      input =
          in.substr(in.find_first_of('(') + 1, --R - in.find_first_of('(') - 1);
      in.insert(in.find_first_of('('), "A");
      in.erase(in.find_first_of('('), R - in.find_first_of('(') + 2);
      bracketlogic(input);
      string::size_type lpos = input.find_first_of('(');
      string::size_type rpos = input.find_first_of(')');
      if (lpos <= rpos && !input.empty()) {
        cout << " 字符串入栈:  " << input << endl;
        savestr.push(input);
      }

      L = R = 0;
      count = 0;
    }
    input.clear();
  }
}
void cheek(const string &in) {
  unsigned int L = 0;
  unsigned int R = 1;
  bool decimal = false;
  cout << "\n-------------字符串检查-------------" << endl;
  if (in.find_first_not_of(punct) != string::npos)
    throw runtime_error("(输入错误 : 含有其他字符)");
  for (decltype(in.size()) i = 0; i < in.size() - 1; ++i) {
    if (puct1.find_first_of(in[i]) != string::npos) {
      if (in[i] != '.') decimal = false;
      if (puct1.find_first_of(in[i + 1]) != string::npos || in[i + 1] == ')')
        throw runtime_error("(输入错误 : 运算符逻辑错误)");
    }

    if (in[i] == '(')
      if (in[i + 1] == ')' || (puct1.find_first_of(in[i + 1]) != string::npos))
        throw runtime_error("(输入错误 : 运算符逻辑错误)");

    if (in[i] == ')')
      if (in[i + 1] == '(' || (puct2.find_first_of(in[i + 1]) != string::npos))
        throw runtime_error("(输入错误 : 运算符逻辑错误)");
    if (puct2.find_first_of(in[i]) != string::npos) {
      if (in[i + 1] == '(') throw runtime_error("(输入错误 : 运算符逻辑错误)");
    }
    if (in[i] == '(') ++L;
    if (in[i] == ')') ++R;
    if (in[i] == '.' && !decimal) {
      decimal = true;
      continue;
    }
    if (in[i] == '.' && decimal)
      throw runtime_error("(输入错误 : 浮点逻辑错误)");
  }

  if (R != L) throw runtime_error("(输入错误 : 括号逻辑错误)");
  cout << " -正常-" << endl;
}
double calulation(void) {
  cout << "--------------计算过程--------------" << endl;
  vector<string> exchange;
  string save;
  string num;
  string::size_type priority = 0;
  string::size_type move, del;
  double before;
  double rear;
  while (!savestr.empty()) {
    exchange.push_back(savestr.top());
    savestr.pop();
  }
  for (const auto &c : exchange) savestr.push(c);

  while (!savestr.empty()) {
    save = savestr.top();
    savestr.pop();
    if (save.find_first_of('A') == string::npos) {
      if (save.find_first_of("*/", priority) != string::npos && cmp(save)) {
        cout << " 不同优先级转换: " << save;
        while ((priority = save.find_first_of("*/", priority)) !=
               string::npos) {
          move = priority - 1;
          del = priority + 1;
          while ((isdigit(save[move]) || save[move] == '.') && move != 0)
            --move;
          if (move != 0) ++move;
          while ((isdigit(save[del]) || save[del] == '.') && del != save.size())
            ++del;
          num = save.substr(move, priority - move);
          before = stod(num);
          num = save.substr(priority + 1, del - priority);
          rear = stod(num);
          switch (save[priority]) {
            case '*':
              save.replace(move, del - move, to_string(before * rear));
              break;
            case '/':
              if (rear == 0) throw runtime_error("(计算错误 : 除数中含有0)");
              save.replace(move, del - move, to_string(before / rear));
              break;
          }
          ++priority;
        }
        cout << " = " << save << endl;
        savestr.push(save);
        save.clear();
        priority = 0;
      } else {
        cout << " 同优先级中间值: " << save;
        move = 0;
        before = stod(save, &move);
        save.erase(0, move);
        while (!save.empty()) {
          switch (save[0]) {
            case '+':
              save.erase(0, 1);
              before += stod(save, &move);
              break;
            case '-':
              save.erase(0, 1);
              before -= stod(save, &move);
              break;
            case '*':
              save.erase(0, 1);
              before *= stod(save, &move);
              break;
            case '/':
              save.erase(0, 1);
              if (stod(save) == 0)
                throw runtime_error("(计算错误 : 除数中含有0)");
              before /= stod(save, &move);
              break;
          }
          save.erase(0, move);
        }
        cout << " = " << before << " 入栈" << endl;
        number.push(before);
      }
    } else {
      while ((priority = save.find_last_of('A')) != string::npos) {
        save.replace(priority, 1, to_string(number.top()));
        number.pop();
      }
      savestr.push(save);
      priority = 0;
    }
  }
  rear = number.top();
  number.pop();
  return rear;
}

inline void basicset(string &in) {
  in.insert(in.begin(), '(');
  in.append(")");
  return;
}
inline void menu(void) {
  cout << "                         *===========================*" << endl;
  cout << "                         |                           |" << endl;
  cout << "                         |      - 优先级计算器 -     |" << endl;
  cout << "                         |      - 版本 : V1.1  -     |" << endl;
  cout << "                         |                           |" << endl;
  cout << "                         *===========================*" << endl;
  return;
}

bool cmp(string &in) {
  string::size_type i;
  if (in.find_first_of('+') != string::npos) return true;
  for (i = 0; i < in.size() - 1; ++i) {
    if (in[i] == '-' && i != 0) {
      if (puct2.find_first_of(in[i - 1]) != string::npos) return true;
    }
  }
  return false;
}