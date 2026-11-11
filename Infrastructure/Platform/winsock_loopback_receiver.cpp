#include "winsock_loopback_receiver.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
//small helpers that read and answer the browser's request
namespace {
    const std::uintptr_t kInvalid = static_cast<std::uintptr_t>(INVALID_SOCKET);

    // The first line of a request looks like "GET /?code=abc&state=xyz HTTP/1.1".
    // Returns the middle part: "/?code=abc&state=xyz".
    std::string ReadRequestTarget(SOCKET client) {
        DWORD timeoutMs = 3000; //dont wait forever for a slow connection, give a 3 second read timeout
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs));

        std::string request;
        char buffer[1024];
        while (request.size() < 8192 && request.find("\r\n") == std::string::npos) { //read until the end of the first line, with an 8 KB cap so a client cannot flood us
            const int received = recv(client, buffer, sizeof(buffer), 0);
            if (received <= 0) {
                break;
            }
            request.append(buffer, static_cast<size_t>(received));
        }

        const size_t lineEnd = request.find("\r\n");
        if (lineEnd == std::string::npos) {
            return "";
        }
        const std::string line = request.substr(0, lineEnd);
        const size_t firstSpace = line.find(' ');
        if (firstSpace == std::string::npos) {
            return "";
        }
        const size_t secondSpace = line.find(' ', firstSpace + 1);
        if (secondSpace == std::string::npos) {
            return "";
        }
        return line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    }
    //sends a minimal HTTP response (status, headers, body)
    void SendPage(SOCKET client, const std::string& status, const std::string& body) {
        const std::string response =
            "HTTP/1.1 " + status + "\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n\r\n" + body;
        send(client, response.c_str(), static_cast<int>(response.size()), 0);
    }
}

WinsockLoopbackReceiver::~WinsockLoopbackReceiver() {
    Stop();
}
//starts listening on a free local port and returns the port number (0 on failure)
int WinsockLoopbackReceiver::Start() {
    Stop();
    //initialize winsock (Windows Sockets API)
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        return 0;
    }
    winsockStarted = true;
    //create a TCP socket for the listener
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        Stop();
        return 0;
    }
    listener = static_cast<std::uintptr_t>(sock);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = 0;                                   // let Windows pick a free port
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);       // 127.0.0.1 only, never the network
    //bind the socket to the chosen port and start listening for incoming connections
    if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0 ||
        listen(sock, 4) != 0) {
        Stop();
        return 0;
    }

    //retrieve the actual port number assigned by Windows
    int length = sizeof(address);
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&address), &length) != 0) {
        Stop();
        return 0;
    }
    return ntohs(address.sin_port); //return the actual port number assigned by Windows
}
//waits up to timeoutSeconds for Google's redirect and returns the request target (or "" on timeout)
std::string WinsockLoopbackReceiver::WaitForRedirect(int timeoutSeconds) {
    if (listener == kInvalid) {
        return "";
    }
    const SOCKET sock = static_cast<SOCKET>(listener);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSeconds); 

    while (std::chrono::steady_clock::now() < deadline) { //check once a second so we can notice the deadline passing
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);
        timeval pause{1, 0};
        const int ready = select(0, &readSet, nullptr, nullptr, &pause); //select() waits up to 1 second for an incoming connection
        if (ready == SOCKET_ERROR) {
            return "";
        }
        if (ready == 0) {
            continue;
        }

        SOCKET client = accept(sock, nullptr, nullptr); //a browser connected, accept it and read what it asked for
        if (client == INVALID_SOCKET) {
            continue;
        }
        const std::string target = ReadRequestTarget(client);
        // Browsers sometimes also ask for /favicon.ico; only a real Google redirect counts.
        const bool isRedirect = target.find("code=") != std::string::npos ||
                                target.find("error=") != std::string::npos;
        if (isRedirect) { 
            SendPage(client, "200 OK", //reply "you can close this tab", for anything else send a 404
                     "<html><body style=\"font-family:sans-serif\">"
                     "<h3>Merken</h3><p>You can close this tab and return to the app.</p>" 
                     "</body></html>");
        } else {
            SendPage(client, "404 Not Found", "");
        }
        closesocket(client);
        if (isRedirect) { //the caller gets "/?code=...&state=..." and parses it
            return target;
        }
    }
    return "";
}
//close the listener and shut winsock down
void WinsockLoopbackReceiver::Stop() {
    if (listener != kInvalid) {
        closesocket(static_cast<SOCKET>(listener));
        listener = kInvalid;
    }
    if (winsockStarted) {
        WSACleanup();
        winsockStarted = false;
    }
}