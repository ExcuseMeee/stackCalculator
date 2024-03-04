#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

template<typename T>
class Stack {
public:
  Stack(unsigned capacity = 50) {
    this->arr = new T[capacity];
    this->capacity = capacity;
    this->top = -1;
  }

  ~Stack() {
    delete[] arr;
  }

  bool empty() const {
    return top == -1;
  }

  bool full() const {
    return top == capacity - 1; // max value of top is max index of array (capacity - 1)
  }

  const T& pop() {
    if (empty()) {
      throw std::string("[pop] Empty Stack");
    }
    return arr[top--]; // return element at index top, then decrement
  }

  const T& peek() const {
    if (empty()) {
      throw std::string("[peek] Empty Stack");
    }
    return arr[top];
  }

  void push(const T& value) {
    if (full()) {
      throw std::string("[push] Full Stack");
    }
    arr[++top] = value; // add value to array at index top + 1. recall that top is the index of top (existing) element
  }

private:
  T* arr; // pointer to array
  unsigned capacity; // max capacity of array
  int top; // index of top element. starts at -1 when no elements are in stack

};

double evaluatePostfix(std::string postfixArr[], int size);
bool isValidNum(char c);
bool isValidOperator(char c);
bool isValidBracket(char c);
double doArithmetic(char op, double firstPopped, double secondPopped);
std::string* infixToPostfix(std::string infix, int& theSize);
bool isHigherPriority(char current, char other);
void checkInvalidToken(std::string str); // if invalid token, throw it. whitespace ' ' should not be invalid
bool check_balance(const std::string& s);


int main() {

  try {

    // get infix
    std::string infix;
    std::cout << "Enter an infix expression: ";
    std::getline(std::cin, infix);

    // check for bad inputs
    checkInvalidToken(infix); // will automatically throw if any invalid tokens
    if (!check_balance(infix)) throw std::string("Unbalanced brackets");

    // convert infix to postfix
    int postFixSize = 0; // pass reference to track arr size
    std::string* postfix = infixToPostfix(infix, postFixSize);

    double res = evaluatePostfix(postfix, postFixSize); // will throw if invalid number of operands/operators

    // come here after all checks are passed
    // print postfix
    std::cout << "Postfix expression: ";
    for (int i = 0; i < postFixSize; i++) {
      std::cout << postfix[i] << " ";
    }
    std::cout << std::endl;

    // print result
    std::cout << std::fixed << std::setprecision(3) << "Result: " << res << std::endl;

  }
  catch (const std::string& e) {
    // functions throw string errors
    std::cout << "[Error] ";
    std::cout << e << std::endl;
  }
  catch (const std::exception& e) {
    // unhandled error?
    std::cout << "[Fatal Error] ";
    std::cerr << e.what() << '\n';
  }


  return 0;
}

double evaluatePostfix(std::string postfixArr[], int size) {
  // postfixArr should contain only valid strings, strings should have no whitespace
  Stack<double> operandStack;
  for (int i = 0; i < size; i++) {
    std::string str = postfixArr[i];
    char c = str[0]; // use the first char to check if num or operator
    if (isValidNum(c)) {
      operandStack.push(std::stod(str));
    }
    else if (isValidOperator(c)) {
      try {
        // pop will throw string error if popping from empty stack
        double firstPopped = operandStack.pop();
        double secondPopped = operandStack.pop();
        double result = doArithmetic(c, firstPopped, secondPopped);
        operandStack.push(result);
      }
      catch (const std::string& e) {
        // pop will fail when there isn't enough operands
        throw std::string("[evaluatePostfix] Not enough operands");
      }
      catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
      }

    }
    else {
      throw std::string("[evaluatePostfix] Invalid token: " + str);
    }
  }

  double result = operandStack.pop();
  // if numbers are still left in stack, this means there wasn't enough operators
  if (!operandStack.empty()) throw std::string("[evaluatePostfix] Not enough operators");

  return result;
}

double doArithmetic(char op, double firstPopped, double secondPopped) {
  // 8 6 / evaluates to 8/6... 6 is popped first and 8 is popped second
  switch (op)
  {
  case '+':
    return secondPopped + firstPopped;
  case '-':
    return secondPopped - firstPopped;
  case '*':
    return secondPopped * firstPopped;
  case '/':
    return secondPopped / firstPopped;
  case '^':
    return std::pow(secondPopped, firstPopped);
  case '%':
    return std::fmod(secondPopped, firstPopped);
  default:
    throw std::string("[doArithmetic] Invalid operation: ") + op;
  }
}

