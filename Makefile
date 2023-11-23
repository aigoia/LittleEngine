# include .env
CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

vulkan.out: *.cpp *.hpp
	g++ $(CFLAGS) -o vulkan.out *.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkan.out
	./vulkan.out

clean: 
	rm -f vulkan.out