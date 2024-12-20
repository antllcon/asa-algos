#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

const int INF = std::numeric_limits<int>::max();

struct Edge {
  std::string to;
  int length;
};

using Graph = std::unordered_map<std::string, std::vector<Edge>>;

void parseGraph(std::ifstream &inputFile, Graph &graph, std::string &capital) {
  std::string cityA, cityB;
  int length;
  bool isCapitalRead = false;

  while (inputFile >> cityA >> cityB >> length) {
    if (!isCapitalRead) {
      capital = cityA; // Первая строка определяет столицу
      isCapitalRead = true;
    }
    graph[cityA].push_back({cityB, length});
    graph[cityB]; // Убедимся, что добавили город, даже если он только
                  // "получатель"
  }
}

void findSecondShortestPaths(
    const Graph &graph, const std::string &capital,
    std::unordered_map<std::string, int> &secondShortest) {
  std::unordered_map<std::string, int> dist;
  std::unordered_map<std::string, int> secondDist;

  for (const auto &[city, _] : graph) {
    dist[city] = INF;
    secondDist[city] = INF;
  }

  dist[capital] = 0;

  using State = std::pair<int, std::string>; // (дистанция, город)
  std::priority_queue<State, std::vector<State>, std::greater<>> pq;
  pq.push({0, capital});

  while (!pq.empty()) {
    auto [currentLength, currentCity] = pq.top();
    pq.pop();

    for (const auto &edge : graph.at(currentCity)) {
      int newLength = currentLength + edge.length;

      // Обновление минимального пути
      if (newLength < dist[edge.to]) {
        secondDist[edge.to] =
            dist[edge.to]; // Сохраняем старый минимальный путь
        dist[edge.to] = newLength; // Обновляем минимальный путь
        pq.push({newLength, edge.to});
      }
      // Обновление второго минимального пути
      else if (newLength > dist[edge.to] && newLength < secondDist[edge.to]) {
        secondDist[edge.to] = newLength; // Обновляем второй минимальный путь
        pq.push({newLength, edge.to});
      }
    }
  }

  secondShortest = secondDist;
}

void writeResults(const std::string &outFile,
                  const std::unordered_map<std::string, int> &secondShortest,
                  const std::string &capital) {
  std::ofstream outputFile(outFile);
  if (!outputFile.is_open()) {
    std::cerr << "Error: не удалось открыть файл " << outFile << std::endl;
    return;
  }

  for (const auto &[city, length] : secondShortest) {
    if (city == capital)
      continue;
    if (length == INF) {
      outputFile << city << ": Нет второго пути\n";
    } else {
      outputFile << city << ": " << length << "\n";
    }
  }
  outputFile.close();
}

int main() {
  std::string inFile, outFile;
  std::ifstream inputFile;
  std::ofstream outputFile;
  bool filesOpened = false;

  do {
    std::cout << "Введите имена input и output файлов\n";
    std::cout << "Имя файла с данными (.txt): ";
    std::cin >> inFile;
    std::cout << "Имя файла с результатами (.txt): ";
    std::cin >> outFile;

    inputFile.open(inFile + ".txt");
    if (!inputFile.is_open()) {
      std::cerr << "Error: не удалось открыть файл " << inFile << "\n";
    } else {
      outputFile.open(outFile + ".txt");
      if (!outputFile.is_open()) {
        std::cerr << "Error: не удалось открыть файл " << outFile << "\n";
        inputFile.close();
      } else {
        filesOpened = true;
      }
    }
  } while (!filesOpened);

  Graph graph;
  std::string capital;

  // Парсинг графа и столицы
  parseGraph(inputFile, graph, capital);
  inputFile.close();

  // Поиск вторых минимальных путей
  std::unordered_map<std::string, int> secondShortest;
  findSecondShortestPaths(graph, capital, secondShortest);

  // Запись результатов
  writeResults(outFile + ".txt", secondShortest, capital);

  std::cout << "Результаты записаны в файл " << outFile << ".txt\n";
  return 0;
}
