using System.Reflection;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerReflect<T> : EditorDrawerBase<T>
{
	private readonly SyProxyEditor _editor;
	private readonly FieldInfo[]   _fields;
	
	public EditorDrawerReflect(SyProxyEditor editor) : base(editor)
	{
		_editor = editor;
		_fields = typeof(T).GetFields(BindingFlags.Instance | BindingFlags.Public);
	}
    
	public override bool Draw(string name, ref T val)
	{
		object rawVal    = val;
		bool   isChanged = DrawRaw(name, ref rawVal);
		val = (T)rawVal;
		return isChanged;
	}
	
	public override bool DrawRaw(string name, ref object rawVal)
	{
		var isObjChanged = false;
		foreach (var field in _fields)
		{
			object fieldVal       = field.GetValue(rawVal);
			bool   isFieldChanged = _editor.DrawField(field.Name, field.FieldType, ref fieldVal);
			if (isFieldChanged)
			{
				field.SetValue(rawVal, fieldVal);
				isObjChanged = true;
			}
		}
		return isObjChanged;
	}
}
}