using System;

namespace SyEngine.Ecs.Sync
{
internal interface ISyEcsSync
{
	void TrySendAll();

	Type ProxyType { get; }
}

internal interface ISyEcsSync<TProxy> : ISyEcsSync
{
	void Receive(uint engineEnt, ref TProxy proxy);
}
}