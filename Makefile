target = main.exe

build_dir = build

srcs = sqlite3.c os.cc format.cc storable.cpp

objs = $(srcs:%=$(build_dir)/%.o)
deps = $(objs:%.o=%.d)

cflags = -g -O3 -Wall  -Wno-unused -I . -MMD -MP
ldflags = -lpthread -lm -static

all: $(target)

run: $(target)
	./$(target)

$(target): $(objs)
	g++ $^ -o $@ $(ldflags)

$(build_dir)/%.c.o: %.c
	mkdir -p $(build_dir)
	gcc $(cflags) -c $< -o $@

$(build_dir)/%.cpp.o: %.cpp
	mkdir -p $(build_dir)
	gcc $(cflags) -std=c++20 -c $< -o $@

$(build_dir)/%.cc.o: %.cc
	mkdir -p $(build_dir)
	gcc $(cflags) -c $< -o $@

.PHONY : clean

clean:
	rm -rf build
	rm -f $(target)

-include $(deps)
