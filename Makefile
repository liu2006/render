CC := gcc
CFLAGS := -Wall -Wextra -g -O0 -MMD -MP -std=c23
CPPFLAGS := -Iinclude -Isrc -Ivendor/glfw/include -Ivendor/vulkan/x86_64/include
LDFLAGS := -Lvendor/vulkan/x86_64/lib/VulkanLoader/lib -Lvendor/glfw/lib64 
LDLIBS := -lglfw3 -lvulkan -lm	

BUILDDIR := build
TARGET := $(BUILDDIR)/render

SRCDIR := src
SRCS := $(wildcard $(SRCDIR)/*.c)

OBJDIR := $(BUILDDIR)/objdir
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

.PHONY: clean all

all: $(TARGET)
-include $(OBJS:.o=.d)

$(TARGET) : $(OBJS) | $(BUILDDIR)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR):
	mkdir -p $@ 
$(BUILDDIR):
	mkdir -p $@
clean:
	rm -rf $(OBJDIR) $(TARGET) $(BUILDDIR)