std::string* infixToPostfix(std::string infix, int& theSize) {

  if (infix.empty()) throw std::string("[infixToPostfix] No expression");

  Stack<char> operatorStack;

  std::string* buffer = new std::string[80]; // max 80 tokens, if equation is larger than 80 terms, too bad
  int actualSize = 0;

  int index = 0;
  while (index < infix.length()) {
    char firstChar = infix[index];

    if (isValidNum(firstChar)) {
      char curChar = infix[index];
      std::string numStr = "";

      while (isValidNum(curChar)) {
        numStr.push_back(curChar);
        index++;
        curChar = infix[index];
      }

      buffer[actualSize] = numStr; // add string to buffer
      actualSize++; // increment actual size

    }
    else if (isValidOperator(firstChar)) {

      if (operatorStack.empty()) {
        operatorStack.push(firstChar);
      }
      else {
        while (true) {
          char topOperator = operatorStack.peek();
          if (isHigherPriority(firstChar, topOperator) || isValidBracket(topOperator)) {
            operatorStack.push(firstChar);
            break;
          }
          else {
            buffer[actualSize] = topOperator;
            actualSize++;
            operatorStack.pop();

            if (operatorStack.empty()) {
              operatorStack.push(firstChar);
              break;
            }
          }
        }
      }
      index++; // increment index
    }
    else if (isValidBracket(firstChar)) {
      if (firstChar == '(' || firstChar == '{' || firstChar == '[') {
        operatorStack.push(firstChar);
      }
      else if (firstChar == ')' || firstChar == '}' || firstChar == ']') {
        char topOperator = operatorStack.peek();
        switch (firstChar)
        {
        case ')':
          while (topOperator != '(') {
            buffer[actualSize] = operatorStack.pop();
            actualSize++;
            topOperator = operatorStack.peek();
          }
          operatorStack.pop(); // pop the closing bracket
          break;
        case '}':
          while (topOperator != '{') {
            buffer[actualSize] = operatorStack.pop();
            actualSize++;
            topOperator = operatorStack.peek();
          }
          operatorStack.pop(); // pop the closing bracket
          break;
        case ']':
          while (topOperator != '[') {
            buffer[actualSize] = operatorStack.pop();
            actualSize++;
            topOperator = operatorStack.peek();
          }
          operatorStack.pop(); // pop the closing bracket
          break;
        default:
          break;
        }
      }
      index++; // increment index
    }
    else if (firstChar == ' ') {
      index++; // if whitespace, skip over
    }
    else {
      throw std::string("[infixToPostfix] Invalid Token: ") + firstChar;
    }

  }

  // at the end, pop any remaining operators
  while (!operatorStack.empty()) {
    char top = operatorStack.pop();
    buffer[actualSize] = top;
    actualSize++;
  }

  std::string* postfixArr = new std::string[actualSize];
  for (int i = 0; i < actualSize; i++) {
    postfixArr[i] = buffer[i];
  }
  theSize = actualSize; // update size to be used outside of function

  delete[] buffer;

  return postfixArr;
}

bool check_balance(const std::string& s) {
  Stack<char> st;
  for (char c : s) {
    if (c == '(' || c == '{' || c == '[') {
      // push into stack if opening delimiter
      st.push(c);
    }
    else if (c == ')' || c == ']' || c == '}') {
      char ch;
      // get the top element of stack, will return false if empty
      try {
        ch = st.pop();
      }
      catch (std::string ex) {
        return false;
      }

      // check for matching opening delimiter if c is closing delimiter
      switch (c)
      {
      case ')':
        if (ch == '(') continue;
        else return false;
      case '}':
        if (ch == '{') continue;
        else return false;
      case ']':
        if (ch == '[') continue;
        else return false;
      default:
        return false;
      }
    }
  }
  return st.empty();
}

bool isValidNum(char c) {
  try {
    return std::isdigit(c);
  }
  catch (const std::exception& e) {
    return false;
  }
}

bool isValidOperator(char c) {
  std::string ops = "+-*/^%";
  try {
    for (char o : ops) {
      if (c == o) return true;
    }
    return false;
  }
  catch (const std::exception& e) {
    return false;
  }

}

bool isValidBracket(char c) {
  std::string brackets = "()[]{}";
  try {
    for (char b : brackets) {
      if (c == b) return true;
    }
    return false;
  }
  catch (const std::exception& e) {
    return false;
  }

}

void checkInvalidToken(std::string str) {
  for (char c : str) {
    if (!(isValidNum(c) || isValidOperator(c) || isValidBracket(c) || c == ' ')) {
      throw std::string("Invalid token: ") + c;
    }
  }
}

bool isHigherPriority(char current, char other) {
  int currentRank, otherRank;
  std::string highToLow[] = { "^", "*/%", "+-" };

  // set ranking for current and other
  for (int i = 0; i < 3; i++) {
    std::string ops = highToLow[i];
    if (ops.find(current) != std::string::npos) {
      currentRank = i;
    }
    if (ops.find(other) != std::string::npos) {
      otherRank = i;
    }
  }

  return currentRank < otherRank; // lower rank means higher priority
}

