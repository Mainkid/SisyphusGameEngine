using SyEngine.Input;

namespace SyEngine.Ecs
{
    public abstract class SyEcsSystemBase
    {
        protected SyEcs   Ecs   { get; private set; }
        protected SyProxyInput Input { get; private set; }

        public void Attach(SyEcs ecs, SyProxyInput input)
        {
            Ecs = ecs;
        }

        public abstract void Init();
        public abstract void Run();
        public abstract void Destroy();
    }
}
