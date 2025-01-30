#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <fstream>
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

int PORT = 50000;
using namespace std;

void startServer();

int main() {
    setlocale(LC_ALL, "Russian");
    startServer();

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

void startServer() {
    int server_fd, new_socket; // Дескрипторы сокетов
    struct sockaddr_in address;
    int opt = 1; // Опция для сокета (разрешение повторного использования адреса)
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Привязка сокета к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; // Указываем семью адресов (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // Привязываем сокет ко всем доступным интерфейсам
    address.sin_port = htons(PORT); // Указываем порт (преобразуем в сетевой порядок байтов)

    // Привязка сокета к адресу и порту
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Начало прослушивания
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен" << std::endl;

    while (true) {
        // Принятие входящего соединения
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "Клиент подключен." << std::endl;
        // Обработка сообщений от клиента в цикле
        while (true) {
            int valread = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
            if (valread <= 0) {
                break; // Выход из цикла, если клиент отключился или произошла ошибка
            }

            if (isStringValid(buffer)) {
                std::cout << "Получено сообщение: " << buffer << std::endl;
            } else {
                std::cout << "Ошибка: функция 3 возвращает ложь " << std::endl;
            }

            // Очистка буфера
            memset(buffer, 0, sizeof(buffer));
        }

#ifdef _WIN32
        closesocket(new_socket); // Закрываем сокет после завершения общения с клиентом
#else
        close(new_socket); // Закрываем сокет после завершения общения с клиентом
#endif
    }
}
