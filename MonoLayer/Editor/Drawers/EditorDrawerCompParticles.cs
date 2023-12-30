using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;

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
		isChanged |= DrawFloatBasedInput("Start Life Time", ref val.StartLifeTime);
		isChanged |= DrawFloatBasedInput("Start Speed", ref val.StartSpeed);
		isChanged |= DrawFloatBasedInput("Start Size", ref val.StartSize);
		isChanged |= DrawFloatBasedInput("Start Rotation", ref val.StartRotation);
		
		isChanged |= DrawColorBasedInput("Start Color", ref val.StartColor);
		
		isChanged |= DrawCurveBasedInput("Size Over Lifetime", ref val.SizeOverLifetime);
		isChanged |= DrawCurveBasedInput("Speed Over Lifetime", ref val.SpeedOverLifetime);
		isChanged |= DrawFloatBasedInput("Rotation Over Lifetime", ref val.RotationOverLifetime);
		isChanged |= DrawFloatBasedInput("Rate Over Lifetime", ref val.RateOverTime);

		if (SyProxyEditor.GeDrawFoldout("Light"))
		{
			isChanged |= Editor.DrawField("Is Lit", ref val.IsLit);
			isChanged |= Editor.DrawField("Ambient Amount", ref val.AmbientAmount);
		}
		
		if (SyProxyEditor.GeDrawFoldout("Emission"))
		{
			isChanged |= Editor.DrawField("Max Particles", ref val.MaxParticles);
			isChanged |= Editor.DrawField("Bursts", ref val.Bursts);
			isChanged |= Editor.DrawField("Emit Shape", ref val.EmitShape);
			isChanged |= Editor.DrawField("Angle", ref val.Angle);
			isChanged |= Editor.DrawField("Radius", ref val.Radius);
		}

		isChanged |= Editor.DrawField("Texture", ref val.Texture);
		
		if (isChanged)
			val.IsDirty = true;
		
		return isChanged;
	}


	private bool DrawFloatBasedInput(string name, ref ParticlesComp.InputData input)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			bool isRandom = input.InputType == ParticlesComp.InputData.EInputType.RandomBetweenFloats;
			isChanged |= Editor.DrawField("Is Random", ref isRandom);
			input.InputType = isRandom
				? ParticlesComp.InputData.EInputType.RandomBetweenFloats
				: ParticlesComp.InputData.EInputType.SimpleFloat;

			if (isRandom)
				isChanged |= Editor.DrawField("Range", ref input.RandomBetweenConstsF);
			else
				isChanged |= Editor.DrawField("Value", ref input.F);
		}
		return isChanged;
	}

	private bool DrawColorBasedInput(string name, ref ParticlesComp.InputData input)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			bool isRandom = input.InputType == ParticlesComp.InputData.EInputType.RandomBetweenVectors;
			isChanged |= Editor.DrawField("Is Random", ref isRandom);
			input.InputType = isRandom
				? ParticlesComp.InputData.EInputType.RandomBetweenVectors
				: ParticlesComp.InputData.EInputType.SimpleVector;

			if (isRandom)
			{
				var colorA = new SyColor(input.RandomBetweenConstsV.A);
				var colorB = new SyColor(input.RandomBetweenConstsV.B);
				isChanged |= Editor.DrawField("Range.A", ref colorA);
				isChanged |= Editor.DrawField("Range.B", ref colorB);

				if (isChanged)
					input.RandomBetweenConstsV = new SyVector4Pair(colorA, colorB);
			}
			else
			{
				var color = new SyColor(input.V);
				isChanged |= Editor.DrawField("Value", ref color);
				if (isChanged)
					input.V = color;
			}
		}
		return isChanged;
	}
	
	private bool DrawVectorBasedInput(string name, ref ParticlesComp.InputData input)
	{
		var isChanged = false;
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			bool isRandom = input.InputType == ParticlesComp.InputData.EInputType.RandomBetweenVectors;
			isChanged |= Editor.DrawField("Is Random", ref isRandom);
			input.InputType = isRandom
				? ParticlesComp.InputData.EInputType.RandomBetweenVectors
				: ParticlesComp.InputData.EInputType.SimpleVector;

			if (isRandom)
				isChanged |= Editor.DrawField("Range", ref input.RandomBetweenConstsV);
			else
				isChanged |= Editor.DrawField("Value", ref input.V);
		}
		return isChanged;
	}

	private bool DrawCurveBasedInput(string name, ref ParticlesComp.InputData input)
	{
		if (SyProxyEditor.GeDrawFoldout(name))
		{
			return Editor.DrawField(name, ref input.Curve);
		}
		return false;
	}
}
}