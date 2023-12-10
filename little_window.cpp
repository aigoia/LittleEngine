#include "little_window.hpp"

#include <stdexcept>

namespace little
{
    LittleWindow::LittleWindow(int w, int h, std::string name) : width(w), height(h), windowName(name)
    {
        initWindow();
    }

    LittleWindow::~LittleWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LittleWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void LittleWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void LittleWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto littleWindow = reinterpret_cast<LittleWindow *>(glfwGetWindowUserPointer(window));
        littleWindow->framebufferResized = true;
        littleWindow->width = width;
        littleWindow->height = height;
    }
}