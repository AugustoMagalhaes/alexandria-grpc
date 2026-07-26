FROM fedora:41 AS builder

RUN dnf install -y \
    gcc \
    gcc-c++ \
    cmake \
    ninja-build \
    git \
    qt6-qtbase-devel \
    qt6-qtdeclarative-devel \
    protobuf-devel \
    protobuf-compiler \
    grpc-devel \
    grpc-plugins \
    libsodium-devel \
    pkgconf-pkg-config \
    mesa-libGL-devel \
    && dnf clean all

WORKDIR /build

COPY CMakeLists.txt .
COPY proto/ proto/
COPY server/ server/

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DBUILD_CLIENT=OFF \
    && cmake --build build --target alexandria_server -j$(nproc)

FROM fedora:41

RUN dnf install -y \
    qt6-qtbase \
    grpc \
    grpc-cpp \
    protobuf \
    libsodium \
    && dnf clean all

WORKDIR /app

COPY --from=builder /build/build/server/alexandria_server .

ENV ALEXANDRIA_DB_PATH=/data/alexandria.db
ENV ALEXANDRIA_LISTEN_ADDRESS=0.0.0.0:50051

VOLUME /data

EXPOSE 50051

ENTRYPOINT ["./alexandria_server"]