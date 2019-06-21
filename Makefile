CXX = g++
CXXFLAGS += -O2 -Wall -Werror -Wextra -std=c++14
ROOTFLAGS := `root-config --cflags --libs`

BUILDDIR = ./build
LIBDIR = ./lib
SRCDIR = ./src

LIBCONF = libpp.a

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.d,$(SRCS))

.PHONY: clean

$(LIBDIR)/$(LIBCONF): $(OBJS)
	@mkdir -p $(LIBDIR)
	ar rc $@ $^
	ranlib $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -MMD -MF $(BUILDDIR)/$(*F).d $< -c -o $@

clean:
	@$(RM) $(LIBDIR)/$(LIBCONF) $(OBJS) $(DEPS)
	@rm -rf $(BUILDDIR)/*

-include $(DEPS)
