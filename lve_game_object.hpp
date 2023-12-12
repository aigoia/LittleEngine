#pragma once

#include "lve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace lve {

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  // Matrix corresponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Trait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  glm::mat4 mat4()
  {
    const float cos3 = glm::cos(rotation.z);
    const float sin3 = glm::sin(rotation.z);
    const float cos2 = glm::cos(rotation.x);
    const float sin2 = glm::sin(rotation.x);
    const float cos1 = glm::cos(rotation.y);
    const float sin1 = glm::sin(rotation.y);
    
    return glm::mat4{
    {
      scale.x * (cos1 * cos3 + sin1 * sin2 * sin3),
      scale.x * (cos2 * sin3),
      scale.x * (cos1 * sin2 * sin3 - cos3 * sin1),
      0.0f,
    },
    {
      scale.y * (cos3 * sin1 * sin2 - cos1 * sin3),
      scale.y * (cos2 * cos3),
      scale.y * (cos1 * cos3 * sin2 + sin1 * sin3),
      0.0f,
    },
    {
      scale.z * (cos2 * sin1),
      scale.z * (-sin2),
      scale.z * (cos1 * cos2),
      0.0f,
    },
    {translation.x, translation.y, translation.z, 1.0f}};
  }
};

class LveGameObject {
  public:
  using id_t = unsigned int;

  static LveGameObject createGameObject() {
    static id_t currentId = 0;
    return LveGameObject{currentId++};
  }

  LveGameObject(const LveGameObject &) = delete;
  LveGameObject &operator=(const LveGameObject &) = delete;
  LveGameObject(LveGameObject &&) = default;
  LveGameObject &operator=(LveGameObject &&) = default;

  id_t getId() { return id; }

  std::shared_ptr<LveModel> model{};
  glm::vec3 color{};
  TransformComponent transform{};

  private:
  LveGameObject(id_t objId) : id{objId} {}

  id_t id;
};
}  // namespace lve
