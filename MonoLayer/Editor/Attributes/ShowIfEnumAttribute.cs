using System;
using System.Collections.Generic;
using System.Reflection;
using SyEngine.Logger;

namespace SyEngine.Editor.Attributes
{
public class ShowIfEnumAttribute : ShowIfBaseAttribute
{
	private readonly string _fieldName;
	private readonly int    _targ;
    
	public ShowIfEnumAttribute(string fieldName, int targ)
	{
		_fieldName = fieldName;
		_targ   = targ;
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private FieldInfo _predField;

	protected override void InitImpl()
	{
		_predField = FindField(_fieldName);

		if (_predField == null)
			SyLog.Err(ELogTag.Editor, $"failed to find field {_fieldName}");
	}

	protected override bool CheckPredicate(object holder)
	{
		if (_predField == null)
			return false;
		object rawCurrPred = _predField.GetValue(holder);
		var    currPred    = (int)rawCurrPred;
		return currPred == _targ;
	}
}
}