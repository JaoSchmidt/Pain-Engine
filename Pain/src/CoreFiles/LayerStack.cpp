#include "LayerStack.h"
#include <algorithm>

namespace pain
{
LayerStack::LayerStack() { m_layerInsert = m_layers.begin(); }

LayerStack::~LayerStack()
{
  for (Layer *layer : m_layers)
    delete layer;
}

void LayerStack::pushLayer(Layer *layer)
{
  m_layerInsert = m_layers.emplace(m_layerInsert, layer);
}

void LayerStack::popLayer(Layer *layer)
{
  auto it = std::find(m_layers.begin(), m_layers.end(), layer);
  if (it != m_layers.end()) {
    m_layers.erase(it);
    m_layerInsert--;
  }
}
std::vector<Layer *>::iterator LayerStack::begin() { return m_layers.begin(); }
std::vector<Layer *>::iterator LayerStack::end() { return m_layers.end(); }
} // namespace pain
