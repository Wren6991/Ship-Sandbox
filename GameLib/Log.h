/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cassert>
#include <deque>
#include <functional>
#include <sstream>
#include <string>

namespace /* anonymous */ {

	template<typename T>
	std::wstringstream & _LogToStream(std::wstringstream & ss, T const & t)
	{
		ss << t << std::endl;
		return ss;
	}

	template<typename T, typename... TArgs>
	std::wstringstream & _LogToStream(std::wstringstream & ss, T const & t, TArgs&&... args)
	{
		ss << t;
		return _LogToStream(ss, std::forward<TArgs>(args)...);
	}
}

class Logger
{
public:

	Logger()
		: mCurrentListener()
		, mStoredMessages()
	{
	}

	Logger(Logger const &) = delete;
	Logger(Logger &&) = delete;
	Logger & operator=(Logger const &) = delete;
	Logger & operator=(Logger &&) = delete;

	void RegisterListener(
		std::function<void(std::wstring const & message)> listener)
	{
		assert(!mCurrentListener);
		mCurrentListener = std::move(listener);

		// Publish all the messages so far
		for (std::wstring const & message : mStoredMessages)
		{
			mCurrentListener(message);
		}
	}

	void UnregisterListener()
	{
		assert(!!mCurrentListener);
		mCurrentListener = {};
	}

	template<typename...TArgs>
	void Log(TArgs&&... args)
	{
		std::wstringstream ss;
		_LogToStream(ss, std::forward<TArgs>(args)...);

		std::wstring const & message = ss.str();

		// Store
		mStoredMessages.push_back(message);
		if (mStoredMessages.size() > MaxStoredMessages)
		{
			mStoredMessages.pop_front();
		}

		// Publish
		if (!!mCurrentListener)
		{
			mCurrentListener(message);
		}
	}

public:

	static Logger Instance;

private:

	// The current listener
	std::function<void(std::wstring const & message)> mCurrentListener;

	// The messages stored so far
	std::deque<std::wstring> mStoredMessages;
	static constexpr size_t MaxStoredMessages = 10000;
};

//
// Global aliases
//

template<typename... TArgs>
void LogMessage(TArgs&&... args)
{
	Logger::Instance.Log(std::forward<TArgs>(args)...);
}

template<typename... TArgs>
void LogDebug(TArgs&&... args)
{
#ifdef _DEBUG
	Logger::Instance.Log(std::forward<TArgs>(args)...);
#endif
}

