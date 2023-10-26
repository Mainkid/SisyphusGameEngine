namespace SyEngine.Game
{
    public abstract class SyEcsSystemBase
    {
        protected SyEcs Ecs { get; private set; }

        public void Attach(SyEcs ecs)
        {
            Ecs = ecs;
        }

        public abstract void Init();
        public abstract void Run();
        public abstract void Destroy();
    }
}
