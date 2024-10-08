#pragma once

#include "Buffer.h"
#include "Ref.h"
#include <spdlog/fmt/fmt.h>
#include "Core/MQTTPlusException.h"

namespace MQTTPlus {

	class CachedBuffer : public RefCount
	{
	public:

		CachedBuffer() = default;

		CachedBuffer(uint64_t size)
		{
			m_DataBuffer.Allocate(size);
			m_DataBuffer.ZeroInitialize();
		}

		CachedBuffer(void* data, uint64_t size)
		{
			m_DataBuffer = Buffer(data, size);
		}

		CachedBuffer(Buffer buffer)
		{
			m_DataBuffer = buffer;
		}

		~CachedBuffer()
		{
			//m_DataBuffer.Release();
		}

		void Allocate(uint64_t size)
		{
			m_DataBuffer.Allocate(size);
			m_DataBuffer.ZeroInitialize();
		}

		template<typename T>
		T Read()
		{
			if (m_DataBuffer.Size < m_CurrentBufferOffset + sizeof(T))
			{
				auto err = fmt::format("Tried reading from {} + {} but size is only {}\n", m_CurrentBufferOffset, sizeof(T), m_DataBuffer.Size);
				throw MQTTPlusException(err);
			}

			T value = Buffer::Get<T>(m_DataBuffer.Data, m_CurrentBufferOffset);
			m_CurrentBufferOffset += sizeof(T);
			return value;
		}

		template<typename T>
		std::vector<T> Read(uint64_t size)
		{
			if (m_DataBuffer.Size < m_CurrentBufferOffset + size) assert(false);

			T* start = (T*)m_DataBuffer.Data + m_CurrentBufferOffset;
			m_CurrentBufferOffset += size;
			return std::vector<T>(start, start + size);
		}

		template<typename T>
		uint64_t Write(T value)
		{
			m_DataBuffer.Write(&value, sizeof(T), m_CurrentBufferOffset);
			m_CurrentBufferOffset += sizeof(T);
			return sizeof(T);
		}

		template<typename T>
		uint64_t Write(T* data, uint64_t dataLength)
		{
			m_DataBuffer.Write((const void*)data, dataLength, m_CurrentBufferOffset);
			m_CurrentBufferOffset += dataLength;
			return dataLength;
		}

		bool Available() const { return m_CurrentBufferOffset < m_DataBuffer.Size; }
		void* GetData() const { return m_DataBuffer.Data; }
		uint64_t GetSize() const { return m_DataBuffer.Size; }
		uint64_t GetCursor() const { return m_CurrentBufferOffset; }
		void AddBufferOffset(uint64_t offset) { m_CurrentBufferOffset = offset; }
		void ResetCursor() { m_CurrentBufferOffset = 0; }

		void Resize(uint64_t size)
		{
			Buffer oldBuffer = m_DataBuffer;

			m_DataBuffer = Buffer();
			m_DataBuffer.Allocate(size);
			m_DataBuffer.TryWrite(oldBuffer.Data, oldBuffer.Size);
			oldBuffer.Release();
		}

	private:
		Buffer m_DataBuffer;
		uint64_t m_CurrentBufferOffset = 0;
	};
}