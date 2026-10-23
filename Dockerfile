FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    clang make python3 gdb ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY Makefile ./
COPY src/ ./src/
COPY tests/ ./tests/
CMD ["bash"]
