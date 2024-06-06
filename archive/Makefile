.PHONY: all clean clean-bin clean-tests clean-all switch-mods-common switch-mods-debug switch-mods-release switch-mods-internal

BUILD_SUFFIX := debug
BUILD_SUFFIX_BMI := debug
ifdef release
	BUILD_SUFFIX := release
	BUILD_SUFFIX_BMI := release-bare
endif
BUILD_LOCATION_BARE := build
BUILD_LOCATION = $(BUILD_LOCATION_BARE)/$(BUILD_SUFFIX)
HEADERS_LOCATION := inc
SOURCES_LOCATION := src
OBJECTS_LOCATION := $(BUILD_LOCATION)/objects
BMI_CACHE_LOCATION_BARE := gcm.cache
BMI_CACHE_LOCATION = $(BUILD_LOCATION)/$(BMI_CACHE_LOCATION_BARE)
OBJECTS_SPECIFIC_LOCATION := $(addprefix $(OBJECTS_LOCATION)/,wc arbit tests)

DEPENDENCIES := readline

DEBUG_FLAGS := -g -DDEBUG
RELEASE_FLAGS := -DNDEBUG
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
BMI := $(addprefix gcm.cache/,arbit.gcm)

TEST_BARE = floats fp mul modules
# TEST_SOURCES := $(addprefix $(SOURCES_LOCATION)/tests/,$(addsuffix .cpp,$(TEST_BARE)))
# TEST_OBJECTS := $(TEST_SOURCES:$(SOURCES_LOCATION)/%.cpp=$(OBJECTS_LOCATION)/%.o)
TEST_BINARIES := $(addprefix $(BUILD_LOCATION)/,$(TEST_BARE))

all: $(BINARIES) $(TEST_BINARIES)

$(BUILD_LOCATION)/wc: $(OBJECTS_LOCATION)/wc/main.o
$(BINARIES): $(CLEAN_OBJECTS)
	@-mkdir $(BUILD_LOCATION) 2>/dev/null; true
	$(CXX) $(LDFLAGS) $^ -o $@
ifdef release
	strip $@
endif

$(TEST_BINARIES): $(BUILD_LOCATION)/%: $(SOURCES_LOCATION)/tests/%.cpp switch-mods-internal
	@-mkdir $(BUILD_LOCATION) 2>/dev/null; true
	$(CXX) $(CURRENT_FLAGS) $(@:$(BUILD_LOCATION)/%=$(SOURCES_LOCATION)/tests/%.cpp) -o $@

# -include $(DEPENDS)

$(OBJECTS): $(OBJECTS_LOCATION)/%.o: $(SOURCES_LOCATION)/%.cpp $(SYS_MODULES_ALL)
	@-mkdir -p $(OBJECTS_SPECIFIC_LOCATION) 2>/dev/null; true
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(SYS_MODULES_ALL):
	@-mkdir -p $(BMI_CACHE_LOCATION) 2>/dev/null; true
	@-rm -f gcm.cache 2>/dev/null; true
	@-ln -sf $(PWD)/$(BMI_CACHE_LOCATION) gcm.cache 2>/dev/null; true
	@-ln -sf $(HOME)/.cache/base.gcm.cache/$(BUILD_SUFFIX_BMI) $(PWD)/$(BMI_CACHE_LOCATION)/usr 2>/dev/null; true
	$(CXX) $(CXXFLAGS) -xc++-system-header $(basename $(notdir $@))

switch-mods-debug: BUILD_SUFFIX := debug
switch-mods-debug: switch-mods-common
	-ln -sf $(HOME)/.cache/base.gcm.cache/debug $(PWD)/$(BMI_CACHE_LOCATION)/usr 2>/dev/null; true

switch-mods-release: BUILD_SUFFIX := release
switch-mods-release: switch-mods-common
	-ln -sf $(HOME)/.cache/base.gcm.cache/release-bare $(PWD)/$(BMI_CACHE_LOCATION)/usr 2>/dev/null; true

switch-mods-internal:
	-ln -sf $(HOME)/.cache/base.gcm.cache/$(BUILD_SUFFIX_BMI) $(PWD)/$(BMI_CACHE_LOCATION)/usr 2>/dev/null; true

switch-mods-common:
	-mkdir -p $(BMI_CACHE_LOCATION) 2>/dev/null; true
	-rm -f gcm.cache 2>/dev/null; true
	-ln -sf $(PWD)/$(BMI_CACHE_LOCATION) $(PWD)/gcm.cache 2>/dev/null; true

clean: clean-bin clean-tests
	rm -rf $(OBJECTS_LOCATION) $(BMI) gcm.cache

clean-bin:
	rm -f $(BINARIES)

clean-tests:
	rm -f $(TEST_BINARIES)

clean-all: clean
	rm -rf $(HOME)/.cache/base.gcm.cache/$(BUILD_SUFFIX_BMI)/include $(BMI_CACHE_LOCATION)
