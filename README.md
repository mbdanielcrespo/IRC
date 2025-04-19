# 💬 IRC Server

![42 Badge](https://img.shields.io/badge/42-IRC_Server-brightgreen)
![Score](https://img.shields.io/badge/Score-100%2F100-success)
![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![Status](https://img.shields.io/badge/Status-Completed-success)

<p align="center">
  <img src="https://raw.githubusercontent.com/byaliego/42-project-badges/main/badges/ft_ircm.png" alt="IRC Badge" width="150" height="150">
</p>

## 📝 Introduction

IRC (Internet Relay Chat) is a networking project at 42 School that challenges students to implement a functional IRC server from scratch. This text-based communication protocol, created in 1988, remains a fundamental example of client-server architecture and real-time messaging systems.

> "This project is about creating your own IRC server. You will use an actual IRC client to connect to your server and test it. Internet is ruled by solid standards protocols that allow connected computers to interact with each other."

This project explores socket programming, client-server architecture, protocol implementation, and concurrent programming concepts.

## 🎯 Project Objectives

- Implement a fully functional IRC server according to RFC specifications
- Handle multiple client connections simultaneously using non-blocking I/O
- Process and respond to standard IRC commands
- Manage channels, private messages, operators, and user privileges
- Implement proper error handling and client validation
- Create a robust and scalable server architecture
- Gain deep understanding of network protocols and socket programming

## 🌐 IRC Protocol: An Overview

IRC is a text-based communication protocol that follows a client-server model, allowing real-time messaging between users in channels or privately:

### Key Protocol Features

| Feature | Description | Implementation |
|---------|-------------|----------------|
| **Text-based** | Simple ASCII commands and responses | Easy to parse and generate |
| **Client-Server** | Centralized architecture | Single server handles multiple clients |
| **Channel-based** | Group conversations in named channels | Managed as collections of users |
| **User Roles** | Operators with special privileges | Access control implementation |
| **Stateful** | Server maintains user/channel state | In-memory data structures |

### Core IRC Commands

| Command | Description | Example |
|---------|-------------|---------|
| **NICK** | Set or change username | `NICK john_doe` |
| **USER** | Set user identity details | `USER john 0 * :John Doe` |
| **JOIN** | Enter a channel | `JOIN #general` |
| **PRIVMSG** | Send a message | `PRIVMSG #general :Hello everyone!` |
| **PART** | Leave a channel | `PART #general` |
| **KICK** | Remove a user from channel | `KICK #general jane_doe` |
| **MODE** | Change channel/user modes | `MODE #general +o john_doe` |
| **QUIT** | Disconnect from server | `QUIT :Gone to lunch` |

## 🏗️ Project Architecture

### Class Structure

```
┌───────────────┐      ┌───────────────┐      ┌───────────────┐
│     Server    │◄────►│     Client    │◄────►│    Channel    │
└───────┬───────┘      └───────────────┘      └───────────────┘
        │                      ▲                      ▲
        │                      │                      │
        ▼                      │                      │
┌───────────────┐      ┌───────────────┐      ┌───────────────┐
│ CommandHandler│◄────►│    Message    │─────►│    Logger     │
└───────────────┘      └───────────────┘      └───────────────┘
```

### Project Structure

```
irc/
├── inc/                # Header files
│   ├── Server.hpp      # Server class declaration
│   ├── Client.hpp      # Client class declaration
│   ├── Channel.hpp     # Channel class declaration
│   ├── CommandHandler.hpp # Command parsing and execution
│   ├── Message.hpp     # IRC message representation
│   └── utils.hpp       # Utility functions
│
├── src/                # Implementation files
│   ├── main.cpp        # Entry point
│   ├── Server.cpp      # Server implementation
│   ├── Client.cpp      # Client implementation
│   ├── Channel.cpp     # Channel implementation
│   ├── CommandHandler.cpp # Command handling logic
│   ├── Message.cpp     # Message parsing and formatting
│   └── utils.cpp       # Utility function implementations
│
├── Makefile            # Build configuration
└── README.md           # Project documentation
```

## 🛠️ Implementation Details

### Key Components

| Component | Purpose | Implementation Approach |
|-----------|---------|-------------------------|
| **Server** | Manages connections and routes messages | Socket programming with non-blocking I/O using `select()` |
| **Client** | Represents a connected user | State machine tracking authentication and activity |
| **Channel** | Manages group conversations | Collection of members with permission levels |
| **CommandHandler** | Processes IRC commands | Command pattern with function map dispatch |
| **Message** | Parses and formats IRC messages | RFC-compliant formatting and validation |

### Networking Model

| Aspect | Implementation | Notes |
|--------|----------------|-------|
| **Socket Type** | TCP/IP | Reliable, ordered delivery |
| **I/O Model** | Non-blocking with select() | Efficient handling of multiple clients |
| **Buffer Management** | Input/output queues per client | Handles partial reads/writes |
| **Connection State** | State tracking per client | Registration, authentication, channels |

## 💡 Technical Approach

My implementation focuses on correctness, performance, and maintainability:

### 1. Architecture Design
- **Event-driven model** for high concurrency
- **Non-blocking I/O** for efficient resource usage
- **Modular class design** for maintainability
- **Separation of concerns** between components

### 2. Protocol Compliance
- **RFC 1459/2812** implementation
- **Proper error codes** and messages
- **Complete command set** support
- **Input validation and sanitization**

### 3. Performance Considerations
- **Efficient data structures** for user and channel lookup
- **Minimal memory footprint** per connection
- **Optimized message parsing and generation**
- **Connection timeouts** and resource limits

## 🔧 Implemented Commands

| Category | Commands |
|----------|----------|
| **Connection** | NICK, USER, PASS, QUIT, PING, PONG |
| **Channel Operations** | JOIN, PART, TOPIC, NAMES, LIST |
| **Messaging** | PRIVMSG, NOTICE |
| **Channel Control** | MODE, KICK, INVITE |
| **Server Information** | MOTD, VERSION, INFO, TIME |
| **User Information** | WHO, WHOIS, WHOWAS |

## 💼 Skills Developed

- **Socket programming** and network I/O
- **Concurrent programming** with non-blocking I/O
- **Protocol design and implementation**
- **C++ design patterns** and object-oriented programming
- **Memory management** and resource optimization
- **Error handling** and robust programming
- **RFC specification** interpretation and implementation

## 🚀 Key Features

- **Full RFC compliance** with core IRC features
- **Robust error handling** with appropriate error codes
- **Channel operations** including modes and privileges
- **User authentication** and operator privileges
- **Private messaging** between users
- **Efficient networking** with non-blocking I/O
- **Clean architecture** with separation of concerns

---

<div align="center">
  
  ### 📊 Project Stats
  
  | Metric | Value |
  |--------|-------|
  | Final Score | 100/100 |
  | Lines of Code | ~3000 |
  | Classes | 5 core classes |
  | Commands Implemented | 5+ IRC commands |
  | Completion Time | 4 weeks |
  
</div>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Socket_Programming-FF5733?style=for-the-badge&logo=socket.io&logoColor=white" alt="Socket">
  <img src="https://img.shields.io/badge/TCP/IP-0078D4?style=for-the-badge&logo=windows-terminal&logoColor=white" alt="TCP/IP">
  <img src="https://img.shields.io/badge/42-000000?style=for-the-badge&logo=42&logoColor=white" alt="42">
</p>
