#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <fstream>
#include <algorithm>
#include "mylib.h"

// #ifdef _WIN32
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #pragma comment(lib, "ws2_32.lib")
// #elif defined(LINUX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
// #else
// #error "platform not supported"
// #endif

using namespace std;

bool isOnlyNumbers(const string& str);

int PORT = 50000;
void readPortFromFile(const string& filename);

string stringBuffer;
bool isBufferFrei = true;

void thread1Function();
void thread2Function();
condition_variable cv;
mutex mtx;

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Program1 working..." << endl;

    //readPortFromFile("port.txt");

    thread thread1(thread1Function);
    thread thread2(thread2Function);

    thread1.join();
    thread2.join();

#ifdef _WIN32
    WSACleanup();
#endif
}

void thread1Function() {
    string line;

    while (getline(cin, line) && isBufferFrei == true) {
        if (isOnlyNumbers(line) && line.size() <= 64) {
            SortAndReplaceEvenCharsToKB(line);
            lock_guard<mutex> lock(mtx);
            stringBuffer = line;
            isBufferFrei = false;
            cv.notify_one();
        }
    }
}

void thread2Function() {
    struct sockaddr_in serv_addr = {0};

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    const char* ip = "127.0.0.1";
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        std::cout << "Неверный адрес/адрес не поддерживается" << std::endl;
        return;
    }

    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !isBufferFrei; });

        while (true) {
            string input = stringBuffer;
            cout << "thread2Function получил " << input << endl;
            int sum = SumOfNumbers(input);

            //попытка создания сокета
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                std::cout << "Ошибка создания сокета" << std::endl;
                continue;
            }

            // Пытаемся подключиться к серверу
            while (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cout << "Ошибка подключения. Повторная попытка через 10 секунд..." << std::endl;
                close(sock);
                std::this_thread::sleep_for(std::chrono::seconds(10)); // Ждем 10 секунд перед повторной попыткой
                sock = socket(AF_INET, SOCK_STREAM, 0); // Создаем новый сокет
                if (sock < 0) {
                    std::cout << "Ошибка создания сокета" << std::endl;
                    continue;
                }
            }

            // Отправляем сообщение
            int bytes_sent = send(sock, to_string(sum).c_str(), to_string(sum).size(), 0);
            if (bytes_sent < 0) {
                std::cout << "Ошибка при отправке сообщения." << std::endl;
            } else {
                std::cout << "Сообщение отправлено." << std::endl;
                close(sock);
                break;
            }

            close(sock); // Закрываем сокет после использования
        }
        isBufferFrei = true; // Устанавливаем флаг обратно
        cv.notify_all();
    }
}

bool isOnlyNumbers(const string& str) {
    for (char symbol : str) {
        if (symbol < '0' || symbol > '9') {
            return false;
        }
    }
    return true;
}

void readPortFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string portStr;
        file >> portStr; // Считываем порт из файла
        if (isOnlyNumbers(portStr)) {
            int x = stoi(portStr); // Преобразуем строку в число и присваиваем глобальной переменной PORT
            cout << "Порт изменен на: " << PORT << endl;
        } else {
            cout << "Неверный формат порта в файле. Порт остается: " << PORT << endl;
        }
        file.close();
    } else {
        cout << "Не удалось открыть файл: " << filename << endl;
    }
}
