#pragma once

#include <string>
#include <vector>

class ByteSequence
{
	public:
		ByteSequence();
		ByteSequence(const char *bytes, size_t len);

		std::string to_string() const;

		void append(char byte);
		void append(const char *bytes, size_t len);
		void append(const ByteSequence &byteSequence);

		void clear();

		void pop_back();

		ByteSequence subSequence(size_t start, size_t len) const;

		std::vector<ByteSequence> split(char delimiter) const;

		const char *rawBytes() const;
		size_t size() const;
		size_t length() const;
		bool empty() const;

		char front() const;
		char back() const;

		const char &operator[](size_t index) const;
		char &operator[](size_t index);

		bool endsWith(const std::string &suffix) const;

	private:
		std::vector<char> _bytes;
};
