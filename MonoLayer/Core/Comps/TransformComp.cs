﻿using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;
using SyEngine.Editor.Attributes;

namespace SyEngine.Core.Comps
{
public struct TransformComp : SyEcs.IInternalComp
{
	[Hide]
	public SyVector3 Position;
	[Hide]
    public SyVector3 Rotation;
    [Hide]
    public SyVector3 Scale;

    public SyVector3 LocalPosition;
    public SyVector3 LocalRotation;
    public SyVector3 LocalScale;

    public int? ParentEnt;


    internal int? Hash;
    
    public override string ToString() => $"({Position}, {Rotation}, {Scale})";

    public override int GetHashCode()
	    => HashHelper.Combine(LocalPosition, LocalRotation, LocalScale);
}
}