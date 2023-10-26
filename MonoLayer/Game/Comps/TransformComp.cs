using System;
using System.Runtime.InteropServices;
using Leopotam.EcsLite;
using SyEngine.Game.Datas;

namespace SyEngine.Game.Comps
{
[StructLayout(LayoutKind.Sequential)]
public struct TransformComp
{
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Position;
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Rotation;
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Scale;
}
}
