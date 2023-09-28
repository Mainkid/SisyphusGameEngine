ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ
1. Для всех функций, которые имеют шанс некорректного выполнения, рекомендуется делать возвращаемым тип SyResult. 
   Если сама функция изначально не void, чтобы поулчить из нее значение, необходимо передать его арзументом по ссылке.
   Таким образом общий вид функции имеет вид SyResult Function(t_in1 in1, t_in2 in2_,..., t_out& out_)
2. Структуры SyResult определена в файле ErrorLogger.h и имеет поля:
	int code - код результата
		В качестве кода можно использовать один из макросов:
			SY_RESCODE_OK - в случае корректной работы функции
			SY_RESCODE_UNEXPECTED - в случае приемлемого, но неожиданного результата
			SY_RESCODE_ERROR - в случае ошибки в выполнении функции.
		Также можно определить свои коды в виде макроса. Желательно, чтобы их имя было таким: SY_RESCODE_ + (модуль, где определяется) + (имя)
	std::wstring message - сообщение пользователя
		Сообщение удобно просмотреть в debug через breakpoint. Чтобы перевести литерал из типа std::string в тип std::wstring, нужно дописать L перед кавычками.
3. После возврата значения SyResult можно предписать различное поведение в зависимости от того, является ли результат корректным или нет.
ЛОГИРОВАНИЕ ОШИБОК
1. Для использования логгера необходимо подключить заголовочный файл ErrorLogger.h. 
2. Для логирования используется функция SyErrorLogger::Log(const std::wstring& channelName_, SyElLogLevellogLevel_, const std::wstring& message_).
	Входные параметры:
		channelName_ - имя канала логирования
			Канал, в котором логируется сообщение соответствует определяемой пользователем области/ модуле, где произошло логирование.
			По умолчанию созданы каналы
				CORE - главные модули движка
				REND - рендер-система
				PHYS - физика
				HUD - интерфейс
		logLevel_ - уровень логирования, задаваемые перечислением LogLevel
			Существует следующие уровни логирования:
				SY_LOGLEVEL_DEBUG
				SY_LOGLEVEL_INFO
				SY_LOGLEVEL_WARNING
				SY_LOGLEVEL_ERROR
				SY_LOGLEVEL_CRITICAL
		message_ - пользовательское сообщение				
3. Для логирования в одном из по умолчанию созданных каналов можно использовать макросы (в них скрыт также и вызов ServiceLocator::instance()->Get<SyErrorLogger>())
	SY_LOG_CORE(LogLevel SyElLogLevel, const std::wstring& message)
	SY_LOG_REND(LogLevel SyElLogLevel, const std::wstring& message)
	SY_LOG_PHYS(LogLevel SyElLogLevel, const std::wstring& message)
	SY_LOG_HUD (LogLevel SyElLogLevel, const std::wstring& message)
  соответственно. 
4. Можно также добавить собственный канал. Для этого используется метод SyErrorLogger::(const t_channelName& channelName_). 

Известные ошибки:
1. Имя файла, имя функции и номер строки пока логируются некорректно.

Все сообщения пока что выводятся в консоль.
Примеры использования SyResult и SyErrorLogger можно найти в файлах ErrorLogger.cpp и PhysicsSystem.h
