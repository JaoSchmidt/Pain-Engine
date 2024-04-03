#pragma once
#include "pch.h"

#include "CoreFiles/Layer.h"

namespace pain
{
/* Eu nÃ£o sei pq o TheCherno criou essa classe ao invÃ©s de usar
 * um wrapper para o vetor
 * Mas vou confiar nele por enquanto
 * */
class LayerStack
{
public:
  LayerStack();
  ~LayerStack();

  void pushLayer(Layer *layer);
  void popLayer(Layer *layer);
  std::vector<Layer *>::iterator begin();
  std::vector<Layer *>::iterator end();

private:
  std::vector<Layer *> m_layers;
  std::vector<Layer *>::iterator m_layerInsert;
};
} // namespace pain
