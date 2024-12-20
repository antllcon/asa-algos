//
// Общими требованиями к лабораторной работе являются:
//
// 1) вводить   исходное   дерево  из  файла  в  понятной  для
//    пользователя форме, а не с клавиатуры;
// 2) по требованию пользователя показывать дерево на экране;
// 3) обрабатывать  дерево  в  оперативной памяти,  а не путем
//    многократного обращения к файлу;
// 4) обеспечить   возможность   многократных   запросов   без
//    повторного запуска программы.
//
// 20. В   некотором   институте   информация   об   имеющихся
// компьютерах  задана двумя деревьями.  В первом из них сыновьям
// корневой вершины соответствуют факультеты,  факультеты в  свою
// очередь  делятся  на  кафедры,  кафедры  могут  иметь  в своем
// составе лаборатории. Компьютеры могут быть установлены в общих
// факультетских   классах,   на   кафедрах,   в  лабораториях  и
// идентифицируются  уникальными  номерами.  Во   втором   дереве
// сыновьям корня соответствуют учебные корпуса, корпуса включают
// списки  аудиторий,  а  для  каждой  аудитории  заданы   номера
// находящихся  в  них  компьютеров.  Некоторые  аудитории  могут
// принадлежать  нескольким  факультетам.  Выдать  список   таких
// аудиторий (11).
//
// Глухарев Степан - ПС-21
// C++ (g++ comp.)
//





#include <fstream>
#include <iostream>
#include <map>
#include <vector>

std::string getWord(std::string &input, size_t &position) {
  while (position < input.size() && isspace(input[position])) {
    ++position;
  }
  const size_t start = position;
  while (position < input.size() && !isspace(input[position])) {
    ++position;
  }
  return input.substr(start, position - start);
}

