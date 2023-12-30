using SyEngine.Core.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerParticlesBurst : EditorDrawerBase<ParticlesComp.BurstData>
{
	public EditorDrawerParticlesBurst(SyProxyEditor editor) : base(editor) { }

	public override bool Draw(string name, ref ParticlesComp.BurstData val)
	{
		var isChanged = false;
		
		SyProxyEditor.GeDrawText(name);

		isChanged |= Editor.DrawField("Time", ref val.Time);
		isChanged |= Editor.DrawField("Time Since Last Burst", ref val.TimeSinceLastBurst);
		isChanged |= Editor.DrawField("Probability", ref val.Probability);
		isChanged |= Editor.DrawField("Is Looping", ref val.IsLooping);

		bool isRandomCount = val.Count.InputType == ParticlesComp.InputData.EInputType.RandomBetweenFloats;
		isChanged |= Editor.DrawField("Count.IsRandom", ref isRandomCount);
		if (isChanged)
			val.Count.InputType = isRandomCount
				? ParticlesComp.InputData.EInputType.RandomBetweenFloats
				: ParticlesComp.InputData.EInputType.SimpleFloat;

		if (isRandomCount)
		{
			isChanged |= Editor.DrawField("Count.Value", ref val.Count.F);
			if (isChanged)
				val.Count.F = (int)val.Count.F;
		}
		else
		{
			isChanged |= Editor.DrawField("Count.Range", ref val.Count.RandomBetweenConstsF);
			if (isChanged)
				val.Count.RandomBetweenConstsF = val.Count.RandomBetweenConstsF.FloorToInt();
		}

		return isChanged;
	}
}
}