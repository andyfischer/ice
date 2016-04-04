
CFLAGS := -g -Isrc -std=c1x -DDEBUG -D_DEBUG $(CFLAGS)

CC := cc
SRCS := $(find src -name '*.c')
src_to_obj = $(patsubst src/%.c,build/obj/%.o,$(1))
OBJS := $(call src_to_obj, $(SRCS))
DEPS := $(OBJS:.o=.d)

all: build/test

-include $(DEPS)

ifndef verbose
  SILENT = @
endif

$(shell mkdir -p build/obj/test build/obj/ext build/obj/tools)

clean:
	rm -rf build

docker:
	docker build -t ice-dev .

build/obj/%.o: src/%.c
	@echo $<
	$(SILENT) $(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(SILENT) $(CC) $(CFLAGS) -c $< -o $@

build/test: $(call src_to_obj, $(wildcard src/*.c) $(wildcard src/test/*.c))
	@echo $@
	$(SILENT) $(CC) -o $@ $^

tags: $(SRCS)
	ctags -R src .
