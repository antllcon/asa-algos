
# include <iostream>
# include <fstream>
# include <string>

int main() {
  std::ifstream inputFile;
  std::ofstream outputFile;
  char inFile[256];
  char outFile[256];
  bool filesOpened = false;

  int numSpies = 0;
  int globalTime = 0;
  int lastTakeTime = 0;
  bool statusEvent = false;

  std::string takeGlass = " взял бинокль ";
  std::string leaveSpy = " вышел из очереди ";
  std::string transferGlass =  " передал бинокль ";
  std::string event = "";

  struct Spy {
    std::string name;
    int viewTime;
    int queueTime;
    bool hasGlass;
  };

  // Проверка наличия файлов
  do {
    std::cout << "========================" << std::endl;
    std::cout << "Enter input name (.txt): ";
    std::cin >> inFile;
    std::cout << "Enter output name (.txt): ";
    std::cin >> outFile;

    inputFile.open(inFile);
    if (!inputFile.is_open()) { // не удалось открыть файл input
      std::cerr << "Error: open intput: " << inFile << std::endl;

    } else {
      outputFile.open(outFile);
      if (!outputFile.is_open()) { // не удалось открыть файл output
        std::cerr << "Error: open output: " << outFile << std::endl;
      } else {
        filesOpened = true;
      }
    }
  } while (!filesOpened);

  std::cout << "Successfully opened input files" << std::endl;
  std::cout << "========================" << std::endl;

  // проходимся по файлу и узнаем сколько шпионов всего
  std::string line;
  while (std::getline(inputFile, line)) {
      numSpies++;
  }

  // перемещаем курсор в начало файла
  inputFile.seekg(0, std::ios::beg);
  outputFile << "Number of spies: " << numSpies << std::endl;

  // Выделяем память для структуры
  Spy* spies = new Spy[numSpies];

  for (int i = 0; i <= numSpies; i++) {
    inputFile >> spies[i].name >> spies[i].viewTime >> spies[i].queueTime;
    spies[i].hasGlass = false;
    // выдаем первому бинокль
    if (i == 0) {
      spies[i].hasGlass = true;
    }
  }

  // пока есть шпионы:
  // обработать новый тик (
  // 1. вы
  // )

while ()

  for (int i = 0; numSpies > 1; i++) {
    event = "";
    if (spies[i].hasGlass) {
      lastTakeTime += 1;
      if (spies[i].viewTime - lastTakeTime == 0) {
        lastTakeTime = 0;
        spies[i].hasGlass = false;
        spies[i + 1].hasGlass = true;
		statusEvent = true;
        event = event + spies[i].name + transferGlass + spies[i + 1].name + takeGlass;
      }
    }

    if (spies[i].queueTime - globalTime == 0) {
      statusEvent = true;
      event = event + spies[i].name + leaveSpy;
      // удаляем элемент массива
    }

    if (statusEvent) {
      outputFile << globalTime << ": "<< event << std::endl;
    }

    globalTime += 1;
    statusEvent = false;
  }



  return 0;
}