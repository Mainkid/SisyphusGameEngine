////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_DEPENDSONMETADATA_H__
#define __GUI_DEPENDSONMETADATA_H__


#include <NsCore/Noesis.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsCore/TypeMetaData.h>
#include <NsCore/Symbol.h>
#include <NsGui/CoreApi.h>


namespace Noesis
{

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Indicates that the attributed property is dependent on the value of another property
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CORE_API DependsOnMetaData: public TypeMetaData
{
public:
    DependsOnMetaData(const char* propertyName);

    /// Gets content property
    Symbol GetDependsOnProperty() const;

private:
    Symbol mDependsOnProperty;

    NS_DECLARE_REFLECTION(DependsOnMetaData, TypeMetaData)
};

NS_WARNING_POP

}


#endif
