BUILD_DIR := build
DOCKER_IMAGE := alexandria-server
QT_PREFIX_PATH ?= $(HOME)/Qt/6.11.1/gcc_64

.PHONY: configure build test run clean docker-build docker-run

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=$(QT_PREFIX_PATH)

build: configure
	cmake --build $(BUILD_DIR) -j$(shell nproc)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/server/alexandria_server

clean:
	rm -rf $(BUILD_DIR)

docker-build:
	docker build -t $(DOCKER_IMAGE) .

docker-run:
	docker run --rm -it --init \
		-p 50051:50051 \
		-v alexandria_data:/data \
		$(DOCKER_IMAGE)
