#pragma once

#include "little_model.hpp"

#include <memory>

namespace little
{
    struct Transform2dComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2()
        {
            const float sin = glm::sin(rotation);
            const float cos = glm::cos(rotation);

            glm::mat2 scaleMatrix{{scale.x, .0f}, {.0f, scale.y}};
            glm::mat2 rotationMatrix{{cos, sin}, {-sin, cos}};

            return scaleMatrix * rotationMatrix;
        };
    };

    class LittleGameObject
    {
    public:
        using id_t = unsigned int;

        static LittleGameObject createGameObject()
        {
            static id_t currentId = 0;
            return LittleGameObject{currentId++};
        }

        LittleGameObject(const LittleGameObject &) = delete;
        LittleGameObject &operator=(const LittleGameObject &) = delete;
        LittleGameObject(LittleGameObject &&) = default;
        LittleGameObject &operator=(LittleGameObject &&) = default;

        id_t getId() { return id; }

        std::shared_ptr<LittleModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d;

    private:
        LittleGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
}