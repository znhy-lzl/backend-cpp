TARGET = download

CXX = g++
CXXFLAGS =
LIBS = -lcurl -lpthread
DIRS = .
OBJS_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
HEADERS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.h))
OBJS = $(patsubst %.cpp, %.o, $(OBJS_FILES))
INCLUDE_PATH = -I./

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(INCLUDE_PATH) $(CXXFLAGS) $(LIBS)

$(OBJS) : %.o:%.cpp
	$(CXX) -g -c $< -o $@ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
