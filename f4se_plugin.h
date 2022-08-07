#pragma once

#include <ShlObj.h>

#include "events.h"
#include "settings.h"

namespace f4se {

	class Plugin {

	public:

		static Plugin& GetInstance()
		{
			static Plugin instance;
			return instance;
		}

		[[nodiscard]] bool Query(const F4SEInterface* f4se, PluginInfo* info);
		[[nodiscard]] bool Load(const F4SEInterface* f4se);

		void AddTask(ITaskDelegate* task);

	private:

		Plugin() {}
		~Plugin() {}

		Plugin(const Plugin&) = delete;
		Plugin(Plugin&&) = delete;

		Plugin& operator=(const Plugin&) = delete;
		Plugin& operator=(Plugin&&) = delete;

		static void MsgCallback(F4SEMessagingInterface::Message* msg);

		IDebugLog iLog;

		PluginHandle hPlugin;

		F4SEMessagingInterface* f4se_msg_interface{ nullptr };
		F4SETaskInterface* f4se_task_interface{ nullptr };
	};
}
