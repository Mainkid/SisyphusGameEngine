#pragma once

#define SY_GET_ENTITY_NAME_CHAR(ecsPtr, entity) ecsPtr->get<GameObjectComp>(entity).Name.c_str()

//set SyResult variable to {SY_RESCODE_ERROR, message}, log error message to PHYS channel
#define SY_PROCESS_PHYS_ERROR(result, logMessage, ...) result.code = SY_RESCODE_ERROR; \
                                                    result.message = xstring(logMessage,__VA_ARGS__); \
                                                    SY_LOG_PHYS(SY_LOGLEVEL_ERROR, logMessage, __VA_ARGS__) \
