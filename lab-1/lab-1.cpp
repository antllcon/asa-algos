//На вход подаются сведения о сдаче экзаменов учениками 9-х
//классов некоторой средней школы.
//В **первой строке** сообщается количество учеников `N`, которое не
//меньше *10*, но не превосходит *100*, каждая из следующих N строк
//имеет следующий формат: `<Фамилия> <Имя> <оценки>`,
//`<Фамилия>` – строка, состоящая не более чем из 20 символов,
//`<Имя>` – строка, состоящая не более чем из 15 символов
//`<оценки>` – через пробел три целых числа, соответствующие
//оценкам по пятибалльной системе.
//`<Фамилия>` и `<Имя>`, а также `<Имя>` и `<оценки>` разделены
//одним пробелом.
//# Пример входной строки:
//### Иванов Петр 4 5 4
//Требуется написать программу, которая будет выводить на экран
//фамилии и имена **трех лучших по среднему баллу учеников**. Если
//среди остальных есть ученики, набравшие тот же средний балл,
//что и один из трех лучших, то следует вывести и их фамилии и
//имена. Требуемые имена и фамилии можно выводить в произвольном
//порядке (8 баллов)
//
// Глухарев Степан - ПС-21
// C++ (g++ comp.)

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
