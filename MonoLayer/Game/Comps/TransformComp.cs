using SyEngine.Game.Datas;

namespace SyEngine.Game.Comps
{
//[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct TransformComp
{
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Position;
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Rotation;
    //[MarshalAs(UnmanagedType.Struct)]
    public SyVector3 Scale;
    
    public override string ToString() => $"({Position}, {Rotation}, {Scale})";
}
}
