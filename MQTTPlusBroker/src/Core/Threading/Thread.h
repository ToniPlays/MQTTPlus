#pragma once
#include "Ref.h"

#include <thread>

namespace MQTTPlus {
	class Thread : public RefCount {
	public:
		Thread(std::thread thread) {
			m_Thread = std::move(thread);
		};
		~Thread() = default;
		
		void Join() { std::cout << "Joining\n"; m_Thread.join(); }
	private:
		std::thread m_Thread;
	};
}
