using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using SyEngine.Datas;

namespace SyEngine.Input
{
public class SyProxyInput
{
	public bool IsPressed(EKey key) => GeIsKeyPressed((int)key);

	public bool IsReleased(EKey key) => GeIsKeyReleased((int)key);

	public bool IsDown(EKey key) => GeIsKeyDown((int)key);

	public SyVector2 MousePos
	{
		get
		{
			var pos = new SyVector2();
            GeGetMousePos(ref pos);
            return pos;
		}
	}

	public SyVector2 MousePosDelta
	{
		get
		{
			var pos = new SyVector2();
			GeGetMousePosDelta(ref pos);
			return pos;
		}
	}

	public float MouseWheel => GeGetMouseWheel();


	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern bool GeIsKeyPressed(int key);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern bool GeIsKeyReleased(int key);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern bool GeIsKeyDown(int key);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void GeGetMousePos(ref SyVector2 pos);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void GeGetMousePosDelta(ref SyVector2 pos);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern float GeGetMouseWheel();
}
}