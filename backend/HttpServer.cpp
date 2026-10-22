#include "HttpServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cctype>
#include <cstdlib>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SocketType;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int SocketType;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define CLOSE_SOCKET close
#endif

namespace {
std::string decode(const std::string& value) {
    std::string result;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '+') result += ' ';
        else if (value[i] == '%' && i + 2 < value.size()) {
            std::string hex = value.substr(i + 1, 2);
            char c = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            result += c; i += 2;
        } else result += value[i];
    }
    return result;
}

std::map<std::string, std::string> parseForm(const std::string& body) {
    std::map<std::string, std::string> values;
    std::stringstream stream(body);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        size_t equals = pair.find('=');
        if (equals != std::string::npos)
            values[decode(pair.substr(0, equals))] = decode(pair.substr(equals + 1));
    }
    return values;
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "<h1>Frontend file not found</h1><p>Run the app from the project root folder.</p>";
    std::ostringstream content; content << file.rdbuf(); return content.str();
}

void sendResponse(SocketType client, const std::string& status,
                  const std::string& type, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
             << "Content-Type: " << type << "; charset=utf-8\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Connection: close\r\n\r\n" << body;
    std::string message = response.str();
    send(client, message.c_str(), static_cast<int>(message.size()), 0);
}
}

void runServer(BudgetManager& manager, int port) {
#ifdef _WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        std::cerr << "Could not start Windows networking.\n"; return;
    }
#endif
    SocketType server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        std::cerr << "Could not create server socket.\n"; return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(static_cast<unsigned short>(port));

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR ||
        listen(server, 10) == SOCKET_ERROR) {
        std::cerr << "Could not open port " << port << ". Is another app using it?\n";
        CLOSE_SOCKET(server);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    std::cout << "Budget Manager is running at http://localhost:" << port << "\n";
    std::cout << "Keep this terminal open. Press Ctrl+C to stop.\n";

    while (true) {
        SocketType client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        char buffer[16384]{};
        int received = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) { CLOSE_SOCKET(client); continue; }
        std::string request(buffer, received);
        std::istringstream firstLine(request);
        std::string method, path, version;
        firstLine >> method >> path >> version;

        size_t bodyStart = request.find("\r\n\r\n");
        std::string body = bodyStart == std::string::npos ? "" : request.substr(bodyStart + 4);

        if (method == "GET" && (path == "/" || path == "/index.html")) {
            sendResponse(client, "200 OK", "text/html", readFile("frontend/index.html"));
        } else if (method == "GET" && path == "/style.css") {
            sendResponse(client, "200 OK", "text/css", readFile("frontend/style.css"));
        } else if (method == "GET" && path == "/app.js") {
            sendResponse(client, "200 OK", "application/javascript", readFile("frontend/app.js"));
        } else if (method == "GET" && path == "/api/summary") {
            sendResponse(client, "200 OK", "application/json", manager.summaryAsJson());
        } else if (method == "GET" && path == "/api/transactions") {
            sendResponse(client, "200 OK", "application/json", manager.transactionsAsJson());
        } else if (method == "POST" && path == "/api/transaction") {
            // Forms are sent as simple URL-encoded values from the browser.
            auto values = parseForm(body);
            bool ok = false;
            try {
                ok = manager.addTransaction(values["type"], values["title"],
                    values["category"], std::stod(values["amount"]), values["date"]);
            } catch (...) { ok = false; }
            sendResponse(client, ok ? "200 OK" : "400 Bad Request", "application/json",
                ok ? "{\"success\":true}" : "{\"success\":false,\"message\":\"Check all fields and amount.\"}");
        } else if (method == "POST" && path == "/api/budget") {
            auto values = parseForm(body);
            try {
                double amount = std::stod(values["amount"]);
                if (amount >= 0 && amount <= 100000000) {
                    manager.setBudget(amount);
                    sendResponse(client, "200 OK", "application/json", "{\"success\":true}");
                } else sendResponse(client, "400 Bad Request", "application/json", "{\"success\":false}");
            } catch (...) {
                sendResponse(client, "400 Bad Request", "application/json", "{\"success\":false}");
            }
        } else {
            sendResponse(client, "404 Not Found", "application/json", "{\"error\":\"Not found\"}");
        }
        CLOSE_SOCKET(client);
    }
}
