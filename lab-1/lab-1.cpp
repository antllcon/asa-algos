
#include <iostream>
#include <fstream>
#include <cmath>

int main() {
  const int MAX_STUDENTS = 100;
  const int MAX_SURNAME_LEN = 21;  // 20 символов + 1 для '\0'
  const int MAX_NAME_LEN = 16;     // 15 символов + 1 для '\0'
  const int TOP_LIST = 3;

  int n;
  char surnames[MAX_STUDENTS][MAX_SURNAME_LEN];
  char names[MAX_STUDENTS][MAX_NAME_LEN];
  int grades[MAX_STUDENTS][3];
  double averages[MAX_STUDENTS];

  char inFile[256];
  char outFile[256];

  std::cout << "Enter input name (.txt): ";
  std::cin >> inFile;
  std::cout << "Enter output name (.txt): ";
  std::cin >> outFile;

  // Открытие входного файла
  std::ifstream inputFile(inFile, std::ios::in);
  if (!inputFile.is_open()) {
    std::cerr << "Error: open intput: " << inFile << std::endl;
    return 1;
  }

  // Открытие выходного файла
  std::ofstream outputFile(outFile, std::ios::out);
  if (!outputFile.is_open()) {
    std::cerr << "Error: open output: " << outFile << std::endl;
    return 1;
  }

  inputFile >> n;
  if (n < 10 || n >= 100) {
    std::cerr << "Error: students or file incorrect" << std::endl;
    return 1;
  }

  for (int i = 0; i < n; ++i) {
    inputFile >> surnames[i] >> names[i] >> grades[i][0] >> grades[i][1] >> grades[i][2];
    averages[i] = round((grades[i][0] + grades[i][1] + grades[i][2]) / 3.0 * 10) / 10.0;
//    std::cout << surnames[i] << " " << names[i] << " " << averages[i] << std::endl;
  }

  inputFile.close();

  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - i - 1; ++j) {
      if (averages[j] < averages[j + 1]) {
        std::swap(averages[j], averages[j + 1]);
        std::swap(surnames[j], surnames[j + 1]);
        std::swap(names[j], names[j + 1]);
        std::swap(grades[j], grades[j + 1]);
      }
    }
  }

  double tempAverage = averages[TOP_LIST - 1];
  int count = 0;

  for (int i = 0; i < n && (count < TOP_LIST || averages[i] == tempAverage); ++i) {
    outputFile << surnames[i] << " " << names[i] << " - " << averages[i] << std::endl;
    ++count;
  }

  outputFile.close();
  std::cout << "Sucessful!" << std::endl;
  return 0;
}
