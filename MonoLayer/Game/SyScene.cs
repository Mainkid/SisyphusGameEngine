using System;
using System.Collections.Generic;
using System.IO;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;
using LeoEcs;
using SimpleJSON;
using SyEngine.Logger;
using SyEngine.Serialization;

namespace SyEngine.Game
{
public class SyScene
{
	private readonly SyEcs _ecs;

	private readonly Dictionary<int, int> _oldEntToNewEnt = new Dictionary<int, int>();
	
	public SyScene(SyEcs ecs)
	{
		_ecs = ecs;
	}


	public void Save()
	{
		int poolsCount = _ecs.World.GetPoolsCount();
		var pools      = new IEcsPool[poolsCount];
		_ecs.World.GetAllPools(ref pools);

		var filter = _ecs.World.Filter<SceneObjectComp>().End();

		var entities     = new List<int>();
		var entitiesDtos = new List<EntityDto>();

		foreach (int ent in filter)
		{
			entities.Add(ent);

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
						TypeFullName = pool.GetComponentType().AssemblyQualifiedName,
						Comp         = pool.GetRaw(ent)
					};
					entDto.Comps.Add(compDto);
				}
            
			entitiesDtos.Add(entDto);
		}

		var sceneDto = new SceneDto();
		sceneDto.Entities     = SerializeHelper.ToJsonNode(entities);
		sceneDto.EntitiesDtos = SerializeHelper.ToJsonNode(entitiesDtos);

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
			SyLog.Err(ELogTag.Scene, "failed to load scene; scene dto is null");
			return;
		}

		var entities = SerializeHelper.FromJsonNode<List<int>>(sceneDto.Entities);
		if (entities == null)
		{
			SyLog.Err(ELogTag.Scene, "failed to load scene; entities is invalid");
			return;
		}
		_oldEntToNewEnt.Clear();
		foreach (int entity in entities)
		{
			int ent = _ecs.CreateEntity(true);
			_oldEntToNewEnt[entity] = ent;
		}
		SerializeHelper.SetContext(_oldEntToNewEnt);

		var entitiesDtos = SerializeHelper.FromJsonNode<List<EntityDto>>(sceneDto.EntitiesDtos);
		if (entitiesDtos == null)
		{
			SyLog.Err(ELogTag.Scene, "failed to load scene; entities dtos is invalid");
			return;
		}
		foreach (var entDto in entitiesDtos)
		{
			if (entDto.Comps == null)
			{
				SyLog.Err(ELogTag.Scene, $"failed to load ent {entDto.Entity}; Comps is null");
				continue;
			}

			int ent = _oldEntToNewEnt[entDto.Entity];
			
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
				
				if (type != typeof(SceneObjectComp) &&
				    type != typeof(TransformComp))
					_ecs.AddCompRaw(type, ent);
				
				_ecs.World.GetPoolByType(type).SetRaw(ent, compDto.Comp);
			}
		}
	}


	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private class SceneDto
	{
		public JSONNode Entities;
		public JSONNode EntitiesDtos;
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