using SyEngine.Core;
using SyEngine.Core.Comps;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerCompParticles : EditorDrawerComp<ParticlesComp>
{
	public EditorDrawerCompParticles(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		
	}

	public override bool Draw(string name, ref ParticlesComp val)
	{
		var isChanged = false;

		isChanged |= Editor.DrawField("Duration", ref val.Duration);
		isChanged |= Editor.DrawField("Is Looping", ref val.IsLooping);

		isChanged |= DrawFloatBasedInput("Start Delay Time", ref val.StartDelayTime);
		//isChanged |= DrawFloatBasedInput("Start Life Time", ref val.StartLifeTime);
		//isChanged |= DrawFloatBasedInput("Start Speed", ref val.StartSpeed);
		//isChanged |= DrawFloatBasedInput("Start Size", ref val.StartSize);
		//isChanged |= DrawFloatBasedInput("Start Rotation", ref val.StartRotation);
		//
		//isChanged |= DrawVectorBasedInput("Start Color", ref val.StartColor);
		//
		//isChanged |= DrawCurveBasedInput("Size Over Lifetime", ref val.SizeOverLifetime);
		//isChanged |= DrawCurveBasedInput("Speed Over Lifetime", ref val.SpeedOverLifetime);
		//isChanged |= DrawFloatBasedInput("Rotation Over Lifetime", ref val.RotationOverLifetime);
		//isChanged |= DrawFloatBasedInput("Rate Over Lifetime", ref val.RateOverTime);
		//
		//isChanged |= Editor.DrawField("Max Particles", ref val.MaxParticles);
		//isChanged |= Editor.DrawField("Is Lit", ref val.IsLit);
		//isChanged |= Editor.DrawField("Ambient Amount", ref val.AmbientAmount);
		//
		//
		//isChanged |= Editor.DrawField("Emit Shape", ref val.ParticleEmitShape);
		//isChanged |= Editor.DrawField("Angle", ref val.Angle);
		//isChanged |= Editor.DrawField("Radius", ref val.Radius);

		if (isChanged)
			val.IsDirty = true;
		
		return isChanged;
	}


	private bool DrawFloatBasedInput(string name, ref ParticlesInputData input)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			bool isRandom = input.InputType == ParticlesInputData.EInputType.RandomBetweenFloats;
			isChanged |= Editor.DrawField("Is Random", ref isRandom);
			input.InputType = isRandom
				? ParticlesInputData.EInputType.RandomBetweenFloats
				: ParticlesInputData.EInputType.SimpleFloat;

			if (isRandom)
				isChanged |= Editor.DrawField("Range", ref input.RandomBetweenConstsF);
			else
				isChanged |= Editor.DrawField("Value", ref input.F);
		}
		return isChanged;
	}

	private bool DrawVectorBasedInput(string name, ref ParticlesInputData input)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			bool isRandom = input.InputType == ParticlesInputData.EInputType.RandomBetweenVectors;
			isChanged |= Editor.DrawField("Is Random", ref isRandom);
			input.InputType = isRandom
				? ParticlesInputData.EInputType.RandomBetweenVectors
				: ParticlesInputData.EInputType.SimpleVector;

			if (isRandom)
				isChanged |= Editor.DrawField("Range", ref input.RandomBetweenConstsV);
			else
				isChanged |= Editor.DrawField("Value", ref input.V);
		}
		return isChanged;
	}

	private bool DrawCurveBasedInput(string name, ref ParticlesInputData input)
	{
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			return Editor.DrawField(name, ref input.Curve);
		}
		return false;
	}
}
}