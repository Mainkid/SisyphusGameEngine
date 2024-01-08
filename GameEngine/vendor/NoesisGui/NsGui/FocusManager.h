////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_FOCUSMANAGER_H__
#define __GUI_FOCUSMANAGER_H__


#include <NsCore/Noesis.h>
#include <NsGui/CoreApi.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsCore/BaseComponent.h>


namespace Noesis
{

class DependencyObject;
class DependencyProperty;
class RoutedEvent;
class UIElement;
struct RoutedEventArgs;
template<class T> class Delegate;
typedef Delegate<void (BaseComponent*, const RoutedEventArgs&)> RoutedEventHandler;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Provides a set of static methods, attached properties, and events for determining and setting
/// focus scopes and for setting the focused element within the scope.
///
/// There are two concepts concerning focus: keyboard focus and logical focus.
///
/// Keyboard focus pertains to the element that is currently receiving keyboard input. There can be
/// only one element with keyboard focus. This element with keyboard focus has *IsKeyboardFocused*
/// set to true. *Keyboard.FocusedElement* returns the element with keyboard focus.
///
/// Logical focus pertains to the *FocusManager.FocusedElement* within a specific focus scope.
///
/// There can be multiple elements with logical focus, but there can only be one element with
/// logical focus within a single focus scope. An element with logical focus does not necessarily
/// have keyboard focus, but an element with keyboard focus will have logical focus. It is possible
/// to define a focus scope within a focus scope. In this case, both the parent focus scope and the
/// child focus scope can have a *FocusManager.FocusedElement*.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.input.focusmanager.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CORE_API FocusManager
{
public:
    /// Gets the element with logical focus within the specified focus scope. Attached property
    static UIElement* GetFocusedElement(DependencyObject* element);

    /// Sets the element with logical focus within the specified focus scope. Attached property
    static void SetFocusedElement(DependencyObject* element, UIElement* value);

    /// Determines whether the specified DependencyObject is a focus scope. Attached property
    static bool GetIsFocusScope(DependencyObject* element);

    /// Specifies if the given DependencyObject is a focus scope. Attached property
    static void SetIsFocusScope(DependencyObject* element, bool value);

    /// Determines the closest ancestor of the specified element that has IsFocusScope set to true
    static DependencyObject* GetFocusScope(DependencyObject* element);

    /// Adds a handler for the GotFocus attached event
    static void AddGotFocusHandler(UIElement* element, const RoutedEventHandler& handler);

    /// Removes a handler for the GotFocus attached event
    static void RemoveGotFocusHandler(UIElement* element, const RoutedEventHandler& handler);

    /// Adds a handler for the LostFocus attached event
    static void AddLostFocusHandler(UIElement* element, const RoutedEventHandler& handler);

    /// Removes a handler for the LostFocus attached event
    static void RemoveLostFocusHandler(UIElement* element, const RoutedEventHandler& handler);

public:
    /// Dependency properties
    //@{
    static const DependencyProperty* FocusedElementProperty;
    static const DependencyProperty* IsFocusScopeProperty;
    //@}

    /// Routed events
    //@{
    static const RoutedEvent* LostFocusEvent;
    static const RoutedEvent* GotFocusEvent;
    //@}

    NS_DECLARE_REFLECTION(FocusManager, NoParent)
};

}

#endif
