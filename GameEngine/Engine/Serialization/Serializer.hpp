#pragma once


#include "../../../vendor/entt/entt.hpp"
#include "../../../vendor/nlohmann_json/json.hpp"

#include "SerializableTraits.h"

namespace ser
{
	/**
     * \brief Works together with SER_COMP or SER_DATA macro.
     */
    class Serializer
    {
    public:
	    /**
         * \brief Allow T comp serialization.
         */
        template<typename T>
        void AddEcsCompMeta();

        /**
         * \brief Takes all entities with comp T and serialize all of them + all their comps.
         */
        template<typename T>
        nlohmann::json Serialize(entt::registry& ecs);

        /**
         * \brief Re-creates entities and their comps from json.
         */
        void Deserialize(const nlohmann::json& json, entt::registry& ecs);

        
	    /**
         * \brief Serialize class/struct with SER_DATA macro.
         */
        template<typename T>
        nlohmann::json Serialize(const T& data);

        /**
		 * \brief Deserialize class/struct with SER_DATA macro.
		 */
        template<typename T>
        void Deserialize(const nlohmann::json& json, T& outData);
        
	    /**
         * \brief Only for internal usage.
         */
        template<typename T>
        void SerializeField(const char* key, const T& val, nlohmann::json& outJson);

        /**
         * \brief Only for internal usage.
         */
        template<typename T>
        void DeserializeField(const nlohmann::json& json, const char* key, T& outVal);

    private:
        struct CompMeta;

        std::unordered_map<entt::id_type, CompMeta> _compTypeToMeta;
        std::unordered_map<std::string, CompMeta> _compNameToMeta;

        std::unordered_map<unsigned int, entt::entity> _contextEntityIdToEntity;


        void SerializeEntity(entt::registry& ecs, entt::entity ent, nlohmann::json& outJson);

        void DeserializeEntity(const nlohmann::json& json, entt::registry& ecs);


        template<typename T>
        constexpr void SerializeVal(const T& val, nlohmann::json& outJson);

        template<typename T>
        constexpr void DeserializeVal(const nlohmann::json& json, T& outVal);

        template<typename T>
        void SerializeVector(const std::vector<T>& vec, nlohmann::json& outJson);

        template<typename T>
        void DeserializeVector(const nlohmann::json& json, T& outVec);


        struct CompMeta
        {
            std::string Name;

            void (*FuncSerialize)(Serializer* sr, void* raw, nlohmann::json& outJson);

            void (*FuncDeserialize)(Serializer* sr, const nlohmann::json& json, entt::registry& ecs, entt::entity ent);
        };
    };


    template<typename T>
    void Serializer::AddEcsCompMeta()
    {
        CompMeta meta;
        meta.Name = typeid(T).name();
        meta.FuncSerialize = &T::Serialize;
        meta.FuncDeserialize = &T::Deserialize;

        _compTypeToMeta[entt::type_id<T>().index()] = meta;
        _compNameToMeta[meta.Name] = meta;
    }

    template<typename T>
    nlohmann::json Serializer::Serialize(entt::registry& ecs)
    {
        std::vector<nlohmann::json> jsEntities;

        auto view = ecs.view<T>();
        for (auto ent : view)
        {
            jsEntities.emplace_back();
            SerializeEntity(ecs, ent, jsEntities.back());
        }

        nlohmann::json result;
        result["entities"] = jsEntities;
        return result;
    }

    template <typename T>
    nlohmann::json Serializer::Serialize(const T& data)
    {
        nlohmann::json json;
        T::Serialize(this, data, json);
        return json;
    }

    template <typename T>
    void Serializer::Deserialize(const nlohmann::json& json, T& outData)
    {
        T::Deserialize(this, json, outData);
    }

    inline void Serializer::SerializeEntity(entt::registry& ecs, entt::entity ent, nlohmann::json& outJson)
    {
        outJson["id"] = static_cast<unsigned int>(ent);

        for (auto [typeId, storage] : ecs.storage())
        {
            auto ptr = storage.find(ent);
            if (ptr == storage.end())
                continue;

            auto ptrMeta = _compTypeToMeta.find(storage.type().index());
            if (ptrMeta == _compTypeToMeta.end())
                continue;

            CompMeta& meta = ptrMeta->second;

            void* val = storage.value(ent);
            meta.FuncSerialize(this, val, outJson[meta.Name]);
        }
    }

    inline void Serializer::Deserialize(const nlohmann::json& json, entt::registry& ecs)
    {
        _contextEntityIdToEntity.clear();

        auto jsEntities = json.at("entities").get<std::vector<nlohmann::json>>();

        for (auto& jsEntity : jsEntities)
        {
            auto entityId = jsEntity.at("id").get<unsigned int>();
            auto ent = ecs.create();
            _contextEntityIdToEntity[entityId] = ent;
        }

        for (auto& jsEntity : jsEntities)
        {
            DeserializeEntity(jsEntity, ecs);
        }
    }

    inline void Serializer::DeserializeEntity(const nlohmann::json& json, entt::registry& ecs)
    {
        auto entityId = json.at("id").get<unsigned int>();
        auto ent = _contextEntityIdToEntity[entityId];

        for (auto& item : json.items())
        {
            if (item.key() == "id")
                continue;

            auto ptrMeta = _compNameToMeta.find(item.key());
            if (ptrMeta == _compNameToMeta.end())
                continue;

            CompMeta& meta = ptrMeta->second;

            meta.FuncDeserialize(this, item.value(), ecs, ent);
        }
    }


    template<typename T>
    void Serializer::SerializeField(const char* key, const T& val, nlohmann::json& outJson)
    {
        SerializeVal(val, outJson[key]);
    }

    template<typename T>
    void Serializer::DeserializeField(const nlohmann::json& json, const char* key, T& outVal)
    {
        DeserializeVal(json.at(key), outVal);
    }


    template<typename T>
    constexpr void Serializer::SerializeVal(const T& val, nlohmann::json& outJson)
    {
        if constexpr (HasSerializableFunc<T>::value)
        {
            T::Serialize(this, val, outJson);
        }
        else if constexpr (IsCollection<T, std::vector>::value)
        {
            SerializeVector(val, outJson);
        }
        else
        {
            outJson = val;
        }
    }


    template<typename T>
    constexpr void Serializer::DeserializeVal(const nlohmann::json& json, T& outVal)
    {
        if constexpr (HasSerializableFunc<T>::value)
        {
            T::Deserialize(this, json, outVal);
        }
        else if constexpr (IsEntity<T>::value)
        {
            auto entityId = json.get<unsigned int>();
            outVal = _contextEntityIdToEntity[entityId];
        }
        else if constexpr (IsCollection<T, std::vector>::value)
        {
            DeserializeVector<T>(json, outVal);
        }
        else
            outVal = json.get<T>();
    }

    template<typename T>
    void Serializer::SerializeVector(const std::vector<T>& vec, nlohmann::json& outJson)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            outJson.emplace_back();
            SerializeVal(vec[i], outJson.back());
        }
    }

    template<typename T>
    void Serializer::DeserializeVector(const nlohmann::json& json, T& outVec)
    {
        for (int i = 0; i < json.size(); i++)
        {
            outVec.emplace_back();
            DeserializeVal(json[i], outVec.back());
        }
    }
}