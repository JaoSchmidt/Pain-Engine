#pragma once

#include "Core.h"

#include "CoreRender/Camera.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "ECS/Components/Particle.h"

namespace pain
{

struct TextQuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoord;
  // NOTE: (jao) you might want to implement `int texIndex` here but one texture
  // is more than fine for now
};

struct QuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
};

struct TriVertex {
  glm::vec3 position;
  glm::vec4 color;
};

struct ParticleVertex {
  glm::vec2 position;
  glm::vec2 offset;
  glm::vec2 normal;
  float time;
  float rotationSpeed;
};

struct Renderer2d {
  static Renderer2d createRenderer2d();
  Renderer2d &operator=(Renderer2d &&o) noexcept;
  ~Renderer2d();
  void changeCamera(const OrthographicMatrices &cameraMatrices);
  // ================================================================= //
  // Renderer basic wrapper around opengl
  // ================================================================= //

  void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  void beginScene(float globalTime,
                  const glm::mat4 &transform = glm::mat4(1.0f));
  void endScene();
  void setViewport(int x, int y, int width, int height);
  void setClearColor(const glm::vec4 &color);
  void clear();
  void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                   uint32_t indexCount = 0);
  void clearEntireRenderer();

  // ================================================================= //
  // Draw Quads
  // ================================================================= //

  /** Draws a quad */
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &tintColor, Texture &texture,
                const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  /** Draws a quad with rotation, new rotationRadians (angle)*/
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &tintColor, const float rotationRadians,
                Texture &texture, const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
  // ================================================================= //
  // Draw Tri
  // ================================================================= //

  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor);

  /** Draws triangles with rotation, new rotationRadians (angle)*/
  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor, const float rotationRadians);

  /** Draws a bunch of particles in a spray format */
  void beginSprayParticle(const float globalTime,
                          const ParticleSprayComponent &particleSprayComponent);
  void drawSprayParticle(const Particle &p);

  /** Draws a string of glyphs from a font atlas */
  void drawString(const glm::vec2 &position, const char *string,
                  const Font &font, const glm::vec4 &color);

  const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                               const float rotationRadians);
  const glm::mat4 getTransform(const glm::vec2 &position,
                               const glm::vec2 &size);

  void removeTexture(const Texture &texture);

private:
  // quads initializer
  VertexArray m_quadVertexArray;
  VertexBuffer m_quadVertexBuffer;
  Shader m_quadTextureShader;
  QuadVertex *m_quadVertexBufferBase = nullptr;
  QuadVertex *m_quadVertexBufferPtr = nullptr;
  uint32_t m_quadIndexCount = 0; // at init, there are 0 quads

  // text vertex initializer
  VertexArray m_textVertexArray;
  VertexBuffer m_textVertexBuffer;
  Shader m_textTextureShader;
  TextQuadVertex *m_textVertexBufferBase = nullptr;
  TextQuadVertex *m_textVertexBufferPtr = nullptr;
  uint32_t m_textIndexCount = 0; // at init, there are 0 texts

  // tri initializer
  VertexArray m_triVertexArray;
  VertexBuffer m_triVertexBuffer;
  Shader m_triShader;
  TriVertex *m_triVertexBufferBase = nullptr;
  TriVertex *m_triVertexBufferPtr = nullptr;
  uint32_t m_triIndexCount = 0; // at init, there are 0 tirangles

  // spray particle initializer
  VertexArray m_sprayVertexArray;
  VertexBuffer m_sprayVertexBuffer;
  Shader m_sprayShader;
  ParticleVertex *m_sprayVertexBufferBase = nullptr;
  ParticleVertex *m_sprayVertexBufferPtr = nullptr;
  uint32_t m_sprayIndexCount = 0;

  // texture initializer
  // // TODO:(jao) search MaxTextureSlots dinamically (i.e TMU value on gpu)
  static constexpr uint32_t MaxTextureSlots = 32;
  Texture *m_whiteTexture = nullptr;
  std::array<Texture *, MaxTextureSlots> m_textureSlots;
  uint32_t m_textureSlotIndex = 1; // at init, there is 1 white texture
  //
  IndexBuffer m_quadIB;
  IndexBuffer m_triIB;
  IndexBuffer m_sprayIB;

  const Texture *m_fontAtlasTexture = nullptr;
  const OrthographicMatrices *m_cameraMatrices = nullptr;

  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           float globalTime, const glm::mat4 &transform);
  void drawBatches(const glm::mat4 &viewProjectionMatrix);
  void bindTextures();
  void goBackToFirstVertex();
  float allocateTextures(Texture &texture);
  void allocateQuad(const glm::mat4 &transform, const glm::vec4 &tintColor,
                    const float tilingFactor, const float textureIndex,
                    const std::array<glm::vec2, 4> &textureCoordinate);
  void allocateTri(const glm::mat4 &transform, const glm::vec4 &tintColor);
  void allocateSprayParticles(const glm::vec2 &position,
                              const glm::vec2 &offset, const glm::vec2 &normal,
                              const float startTime, const float rotationSpeed);
  void allocateCharacter(const glm::mat4 &transform, const glm::vec4 &tintColor,
                         const std::array<glm::vec2, 4> &textureCoordinate,
                         const std::array<glm::vec4, 4> &textVertexPositions);
  Renderer2d(IndexBuffer quadIB,               //
             IndexBuffer triIB,                //
             IndexBuffer sprayIB,              //
             VertexArray quadVertexArray,      //
             VertexBuffer quadVertexBuffer,    //
             Shader quadTextureShader,         //
             QuadVertex *quadVertexBufferBase, //
             // text initializer
             VertexArray textVertexArray,          //
             VertexBuffer textVertexBuffer,        //
             Shader textTextureShader,             //
             TextQuadVertex *textVertexBufferBase, //
             // tri initializer
             VertexArray triVertexArray,     //
             VertexBuffer triVertexBuffer,   //
             Shader triShader,               //
             TriVertex *triVertexBufferBase, //
             // spray particle initializer
             VertexArray sprayVertexArray,   //
             VertexBuffer sprayVertexBuffer, //
             Shader sprayShader,             //
             ParticleVertex *sprayVertexBufferBase);
};

} // namespace pain
