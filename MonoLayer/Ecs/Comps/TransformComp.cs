using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct TransformComp : SyEcs.IInternalComp
{
	public SyVector3 Position;
    public SyVector3 Rotation;
    public SyVector3 Scale;

    public SyVector3 LocalPosition;
    public SyVector3 LocalRotation;
    public SyVector3 LocalScale;

    public SySceneEnt? Parent;


    [Hide, NonSerialized]
    internal int? Hash;
    
    public override string ToString() => $"({Position}, {Rotation}, {Scale})";

    public override int GetHashCode()
	    => HashHelper.Combine(Position,Rotation,Scale,LocalPosition, LocalRotation, LocalScale);
}
}