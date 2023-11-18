using SyEngine.Core.Datas;

namespace SyEngine.Core.ProxyComps
{
internal struct ProxyTransformComp
{
    public SyVector3 Position;
    public SyVector3 Rotation;
    public SyVector3 Scale;
    public SyVector3 LocalPosition;
    public SyVector3 LocalRotation;
    public SyVector3 LocalScale;

    public bool HasParent;
    public uint ParentEngineEnt;
}
}
