﻿using System;

namespace SyEngine.Core.Resources
{
public static class ResHelper
{
	public static EResourceType GetResType<T>() where T: ResBase
	{
		var type = typeof(T);
		if (type == typeof(ResModel))
			return EResourceType.Model;
		if (type == typeof(ResMaterial))
			return EResourceType.Material;

		throw new Exception($"unknown res type {type.FullName}");
	}
}
}