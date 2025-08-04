# 🎬 StreamFlix - C Streaming Platform

A comprehensive command-line streaming platform simulation built in **C**, featuring complete user management, content tracking, viewing analytics, and persistent data storage through CSV files.

> **Academic Project** | Developed as part of Computer Engineering studies to demonstrate proficiency in systems programming, modular design patterns, and data structure implementation in C.

---

## ✨ Key Features

### 🎥 **Content Management System**
- Complete CRUD operations for movies and TV shows
- Dynamic content catalog with metadata support
- Genre categorization and content filtering
- Efficient search and listing capabilities

### 👥 **User Management**
- Secure user registration and authentication
- Personal viewing history tracking
- User preference management
- Session management system

### 📊 **Analytics & Reporting**
- Real-time viewing statistics
- Most-watched content analysis
- User engagement metrics
- Personalized viewing recommendations
- Detailed activity reports

### 💾 **Data Persistence**
- CSV-based data storage system
- Automatic file generation and management
- Data integrity validation
- Cross-session data persistence

---

## 🏗️ Technical Architecture

### **Modular Design Philosophy**
The project follows a clean, modular architecture with separation of concerns:

```
📁 Project Structure
├── 📄 main.c              # Application entry point and main loop
├── 📄 csvutil.c/.h        # CSV file I/O operations
├── 📄 content.c/.h        # Content management logic
├── 📄 user.c/.h           # User authentication and management
├── 📄 list.c/.h           # Dynamic data structure implementations
├── 📄 recommendation.c/.h  # Content recommendation algorithms
├── 📄 report.c/.h         # Analytics and reporting system
├── 📄 test.c              # Unit testing framework
├── 📁 data/               # CSV data files
└── 📄 Makefile            # Build automation
```

### **Core Technologies**
- **Language**: C (C99 standard)
- **Compiler**: GCC with strict warning flags
- **Build System**: Make
- **Data Format**: CSV for persistence
- **Memory Management**: Manual allocation/deallocation
- **Data Structures**: Dynamic arrays, linked lists

---

## 🚀 Getting Started

### **Prerequisites**
```bash
# Required tools
- GCC compiler (v4.9+)
- Make build system
- Terminal/Command line interface
```

### **Quick Start**
```bash
# Clone the repository
git clone https://github.com/MJ-Sarabando/playingwith_C_streaming.git
cd playingwith_C_streaming

# Build the application
make

# Run the streaming platform
./streamflix

# Run test suite (optional)
make test
```

### **Sample Data Files**
The application automatically manages these CSV files:
- `📁 data/contents.csv` - Movie and TV show catalog
- `📁 data/users.csv` - User accounts and credentials
- `📁 data/viewing_history.csv` - User viewing records and timestamps

---

## 🧪 Testing & Quality Assurance

### **Test Coverage**
- Unit tests for all core modules
- Data validation testing
- File I/O error handling
- Memory leak detection

```bash
# Run comprehensive test suite
make test

# Clean build artifacts
make clean
```

### **Code Quality Standards**
- Strict compiler warnings (`-Wall -Wextra -pedantic`)
- C99 standard compliance
- Consistent coding style
- Comprehensive error handling

---

## 💡 Technical Highlights

### **Advanced C Programming Concepts**
- **Dynamic Memory Management**: Efficient allocation and deallocation patterns
- **File I/O Operations**: Robust CSV parsing and writing with error handling
- **Data Structures**: Implementation of dynamic arrays and linked structures
- **Modular Programming**: Clean separation of concerns across multiple modules
- **Function Pointers**: Used for flexible callback implementations
- **String Manipulation**: Advanced string processing for data parsing

### **System Design Patterns**
- **Model-View-Controller**: Separation of data, logic, and presentation
- **Factory Pattern**: Dynamic object creation for content and users
- **Observer Pattern**: Event-driven updates for viewing history
- **Strategy Pattern**: Flexible recommendation algorithms

---

## 📈 Learning Outcomes

This project demonstrates proficiency in:

- ✅ **Systems Programming**: Low-level memory management and file operations
- ✅ **Software Architecture**: Modular design and code organization
- ✅ **Data Structures**: Implementation and manipulation of complex data types
- ✅ **Algorithm Design**: Search, sort, and recommendation algorithms
- ✅ **File Systems**: CSV parsing, validation, and persistence
- ✅ **Testing**: Unit testing and quality assurance practices
- ✅ **Build Systems**: Makefile creation and dependency management

---

## 🔧 Build Configuration

The project uses a sophisticated Makefile with multiple targets:

```makefile
# Production build
make                # Compile main application

# Development tools
make test          # Build and run test suite
make clean         # Remove build artifacts

# Compiler flags ensure code quality
CFLAGS = -Wall -Wextra -pedantic -std=c99
```

---

## 🎯 Future Enhancements

Potential extensions to demonstrate additional skills:

- 🔐 **Enhanced Security**: User password hashing and session tokens
- 🌐 **Network Support**: TCP/IP client-server architecture
- 🗄️ **Database Integration**: SQLite or MySQL backend
- 🎨 **GUI Interface**: GTK+ or ncurses-based interface
- ⚡ **Performance**: Multi-threading for concurrent users
- 📱 **API Development**: RESTful API for mobile client support

---

## 👩‍💻 About the Developer

This project showcases my passion for systems programming and demonstrates my ability to build complete, production-quality applications in C. It reflects my understanding of:

- Computer Science fundamentals
- Software engineering best practices
- System-level programming concepts
- Professional development workflows

---

## 🤝 Connect & Collaborate

I'm always interested in discussing C programming, systems development, and software engineering:

[![GitHub](https://img.shields.io/badge/GitHub-MJ--Sarabando-black?style=flat&logo=github)](https://github.com/MJ-Sarabando)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Maria%20João%20Sarabando-blue?style=flat&logo=linkedin)](https://www.linkedin.com/in/mariajoaosarabando)

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

<div align="center">

**Built with ❤️ and lots of C programming**

⭐ *If you find this project interesting, please consider giving it a star!* ⭐

</div>