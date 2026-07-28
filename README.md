# Alexandria

School library management system, built with a client-server architecture over gRPC.

Alexandria lets school staff manage a book catalog and user accounts from any computer on the local network, while a single machine (e.g. the school office) hosts the central server and database.

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Technology Stack](#technology-stack)
- [Technical Decisions](#technical-decisions)
- [Getting Started](#getting-started)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Known Limitations & Future Work](#known-limitations--future-work)

## Overview

Alexandria is composed of two independent applications:

- **Server** (`alexandria_server`): a headless gRPC service that owns the SQLite database, enforces business rules, authentication and role-based access control.
- **Client** (`alexandria_client`): a Qt Quick (QML) desktop application that connects to the server over the local network and provides the user interface for librarians (admins) and regular staff (users).

Two roles are supported:

- **Admin**: full CRUD on books, user management, CSV import/export.
- **User**: read-only access to the book catalog (search and browse only).

## Architecture

The system follows a layered architecture on both sides, with dependencies always pointing inward toward abstractions.

### Server layers

```
grpc/          → thin adapters between protobuf messages and domain/service types
service/       → business rules (validation, orchestration), framework-agnostic
auth/          → password hashing, session tokens
repository/    → interfaces (contracts) for data access
persistence/   → SQLite implementation of the repository interfaces
domain/        → plain structs representing core concepts (Book, User, Role)
```

Each layer only depends on the abstractions of the layer below it, never on concrete implementations. For example, `BookService` depends on `IBookRepository` (an interface), not on `SqliteBookRepository` directly. This is what allows the service layer to be unit-tested with in-memory mocks (GoogleMock) instead of a real database.

### Client layers

```
qml/            → views (pages, dialogs, reusable styled components)
viewmodels/     → QObject-based bridges exposing C++ state/actions to QML (MVVM)
core/           → AlexandriaClient, a framework-agnostic gRPC client wrapper
```

`AlexandriaClient` has no Qt dependency — it only depends on gRPC-generated stubs and plain domain types. ViewModels wrap it with `QObject`, `Q_PROPERTY`, and `Q_INVOKABLE` so QML can bind to it reactively. This separation is what allows the client to have its own integration tests (spinning up a real in-process gRPC server) independent of any Qt Quick UI code.

### Communication

Client and server communicate exclusively through gRPC, using contracts defined in `proto/`. Authentication uses a bearer-style session token sent as gRPC metadata (`authorization` key) on every authenticated call. The server validates the token and role on every request through a shared `auth_guard::requireRole` helper.

## Technology Stack

- **Language**: C++17
- **UI**: Qt 6 (Widgets-free), Qt Quick / QML
- **RPC**: gRPC + Protocol Buffers
- **Database**: SQLite via Qt SQL module
- **Password hashing**: libsodium (Argon2)
- **Testing**: GoogleTest + GoogleMock
- **Build**: CMake
- **Containerization**: Docker (multi-stage build), Makefile for automation

## Technical Decisions

This section documents deliberate trade-offs made throughout the project, since they reflect real engineering judgment rather than defaults.

- **gRPC over Qt Remote Objects.** Qt Remote Objects would be the more idiomatic choice for a pure Qt-to-Qt scenario, but gRPC/Protobuf was chosen deliberately to demonstrate a technology explicitly relevant to distributed/connected systems, where a backend may not always be a Qt/C++ application.
- **No TLS on the gRPC channel.** The server is designed to run on a trusted local network only, never exposed to the public internet. `grpc::InsecureServerCredentials()` reflects that constraint explicitly, not an oversight.
- **Sessions are in-memory and volatile.** `SessionManager` does not persist tokens to disk. Restarting the server invalidates all active sessions. This was an accepted trade-off for simplicity; a production system serving many concurrent users over long uptimes might persist sessions in the database with expiration.
- **A single mutex protects all database access.** `QSqlDatabase`/`QSqlQuery` are not thread-safe, and gRPC serves each call on its own thread from an internal pool. A single mutex in `Database` serializes all queries. This trades some throughput for correctness; given the expected scale (a school library), this is not a meaningful bottleneck.
- **ISBN uniqueness is enforced in the service layer, not via a database constraint.** This keeps the business rule explicit and testable in C++ without a live database, at the cost of relying on application code rather than the schema to guarantee the invariant.
- **CSV import/export travels as a string over gRPC, not as a file path.** Since the client and server may run on different machines, the server generates/consumes CSV content in memory and the client handles the actual file I/O locally.
- **Client-side role checks (hiding buttons) are UX only, not security.** The server independently re-validates every request's role via `auth_guard`, so a modified or malicious client gains no privilege by bypassing the UI.
- **Connectivity errors are detected via a combination of `grpc::StatusCode::UNAVAILABLE` and known transport-error message substrings** (e.g. "Socket closed"), because gRPC C++ does not always surface network failures as `UNAVAILABLE` consistently across every failure mode.

## Getting Started

### Prerequisites (native build)

- CMake 3.16+
- A C and C++17 compiler (GCC/Clang)
- Qt 6.5+ (Core, Sql, Quick, QuickControls2, Concurrent, QuickDialogs2)
- Protobuf and gRPC (with C++ bindings)
- libsodium

On Fedora:

```bash
sudo dnf install gcc gcc-c++ cmake ninja-build git \
    protobuf-devel protobuf-compiler grpc-devel grpc-plugins grpc-cpp \
    libsodium-devel mesa-libGL-devel libxkbcommon-devel libxcb-devel mesa-libEGL-devel
```

On Ubuntu/Debian:

```bash
sudo apt install build-essential cmake ninja-build git \
    libprotobuf-dev protobuf-compiler protobuf-compiler-grpc libgrpc++-dev \
    libsodium-dev libgl1-mesa-dev libxkbcommon-dev libxcb1-dev libegl1-mesa-dev
```

On Arch Linux:

```bash
sudo pacman -S base-devel cmake ninja git protobuf grpc libsodium mesa libxkbcommon libxcb
```

Package names and exact split between `grpc`/`grpc++` vary slightly across distributions and release versions; if `find_package`/`pkg-config` fails to locate a component, check your distro's package search for the exact name.

Qt 6 itself is expected to be installed separately (e.g. via the Qt Online Installer), with `CMAKE_PREFIX_PATH` pointed at its installation directory.

### Build and run natively

```bash
make build
make run       # runs the server
```

The server listens on `0.0.0.0:50051` by default and creates `alexandria.db` in the working directory. On first run, it seeds an initial administrator account (`admin` / `admin123` — change this password after first login).

Configuration is done via environment variables:

| Variable                     | Default              | Description                        |
|-------------------------------|-----------------------|------------------------------------|
| `ALEXANDRIA_DB_PATH`          | `alexandria.db`       | Path to the SQLite database file   |
| `ALEXANDRIA_LISTEN_ADDRESS`   | `0.0.0.0:50051`       | Address/port the server listens on |

To run the client, build the project with the desktop Qt kit through Qt Creator (or `cmake --build`), then run `alexandria_client`. On first launch it will ask for the server's address (e.g. `192.168.1.10:50051`).

### Run with Docker

The server can be built and run in a container without installing any dependency locally:

```bash
make docker-build
make docker-run
```

The container uses `--init` (tini) so that `SIGTERM`/`SIGINT` are forwarded correctly for a graceful shutdown, since the server process is not PID 1 inside the container.

Database persistence across container restarts is handled through a named Docker volume (`alexandria_data`, mounted at `/data`).

### Building on Windows

> **Note:** this project currently targets Linux, and the Windows build path described below has not yet been validated on a real Windows machine. The CMake configuration includes conditional logic (`if(WIN32)`) to resolve dependencies through [vcpkg](https://vcpkg.io) instead of pkg-config, but treat this section as a documented starting point rather than a verified procedure.

Prerequisites:

- Visual Studio 2022 (with the "Desktop development with C++" workload) or an equivalent MSVC toolchain
- [vcpkg](https://github.com/microsoft/vcpkg), bootstrapped and integrated with CMake (`vcpkg integrate install`)
- Qt 6.5+ for MSVC (via the Qt Online Installer)

Dependencies (Protobuf, gRPC, libsodium) are declared in `vcpkg.json` and are expected to be resolved automatically by vcpkg's manifest mode when configuring the project, provided the vcpkg toolchain file is passed to CMake:

```powershell
cmake -S . -B build ^
    -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_PREFIX_PATH=<path-to-Qt>\6.5.0\msvc2019_64
cmake --build build --config Release
```

To package the client for distribution, Qt provides `windeployqt`, which copies the required Qt DLLs (and QML modules) next to the built executable:

```powershell
windeployqt.exe --qmldir client\qml build\Release\alexandria_client.exe
```

The resulting folder (executable + DLLs) can then be zipped or wrapped in an installer for distribution to school computers. Packaging the server the same way, and testing the full client-server flow across two Windows machines on the same network, is left as a follow-up validation step.

## Testing

Tests are run via CTest:

```bash
make test
```

This includes:

- **Unit tests** for repositories (in-memory SQLite), services (with mocked repositories via GoogleMock), authentication, and CSV import/export.
- **Integration tests** that spin up a real in-process gRPC server and exercise `AlexandriaClient` against it end-to-end — covering login, role-based authorization, the full book lifecycle (create/read/update/delete), and CSV round-tripping.

## Project Structure

```
alexandria_grpc/
├── proto/              gRPC/Protobuf contract definitions
├── server/
│   ├── domain/          Plain domain models (Book, User, Role)
│   ├── repository/      Repository interfaces
│   ├── persistence/      SQLite implementations
│   ├── auth/             Password hashing, sessions, authentication service
│   ├── service/          Business logic (books, users, CSV)
│   ├── grpc/             gRPC service implementations
│   ├── tests/            Unit tests
│   └── main.cpp
├── client/
│   ├── core/             Framework-agnostic gRPC client wrapper
│   ├── viewmodels/       QObject bridges for QML
│   ├── qml/              UI pages, dialogs, and reusable styled components
│   ├── tests/            Integration tests
│   └── main.cpp
├── Dockerfile
├── Makefile
└── CMakeLists.txt
```

## Known Limitations & Future Work

- Sessions do not expire and are lost on server restart (see [Technical Decisions](#technical-decisions)).
- No automatic reconnection backoff strategy — reconnection is retried on the next user action or app restart.
- Windows support is prepared at the CMake/dependency level (vcpkg) but has not been validated on an actual Windows machine yet — see [Building on Windows](#building-on-windows).