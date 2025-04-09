# C++ Project Template

This is a C++20 project template that includes:

- **cpp-httplib**: A C++11 single-file header-only HTTP/HTTPS library.
- **nlohmann/json**: JSON for Modern C++.
- **cpplint**: A tool to ensure C++ code adheres to Google's style guide.
- **astyle**: A code formatter for C, C++, C#, and Java.

## Features

- **Containerized Development**: Utilizes Docker to provide a consistent development environment.
- **Dependency Management**: Automatically fetches and integrates the latest releases of `cpp-httplib` and `nlohmann/json` using CMake's `FetchContent`.
- **Code Quality Tools**: Integrates `cpplint` and `astyle` for linting and formatting.

## Getting Started

### Prerequisites

- [Docker](https://www.docker.com/get-started)

### Setup

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/yourusername/cpp_project_template.git
   cd cpp_project_template