int main() {

  // Структура дерева
  struct Tree {

    // Поля
    int value;                    // Вес узла
    std::string name;             // Имя узла
    Tree *parent;                 // Указатель на родителя
    std::vector<Tree *> children; // Прямо вектор потомков

    // Конструктор для удобной инициализации
    Tree(const int value, const std::string &nodeName,
         Tree *parentTree = nullptr)
        : value(value), name(nodeName), parent(parentTree) {}

    // Инициализация дерева
    static Tree *initTree(std::ifstream &inputFile) {

      // Инициализация данных
      Tree *root = nullptr;
      std::vector<Tree *> levelNodes(10, nullptr);
      std::string line;

      // Пропускаем пробелы перед записью
      while (std::getline(inputFile, line)) {
        if (line.length() > 1) {
          break;
        }
      }

      // Читаем файл
      while (!inputFile.eof()) {

        // Определяем уровень поля
        int level = 0;
        while (line[level] == '#') {
          ++level;
        }

        size_t position = level;

        // Нет # - корень дерева
        if (level == 0 && !root) {
          const std::string word = getWord(line, position);
          root = new Tree(0, word);
          levelNodes[level] = root;
        }

        // Есть # - другие ветки или листья
        if (level > 0) {
          const std::string word = getWord(line, position);
          Tree *newNode = new Tree(level, word);

          if (levelNodes[level - 1]) {
            levelNodes[level - 1]->children.push_back(newNode);
            newNode->parent = levelNodes[level - 1];
          }
          levelNodes[level] = newNode;

          // Сбрасываем узлы уровней выше
          for (size_t i = level + 1; i < levelNodes.size(); ++i) {
            levelNodes[i] = nullptr;
          }
        }

        // Выходим, если работали с пустой строкой
        if (inputFile.peek() == '\n') {
          break;
        }

        // Получаем следующую строку
        std::getline(inputFile, line);
      }
      return root;
    }

    // Ищет ПК (узел) по имени в дереве
    static Tree *findPC(Tree *node, const std::string &pcName) {
      if (!node) return nullptr;

      // Проверяем текущий узел
      if (node->name == pcName) {
        return node;
      }

      // Рекурсивно ищем в потомках
      for (Tree *child : node->children) {
        Tree* found = findPC(child, pcName);
        if (found) return found;
      }

      return nullptr; // ПК не найден
    }

    // Находит факультет, которому принадлежит ПК, поднимаясь по дереву
    static Tree* findFacultyForPC(const std::string& pcName, Tree* facultyTree) {
      // Находим узел с ПК в дереве факультетов
      Tree* pcNode = findPC(facultyTree, pcName);
      if (!pcNode) return nullptr;

      // Поднимаемся вверх по дереву, чтобы найти факультет
      Tree* current = pcNode;
      while (current && current->parent) {
        if (current->parent->parent == nullptr) {
          // Узел выше родительского уровня — это факультет
          return current;
        }
        current = current->parent;
      }

      return nullptr; // Факультет не найден
    }

    // Анализ деревьев на поиск кабинетов с указанием факультетов
    static void analyzeBuilding(Tree *buildingNode, Tree *facultyTree, std::ofstream &outputFile) {
      if (!buildingNode) return;

      // Рекурсивно обходим узлы корпуса
      for (Tree *room : buildingNode->children) {
        if (room->children.empty()) continue; // Пропускаем узлы без детей

        // Карта: факультет -> список компьютеров
        std::map<std::string, std::vector<std::string>> facultyToComputers;

        // Сбор компьютеров и их факультетов
        for (Tree *pc : room->children) {
          if (pc->children.empty()) { // Если это ПК
            Tree *facultyNode = findFacultyForPC(pc->name, facultyTree);
            std::string facultyName = facultyNode ? facultyNode->name : "Неизвестный факультет";
            facultyToComputers[facultyName].push_back(pc->name);
          }
        }

        // Вывод данных о кабинете
        if (!facultyToComputers.empty()) {
          if (facultyToComputers.size() > 1) {
            outputFile << buildingNode->name << " - " << room->name << " (";
            bool firstFaculty = true;

            for (const auto &[facultyName, computers] : facultyToComputers) {
              if (!firstFaculty) {
                outputFile << ", ";
              }
              outputFile << facultyName << ": ";
              for (size_t i = 0; i < computers.size(); ++i) {
                outputFile << computers[i];
                if (i < computers.size() - 1) {
                  outputFile << ", ";
                }
              }
              firstFaculty = false;
            }

            outputFile << ")" << std::endl;
          }
        }
      }

      // Рекурсивно обходим корпуса
      for (Tree *child : buildingNode->children) {
        if (!child->children.empty()) { // Если у узла есть дети, это корпус или кабинет
          analyzeBuilding(child, facultyTree, outputFile);
        }
      }
    }

    // Печать дерева
    static void printTree(const Tree* node, const int depth = 0) {
      if (!node) return;

      // Печатаем отступы и имя узла
      std::cout << std::string(depth * 2, ' ') << node->name << std::endl;

      // Рекурсивно печатаем детей
      for (const Tree* child : node->children) {
        printTree(child, depth + 1);
      }
    }
  };

  // Инициализация переменных
  std::ifstream inputFile;
  std::ofstream outputFile;

  std::string inFile, outFile;
  bool filesOpened = false;

  // Название программы
  std::cout << "Программа поиска аудиторий\n" << std::endl;

  // Работа с файлами
  do {
    std::cout << "Введите имена input и out файлов" << std::endl;
    std::cout << "Имя файла с данными (.txt): ";
    std::cin >> inFile;
    std::cout << "Имя файла с результатами (.txt): ";
    std::cin >> outFile;

    inputFile.open(inFile + ".txt");
    if (!inputFile.is_open()) {
      std::cerr << "Error: не удалось открыть файл " << inFile << std::endl;
    } else {
      outputFile.open(outFile + ".txt");
      if (!outputFile.is_open()) {
        std::cerr << "Error: не удалось открыть файл " << outFile << std::endl;
        inputFile.close();
      } else {
        filesOpened = true;
      }
    }
  } while (!filesOpened);


  // Инициализация деревьев
  Tree* facultyTree = Tree::initTree(inputFile);
  Tree* buildingTree = Tree::initTree(inputFile);

  // Полученные деревья
  Tree::printTree(facultyTree);
  std::cout << "\n\n" << std::endl;
  Tree::printTree(buildingTree);

  // Запуск сравнения деревьев на поиск соответствий
  Tree::analyzeBuilding(buildingTree, facultyTree, outputFile);

  inputFile.close();
  outputFile.close();
  return 0;
}
