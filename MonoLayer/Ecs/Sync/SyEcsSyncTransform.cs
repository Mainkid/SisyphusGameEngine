using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncTransform : SyEcsSyncBase<TransformComp, ProxyTransformComp>
{
	public SyEcsSyncTransform(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Transform;

	protected override void SendImpl(uint engineEnt, ref TransformComp tf)
	{
		uint engineParentEnt = default;
		bool hasParent = tf.Parent?.InternalEnt != null &&
		                 Ecs.ToEngineEnt(tf.Parent.Value.InternalEnt.Value, out engineParentEnt);

		var proxy = new ProxyTransformComp
		{
			Position        = tf.Position,
			Rotation        = tf.Rotation,
			Scale           = tf.Scale,
			LocalPosition   = tf.LocalPosition,
			LocalRotation   = tf.LocalRotation,
			LocalScale      = tf.LocalScale,
			HasParent       = hasParent,
			ParentEngineEnt = engineParentEnt
		};
		SyProxyEcs.GeUpdateTransformComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyTransformComp proxy, ref TransformComp tf)
	{
		tf.Position      = proxy.Position;
		tf.Rotation      = proxy.Rotation;
		tf.Scale         = proxy.Scale;
		tf.LocalPosition = proxy.LocalPosition;
		tf.LocalRotation = proxy.LocalRotation;
		tf.LocalScale    = proxy.LocalScale;

		if (proxy.HasParent && Ecs.ToGameEnt(proxy.ParentEngineEnt, out int parentEnt))
			tf.Parent = new SySceneEnt(parentEnt);
		else
			tf.Parent = null;
	}

	protected override int? GetHashImpl(ref TransformComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref TransformComp comp, int hash)
		=> comp.Hash = hash;
}

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