
////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_LINEBREAK_H__
#define __GUI_LINEBREAK_H__


#include <NsCore/Noesis.h>
#include <NsGui/Inline.h>
#include <NsCore/ReflectionDeclare.h>


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// An inline flow content element that causes a line break to occur in flow content.
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CORE_API LineBreak : public Inline
{
    NS_DECLARE_REFLECTION(LineBreak, Inline)
};

}


#endif
