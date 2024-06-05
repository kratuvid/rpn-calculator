.PHONY: all clean clean-all

BUILD_SUFFIX := debug
ifdef release
	BUILD_SUFFIX := release
endif
BUILD_LOCATION := build/$(BUILD_SUFFIX)
HEADERS_LOCATION := inc
SOURCES_LOCATION := src
OBJECTS_LOCATION := $(BUILD_LOCATION)/objects
OBJECTS_SPECIFIC_LOCATION := $(addprefix $(OBJECTS_LOCATION)/,wc arbit tests)

DEPENDENCIES := readline

DEBUG_FLAGS := -g -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG
CURRENT_FLAGS := -std=c++23 -fmodules-ts
ifdef release
	CURRENT_FLAGS += $(RELEASE_FLAGS)
else
	CURRENT_FLAGS += $(DEBUG_FLAGS)
endif

CXX := g++
CXXFLAGS := -I$(HEADERS_LOCATION) $(CURRENT_FLAGS)
CXXFLAGS += $(shell pkg-config $(DEPENDENCIES) --cflags)
LDFLAGS := $(shell pkg-config $(DEPENDENCIES) --libs)

SYS_MODULES_LOCATION := gcm.cache/usr/include/c++/14.1.1
SYS_MODULES := iostream print format cstdint cmath random limits array exception string string_view unordered_map \
	any cctype chrono cstring deque fstream list tuple sstream vector
SYS_MODULES_ALL := $(addsuffix .gcm,$(addprefix $(SYS_MODULES_LOCATION)/,$(SYS_MODULES)))

# HEADERS := $(wildcard $(HEADERS_LOCATION)/*.hpp $(HEADERS_LOCATION)/*.inl)
WC_SOURCES := $(addprefix $(SOURCES_LOCATION)/wc/,$(addsuffix .cpp,main operations wc))
ARBIT_SOURCES := $(addprefix $(SOURCES_LOCATION)/arbit/,$(addsuffix .cpp,arbit basic bits construction heap operations))
SOURCES := $(WC_SOURCES) $(ARBIT_SOURCES)
OBJECTS := $(SOURCES:$(SOURCES_LOCATION)/%.cpp=$(OBJECTS_LOCATION)/%.o)
MAIN_OBJECTS := $(addprefix $(OBJECTS_LOCATION)/,wc/main.o)
CLEAN_OBJECTS := $(filter-out $(MAIN_OBJECTS),$(OBJECTS))
DEPENDS := $(SOURCES:$(SOURCES_LOCATION)/%.cpp=$(OBJECTS_LOCATION)/%.d)
BINARIES := $(addprefix $(BUILD_LOCATION)/,wc)

TEST_BARE = floats fp mul modules
# TEST_SOURCES := $(addprefix $(SOURCES_LOCATION)/tests/,$(addsuffix .cpp,$(TEST_BARE)))
# TEST_OBJECTS := $(TEST_SOURCES:$(SOURCES_LOCATION)/%.cpp=$(OBJECTS_LOCATION)/%.o)
TEST_BINARIES := $(addprefix $(BUILD_LOCATION)/,$(TEST_BARE))

all: $(BINARIES) $(TEST_BINARIES)

$(BUILD_LOCATION)/wc: $(OBJECTS_LOCATION)/wc/main.o
$(BINARIES): $(CLEAN_OBJECTS)
	@-mkdir $(BUILD_LOCATION) 2>/dev/null; true
	$(CXX) $(LDFLAGS) $^ -o $@

$(TEST_BINARIES): $(BUILD_LOCATION)/%: $(SOURCES_LOCATION)/tests/%.cpp
	@-mkdir $(BUILD_LOCATION) 2>/dev/null; true
	$(CXX) $(CURRENT_FLAGS) $(@:$(BUILD_LOCATION)/%=$(SOURCES_LOCATION)/tests/%.cpp) -o $@

# -include $(DEPENDS)

$(OBJECTS): $(OBJECTS_LOCATION)/%.o: $(SOURCES_LOCATION)/%.cpp $(SYS_MODULES_ALL)
	@-mkdir -p $(OBJECTS_SPECIFIC_LOCATION) 2>/dev/null; true
	$(CXX) $(CXXFLAGS) -MMD -MP $< -c -o $@

$(SYS_MODULES_ALL):
	$(CXX) $(CXXFLAGS) -xc++-system-header $(basename $(notdir $@))

clean: clean-tests
	rm -rf $(BINARIES) $(OBJECTS_LOCATION)

clean-tests:
	rm -rf $(TEST_BINARIES)

clean-all: clean
	rm -rf $(SYS_MODULES_LOCATION)
