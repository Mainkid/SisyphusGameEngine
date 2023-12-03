using System;
using System.Collections.Generic;
using System.IO;
using Leopotam.EcsLite;
using SyEngine.Core;
using SyEngine.Logger;
using SyEngine.Serialization;

namespace SyEngine.Game
{
public class SyScene
{
	private readonly SyEcs _ecs;
	
	public SyScene(SyEcs ecs)
	{
		_ecs = ecs;
	}


	public void Save()
	{
		int poolsCount = _ecs.World.GetPoolsCount();
		var pools      = new IEcsPool[poolsCount];
		_ecs.World.GetAllPools(ref pools);

		int entitiesCount = _ecs.World.GetEntitiesCount();
		var entities      = new int[entitiesCount];
		_ecs.World.GetAllEntities(ref entities);

		var sceneDto = new SceneDto
		{
			Entities = new List<EntityDto>()
		};
		
		foreach (int ent in entities)
		{
			var entDto = new EntityDto
			{
				Entity = ent,
				Comps  = new List<CompDto>()
			};
			foreach (var pool in pools)
				if (pool.Has(ent))
				{
					var compDto = new CompDto
					{
						TypeFullName = pool.GetComponentType().FullName,
						Comp         = pool.GetRaw(ent)
					};
					entDto.Comps.Add(compDto);
				}

			if (entDto.Comps.Count > 0)
				sceneDto.Entities.Add(entDto);
		}

		string json = SerializeHelper.ToJson(sceneDto);
		File.WriteAllText("test_scene.json", json);
	}
	
	
	
	public void Load()
	{
		if (!File.Exists("test_scene.json"))
			return;
		
		string json = File.ReadAllText("test_scene.json");
		
		var sceneDto = SerializeHelper.FromJson<SceneDto>(json);
		if (sceneDto == null)
		{
			SyLog.Err(ELogTag.Scene, "failed to load scene; sceneDto is null");
			return;
		}
		if (sceneDto.Entities == null)
		{
			SyLog.Err(ELogTag.Scene, "failed to load scene; sceneDto::Entities is null");
			return;
		}
		
		foreach (var entDto in sceneDto.Entities)
		{
			if (entDto.Comps == null)
			{
				SyLog.Err(ELogTag.Scene, $"failed to load ent {entDto.Entity}; Comps is null");
				continue;
			}
			
			int ent = _ecs.CreateEntity();
			foreach (var compDto in entDto.Comps)
			{
				var type = Type.GetType(compDto.TypeFullName);
				if (type == null)
				{
					SyLog.Err(ELogTag.Scene, $"failed to load comp type {compDto.TypeFullName}; type not found");
					continue;
				}
				if (compDto.Comp == null)
				{
					SyLog.Err(ELogTag.Scene, $"failed to load comp {compDto.TypeFullName}; body is null");
					continue;
				}
				_ecs.AddCompRaw(type, ent);
				_ecs.World.GetPoolByType(type).SetRaw(ent, compDto.Comp);
			}
		}
	}


	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private class SceneDto
	{
		public List<EntityDto> Entities;
	}
	
	private class EntityDto
	{
		public int           Entity;
		public List<CompDto> Comps;
	}

	private class CompDto
	{
		public string TypeFullName;
		public object Comp;
	}
}
}