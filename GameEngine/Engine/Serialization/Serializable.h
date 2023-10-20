#pragma once

#include "SerializableMacros.h"
#include "SerializableOuters.h"

/* Important!!! Include this header into any place where you use following macros.
 *
 * Put SER_COMP inside component and enumerate component Type and Field Names to serialize.
 * Ex. SER_COMP(CompType, FieldA, FieldB...)
 *
 * Put SER_DATA inside class/struct and enumerate its Type and Field Names as in example above.
 *
 * In case third-party structs such as DirectX::XMVECTOR, put SER_DATA_OUTER in SerializableOuters.h.
 * SER_DATA_OUTER must be placed inside same namespace as serializable type.
 * See example of DirectX::SimpleMath::Vector4.
 */