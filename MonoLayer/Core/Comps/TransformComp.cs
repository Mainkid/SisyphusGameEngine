using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct TransformComp : SyEcs.IComp
{
    public SyVector3 Position { get; internal set; }
    public SyVector3 Rotation { get; internal set; }
    public SyVector3 Scale    { get; internal set; }

    public SyVector3 LocalPosition;
    public SyVector3 LocalRotation;
    public SyVector3 LocalScale;

    public int? ParentEnt;


    internal int Hash;
    
    public override string ToString() => $"({Position}, {Rotation}, {Scale})";

    public override int GetHashCode()
	    => HashHelper.Combine(LocalPosition, LocalRotation, LocalScale);
}
}