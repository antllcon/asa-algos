// Задание
// У множества шпионов, собравшихся вместе для наблюдения секретного объекта,
// имеется единственный бинокль. Сформировалась очередь на этот бинокль. Для
// каждого шпиона задан период наблюдения в минутах и предельное время
// нахождения в очереди. После наблюдения каждый шпион становится снова в конец
// очереди. Как только для какого-либо шпиона истекает предельное время
// нахождения в очереди, он покидает ее (даже если в этот момент владеет
// биноклем) и отправляется к резиденту. Вывести протокол наблюдения шпионов за
// объектом.
//
// Глухарев Степан - ПС-21
// C++ (g++ comp.)

#include <fstream>
#include <iostream>
#include <string>

constexpr int DELTA_TIME = 1;

// Структура шпиона
struct Spy {
  int id;           // Идентификатор
  std::string name; // Имя шпиона
  int viewTime;     // Время наблюдения с биноклем
  int limitTime; // Предельное время нахождения в очереди
  mutable int watchTime; // Текущее время наблюдения
  Spy *next; // Указатель на следующего шпиона
  Spy *prev; // Указатель на предыдущего

  Spy(const int id, const std::string &name, const int view_time,
      const int limit_time)
      : id(id), name(name), viewTime(view_time), limitTime(limit_time),
        watchTime(0), next(nullptr), prev(nullptr) {}
};

// Структура сторожа
struct CircularQueue {
  Spy *sentinel;

  CircularQueue() {
    sentinel = new Spy(0, "sentinel", 0, 0);
    sentinel->next = sentinel;
    sentinel->prev = sentinel;
  }

  // Добавление шпиона в конец очереди
  void addSpy(Spy *spy) const {
    spy->next = sentinel;
    spy->prev = sentinel->prev;
    sentinel->prev->next = spy;
    sentinel->prev = spy;
  }

  // Удаление шпиона из очереди
  static void removeSpy(const Spy *spy) {
    spy->prev->next = spy->next;
    spy->next->prev = spy->prev;
    delete spy;
  }

  // Перемещение шпиона в конец очереди без удаления из памяти
  void moveToBack(Spy *spy) const {
    // Удаляем шпиона из текущего места в очереди
    spy->prev->next = spy->next;
    spy->next->prev = spy->prev;

    // Добавляем шпиона в конец очереди
    addSpy(spy);
  }

  // Проверка, пуста ли очередь (кроме сторожа)
  bool isEmpty() const { return sentinel->next == sentinel; }

  // Получение первого шпиона в очереди
  Spy *front() const { return isEmpty() ? nullptr : sentinel->next; }
};

// Инициализация очереди шпионов
void initSpy(CircularQueue &queueSpy, std::ifstream &inputFile) {
  int id, viewTime = 0, limitTime = 0;
  std::string name;

  while (inputFile >> id >> name >> viewTime >> limitTime) {
    queueSpy.addSpy(new Spy(id, name, viewTime, limitTime));
  }
}

// Симуляция наблюдения
void simulateObservation(CircularQueue &queueSpy, std::ofstream &outputFile) {
  int globalTime = 0;
  while (!queueSpy.isEmpty()) {
    std::string event;
    Spy *current = queueSpy.front();
    do {

      // выход из очереди
      if (globalTime >= current->limitTime) {
        // Удаляем шпиона, если его предельное время истекло
        if (event != "") {
          event = event + ", ";
        }
        event = event + current->name + " ушел к резиденту";
        if (current == queueSpy.sentinel->next) {
          event = event + " и выкинул бинокль";
        }

        const Spy *toDelete = current;
        current = current->next;
        queueSpy.removeSpy(toDelete);
        continue;
      }

      // Обработка шпиона, владеющего биноклем
      if (current == queueSpy.front()) {
        if (current->watchTime == 0) {
          if (event != "") {
            event = event + ", ";
          }
          event += current->name + " взял бинокль и начал наблюдать";
        }

        current->watchTime += DELTA_TIME;

        if (current->viewTime < current->watchTime &&
            current->next != queueSpy.sentinel) {
          if (event != "") {
            event = event + ", ";
          }
          event = event + current->name + " оставил бинокль и ушел в конец очереди";
          current->watchTime = 0;
          Spy *finishedSpy = current;
          current = current->next;
          queueSpy.moveToBack(finishedSpy);
          continue;
        }
      }

      current = current->next;
    } while (current != queueSpy.sentinel);

    if (event != "") {
      outputFile << globalTime << ": " << event << std::endl;
    }
    globalTime += DELTA_TIME;
  }
}

int main() {
  std::ifstream inputFile;
  std::ofstream outputFile;

  std::string inFile, outFile;
  bool filesOpened = false;

  CircularQueue queueSpy;

  // Работа с файлами
  do {
    std::cout << "Введите имя input (.txt): ";
    std::cin >> inFile;
    std::cout << "Введите имя out (.txt): ";
    std::cin >> outFile;

    inputFile.open(inFile);
        if (!inputFile.is_open()) {
            std::cerr << "Error: не удалось открыть файл " << inFile << std::endl;
        }
        else {
            outputFile.open(outFile);
            if (!outputFile.is_open()) {
                std::cerr << "Error: не удалось открыть файл " << outFile << std::endl;
            }
            else {
                filesOpened = true;
            }
        }
    }
    while (!filesOpened);

    // Инициализация шпионов
    initSpy(queueSpy, inputFile);
    std::cout << " Инициализация прошла " << std::endl;

    // Запуск симуляции обработки
    simulateObservation(queueSpy, outputFile);
    std::cout << " Симуляция завершена " << std::endl;

    inputFile.close();
    outputFile.close();
    return 0;
}
