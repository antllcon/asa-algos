//
// Общими требованиями к лабораторной работе являются:
//
// 1) вводить  граф из файла в понятной для пользователя форме
// (не в виде матрицы смежности, без дублирования информации и т.
// п.);
// 2) обеспечить   возможность   многократных   запросов   без
//  повторного запуска программы.
//
// 26. Имеется сеть автомобильных дорог. По  некоторым дорогам
// можно  проехать  только  в одном  направлении. Известна  длина
// каждой дороги, причем она может быть разной в  зависимости  от
// направления. Один  из  городов  является  столицей.  Требуется
// вывести список длин вторых по минимальности путей из столицы в
// другие города. Допускается присутствие циклических путей (12)
//
// Глухарев Степан - ПС-21
// C++ (g++ comp.)
//
//

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// Типы данных
struct Edge {
  std::string neighbor;
  int weight;
};

struct PathInfo {
  int first;
  int second;
};

using Graph = std::unordered_map<std::string, std::vector<Edge>>;

// Функции
void clearTerminal() {
#ifdef _WIN32
  std::system("cls");
#else
  std::system("clear");
#endif
}

void printProgramName(const std::string &programName) {
  const std::string line =
      "════════════════════════════════════════════════════════════════════";
  const std::string text = line + "\n" + programName + "\n" + line;
  std::cout << text << std::endl;
}

std::ifstream openInputFile() {

  std::string fileName;
  std::ifstream file;

  do {
    std::cout << "\nВведите имя файла графа маршрутов: ";
    std::cin >> fileName;

    fileName += ".txt";
    file.open(fileName);

    if (!file.is_open()) {
      std::string errorText =
          "Ошибка: Не удалось открыть файл [ " + fileName + " ]";
      std::cerr << errorText << std::endl;
    }
  } while (!file.is_open());

  std::cout << "Файл [ " << fileName << " ] - успешно открыт" << std::endl;
  return file;
}

std::ofstream openOutputFile() {

  std::string fileName;
  std::ofstream file;

  do {
    std::cout << "\nВведите имя файла вывода: ";
    std::cin >> fileName;

    fileName += ".txt";
    file.open(fileName);

    if (!file.is_open()) {
      std::string errorText =
          "Ошибка: Не удалось открыть файл [ " + fileName + " ]";
      std::cerr << errorText << std::endl;
    }

    std::cout << "Файл [ " << fileName << " ] - успешно открыт" << std::endl;
  } while (!file.is_open());

  return file;
}

void readGraphFromFile(
    std::ifstream &file,
    std::unordered_map<std::string, std::vector<Edge>> &graph,
    std::string &capital) {

  std::string from;
  std::string to;
  int weight;

  file >> capital;

  while (file >> from >> to >> weight) {
    graph[from].push_back({to, weight});
    if (graph.find(to) == graph.end()) {
      graph[to] = {};
    }
  }
}

std::unordered_map<std::string, PathInfo>
findSecondShortestPaths(const Graph &graph, const std::string &capital) {

  // Хранилище для расстояний до каждой вершины
  std::unordered_map<std::string, PathInfo> distances;

  // Инициализация расстояний
  for (const auto &node : graph) {
    distances[node.first] = {std::numeric_limits<int>::max(),
                             std::numeric_limits<int>::max()};
  }

  distances[capital].first = 0;

  // Очередь с приоритетом: {текущая длина, город, признак "первый или второй
  // путь"}
  using QueueNode = std::tuple<int, std::string, bool>;
  std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<>> queue;

  queue.push({0, capital, true});

  while (!queue.empty()) {
    auto [currentDist, currentCity, isFirst] = queue.top();
    queue.pop();
    //
    // // Пропускаем устаревшие записи
    // if ((isFirst && currentDist > distances[currentCity].first) ||
    //     (!isFirst && currentDist > distances[currentCity].second)) {
    //   continue;
    // }
    //
    // // Если у текущего узла нет соседей, пропускаем
    // if (graph.find(currentCity) == graph.end()) {
    //   continue;
    // }

    // Обработка соседей
    for (const auto &edge : graph.at(currentCity)) {
      int newDist = currentDist + edge.weight;

      // Обновление первого пути
      if (newDist < distances[edge.neighbor].first) {
        distances[edge.neighbor].second = distances[edge.neighbor].first;
        distances[edge.neighbor].first = newDist;
        queue.push({newDist, edge.neighbor, true});

        // Обновление второго пути
      } else if (newDist >= distances[edge.neighbor].first &&
                 newDist < distances[edge.neighbor].second) {
        distances[edge.neighbor].second = newDist;
        queue.push({newDist, edge.neighbor, false});
      }
    }
  }

  return distances;
}

void writeResultsToFile(
    std::ofstream &file,
    const std::unordered_map<std::string, PathInfo> &distances,
    const std::string &capital) {
  file << "Вторые кратчайшие пути из столицы " << capital << ":\n";

  // Собираем результаты в вектор
  std::vector<std::pair<std::string, PathInfo>> sortedDistances;
  for (const auto &entry : distances) {
    if (entry.first != capital) {
      sortedDistances.emplace_back(entry);
    }
  }

  // Сортирую по числу и по строке, в зависимости от данных
  std::sort(sortedDistances.begin(), sortedDistances.end(),
            [](const auto &a, const auto &b) {
              try {
                int numA = std::stoi(a.first);
                int numB = std::stoi(b.first);
                return numA < numB; // Сравнение как чисел
              } catch (...) {
                return a.first < b.first; // Если не число, сравнение как строки
              }
            });

  // Записываем результаты в файл
  for (const auto &entry : sortedDistances) {
    file << capital << " --> " << entry.first;
    if (entry.second.second == std::numeric_limits<int>::max()) {
      file << ": Нет второго пути\n";
    } else {
      file << " = " << entry.second.second << "\n";
    }
  }
}

// программа
int main() {

  // Объявление констант
  const std::string programName =
      "Программа поиска ВТОРОГО кратчайшего пути из столицы в другие города \n"
      "(реализация через модификацию алгоритма Дейкстры)\n \n"
      "Программа работает с двумя файлами:\n"
      " ▪ Первый файл - первой строкой должен содержать столицу, а после в\n"
      "   каждой новой строке могут быть перечислены ребра графа в формате\n"
      "   [Город_откуда Город_куда 32]\n"
      " ▪ Второй файл - необходим для получения результатов программы";

  // Приветствуем пользователя
  clearTerminal();
  printProgramName(programName);

  // Объявляем файлы для работы
  std::ifstream inputFile = openInputFile();
  std::ofstream outputFile = openOutputFile();

  // Хранилище графа - матрица смежности
  Graph graph;

  // Название столицы
  std::string capital;

  // Читаем из файла граф с определением столицы
  readGraphFromFile(inputFile, graph, capital);

  // Получаем все пути
  // Поиск вторых минимальных путей
  auto distances = findSecondShortestPaths(graph, capital);

  // Запись результатов
  writeResultsToFile(outputFile, distances, capital);

  std::cout << "Результаты записаны в файл." << std::endl;

  return 0;
}
