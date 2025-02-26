#pragma once

#include "Core.h"

#include "CoreRender/Shader.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "ECS/Components/Particle.h"
#include "Misc/BasicOrthoCameraObj.h"

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

namespace Renderer2d
{
void init(const OrthoCameraEntity &cameraEntity);
// ================================================================= //
// Renderer basic wrapper around opengl
// ================================================================= //

void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
void beginScene(float globalTime, const glm::mat4 &transform = glm::mat4(1.0f));
void endScene();
void shutdown();
void setViewport(int x, int y, int width, int height);
void setClearColor(const glm::vec4 &color);
void clear();
void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                 uint32_t indexCount = 0);

// ================================================================= //
// Draw Quads
// ================================================================= //

/** Draws a quad */
void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
              const glm::vec4 &tintColor, Texture *texture = nullptr,
              const float tilingFactor = 1.0f,
              const std::array<glm::vec2, 4> &textureCoordinate = {
                  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
              const glm::vec4 &tintColor, Texture &texture,
              const float tilingFactor = 1.0f,
              const std::array<glm::vec2, 4> &textureCoordinate = {
                  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

/** Draws a quad with rotation, new rotationRadians (angle)*/
void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
              const glm::vec4 &tintColor, const float rotationRadians,
              Texture *texture = nullptr, const float tilingFactor = 1.0f,
              const std::array<glm::vec2, 4> &textureCoordinate = {
                  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

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
void drawString(const glm::vec2 &position, const char *string, const Font &font,
                const glm::vec4 &color);

const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                             const float rotationRadians);
const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size);

void removeTexture(const Texture &texture);

// void flush();
// // ================================================================= //
// // Texture Data
// // ================================================================= //
// void initBatches();
// void bindTextures();
// float allocateTextures(Texture &texture);
// // ================================================================= //
// // Data for Renderer
// // ================================================================= //
// void allocateQuad(const glm::mat4 &transform, const glm::vec4 &tintColor,
//                   const float tilingFactor, const float textureIndex,
//                   const std::array<glm::vec2, 4> &textureCoordinate);
// void allocateTri(const glm::mat4 &transform, const glm::vec4 &tintColor);
// void allocateSprayParticles(const glm::vec2 &position, const glm::vec2
// &offset,
//                             const glm::vec2 &normal, const float startTime,
//                             const float rotationSpeed);
// void allocateCharacter(const glm::mat4 &transform, const glm::vec4
// &tintColor,
//                        const std::array<glm::vec2, 4> &textureCoordinate,
//                        const std::array<glm::vec4, 4> &textVertexPositions);

// void goBackToFirstVertex();
// void drawBatches(const glm::mat4 &viewProjectionMatrix);
// void sendAllDataToOpenGL();

// constexpr uint32_t MaxQuads = 2000;
// constexpr uint32_t MaxQuadVertices = MaxQuads * 4;
// constexpr uint32_t MaxQuadIndices = MaxQuads * 6;
// std::shared_ptr<VertexArray> m_quadVertexArray;
// std::shared_ptr<VertexBuffer> m_quadVertexBuffer;
// std::shared_ptr<Shader> m_quadTextureShader;
// QuadVertex *m_quadVertexBufferBase;
// QuadVertex *m_quadVertexBufferPtr;
// uint32_t m_quadIndexCount;

// constexpr uint32_t MaxTri = 2000;
// constexpr uint32_t MaxTriVertices = MaxTri * 3;
// constexpr uint32_t MaxTriIndices = MaxTri * 3;
// std::shared_ptr<VertexArray> m_triVertexArray;
// std::shared_ptr<VertexBuffer> m_triVertexBuffer;
// std::shared_ptr<Shader> m_triShader;
// TriVertex *m_triVertexBufferBase;
// TriVertex *m_triVertexBufferPtr;
// uint32_t m_triIndexCount;

// std::shared_ptr<VertexArray> m_textVertexArray;
// std::shared_ptr<VertexBuffer> m_textVertexBuffer;
// std::shared_ptr<Shader> m_textTextureShader;
// TextQuadVertex *m_textVertexBufferBase;
// TextQuadVertex *m_textVertexBufferPtr;
// uint32_t m_textIndexCount;

// constexpr uint32_t MaxSprayParticles = 10000;
// constexpr uint32_t MaxSprayVertices = MaxSprayParticles * 4;
// constexpr uint32_t MaxSprayIndices = MaxSprayParticles * 6;
// std::shared_ptr<VertexArray> m_sprayVertexArray;
// std::shared_ptr<VertexBuffer> m_sprayVertexBuffer;
// std::shared_ptr<Shader> m_sprayShader;
// ParticleVertex *m_sprayVertexBufferBase;
// ParticleVertex *m_sprayVertexBufferPtr;
// uint32_t m_sprayIndexCount;
// const Texture *m_fontAtlasTexture;

// // TODO:(jao) search MaxTextureSlots dinamically (i.e TMU value on gpu)
// constexpr uint32_t MaxTextureSlots = 32;

// std::shared_ptr<Texture> m_whiteTexture;
// std::array<Texture *, MaxTextureSlots> m_textureSlots;

// uint32_t m_textureSlotIndex;
// // clang-format off
//   constexpr  glm::vec4 m_quadVertexPositions[4] = {
//       glm::vec4(-0.5f,-0.5f,0.f,1.f),
//       glm::vec4( 0.5f,-0.5f,0.f,1.f),
//       glm::vec4( 0.5f, 0.5f,0.f,1.f),
//       glm::vec4(-0.5f, 0.5f,0.f,1.f),
//   };
//   constexpr  glm::vec4 m_triVertexPositions[3] = {
//       glm::vec4( 0.0f, 0.5f,0.f,1.f),
//       glm::vec4( 0.5f,-0.5f,0.f,1.f),
//       glm::vec4(-0.5f,-0.5f,0.f,1.f),
//   };
//   constexpr  glm::vec2 m_sprayVertexPositions[4] = {
//       glm::vec2(-0.5f,-0.5f),
//       glm::vec2( 0.5f,-0.5f),
//       glm::vec2( 0.5f, 0.5f),
//       glm::vec2(-0.5f, 0.5f),
//   };
}; // namespace Renderer2d

} // namespace pain
