# foldit - a terminal folding editor

CXX      ?= g++
CXXFLAGS ?= -O2 -g
CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-unused-parameter -MMD -MP
LDLIBS    = -lncursesw
PREFIX   ?= $(HOME)/.local

CORE = build/text.o build/lang.o build/fold.o
APP  = build/editor.o build/main.o

all: foldit

foldit: $(CORE) $(APP)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/test_%.o: tests/test_%.cpp | build
	$(CXX) $(CXXFLAGS) -Isrc -c -o $@ $<

build:
	mkdir -p build

build/test_fold: $(CORE) build/test_fold.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test: build/test_fold
	./build/test_fold

install: foldit
	install -D -m 755 foldit $(PREFIX)/bin/foldit
	@if [ ! -f $(HOME)/.config/foldit/foldit.ini ]; then \
	  install -D -m 644 foldit.ini $(HOME)/.config/foldit/foldit.ini; \
	  echo "installed $(HOME)/.config/foldit/foldit.ini"; fi

clean:
	rm -rf build foldit

.PHONY: all test install clean

-include build/*.d
