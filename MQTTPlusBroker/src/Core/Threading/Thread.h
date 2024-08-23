#pragma once
#include "Ref.h"

#include <thread>

namespace MQTTPlus {
	class Thread : public RefCount {
	public:
		Thread(std::thread thread) {
			m_Thread = std::move(thread);
		};
		~Thread() { Join(); };
		
		void Join() { m_Thread.join(); }
		
		

	private:
		std::thread m_Thread;
	};
}
