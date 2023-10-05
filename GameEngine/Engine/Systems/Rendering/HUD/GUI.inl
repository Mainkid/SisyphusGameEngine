

//template <typename T>
//void GUI::DrawInputField(T* var, const char* name, int numberOfElements)
//{
//	switch (numberOfElements)
//	{
//	case 1:
//		{	// улучшить
//			float vec1[1]{var->x};
//			ImGui::DragFloat(name, vec1, 0.1f);
//			var->x = vec1[0];
//			break;
//		}
//
//	case 2:
//		{
//			float vec2[2]{ var->x, var->y};
//			ImGui::DragFloat2(name, vec2, 0.1f);
//			var->x = vec2[0];
//			var->y = vec2[1];
//			break;
//		}
//		
//
//	case 3:
//		{	// для SyVector3
//			float vec3[3]{ var->x, var->y, var->z };
//			ImGui::DragFloat3(name, vec3, 0.1f);
//			var->x = vec3[0];
//			var->y = vec3[1];
//			var->z = vec3[2];
//			break;
//		}
//		
//	case 4:
//		{//что-то тут он не хочет
//			//float vec4[4]{ var->x, var->y, var->z, var->w };
//			//ImGui::DragFloat4(name, vec4, 0.1f);
//			//var->x = vec4[0];
//			//var->y = vec4[1];
//			//var->z = vec4[2];
//			//var->w = vec4[4];
//			break;
//		}
//		//ImGui::DragFloat4(name, var, 0.1f);
//
//	default:
//		// тут надо ошибку вызывать?
//		break;
//	}
//}


