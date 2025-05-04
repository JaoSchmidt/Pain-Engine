module pain.InitializeScript;

namespace pain
{
void initializeScript(Scene *scene, NativeScriptComponent &nsc, int e)
{
  if (!nsc.instance) {
    nsc.instantiateFunction(nsc.instance);
    nsc.instance->m_scene = scene;
    nsc.instance->m_entity = e;

    if (nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
}

} // namespace pain
