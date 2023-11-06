using SyEngine.Core.Datas;

namespace SyEngine.Core.Comps
{
public struct TransformComp
{
    public SyVector3 Position { get; internal set; }
    public SyVector3 Rotation { get; internal set; }
    public SyVector3 Scale    { get; internal set; }

    public SyVector3 LocalPosition;
    public SyVector3 LocalRotation;
    public SyVector3 LocalScale;

    public int? ParentEnt;
    
    public override string ToString() => $"({Position}, {Rotation}, {Scale})";
}
}
