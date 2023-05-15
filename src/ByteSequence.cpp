#include "ByteSequence.hpp"

ByteSequence::ByteSequence() {}

ByteSequence::ByteSequence(const char *bytes, size_t len)
{
	append(bytes, len);
}

std::string ByteSequence::to_string() const
{
	return std::string(_bytes.begin(), _bytes.end());
}

void ByteSequence::append(const char byte)
{
	_bytes.push_back(byte);
}

void ByteSequence::append(const char *bytes, size_t len)
{
	_bytes.insert(_bytes.end(), bytes, bytes + len);
}

void ByteSequence::append(const ByteSequence &byteSequence)
{
	_bytes.insert(
		_bytes.end(),
		byteSequence._bytes.begin(),
		byteSequence._bytes.end()
	);
}

void ByteSequence::clear()
{
	_bytes.clear();
}

void ByteSequence::pop_back()
{
	_bytes.pop_back();
}

ByteSequence ByteSequence::subSequence(const size_t start, const size_t len) const
{
	ByteSequence seq;

	seq.append(_bytes.data() + start, len);
	return seq;
}

std::vector<ByteSequence> ByteSequence::split(const char delimiter) const
{
	std::vector<ByteSequence> result;

	size_t start = 0;
	size_t end = std::distance(
		_bytes.begin(),
		std::find(_bytes.begin(), _bytes.end(), delimiter)
	);

	while (end < _bytes.size())
	{
		// if (end != start)
		// {
			result.push_back(this->subSequence(start, end - start + 1));
		// }
		start = end + 1;
		end = std::distance(
			_bytes.begin(),
			std::find(_bytes.begin() + start, _bytes.end(), delimiter)
		);
	}
	if (start < _bytes.size())
	{
		result.push_back(this->subSequence(start, _bytes.size() - start));
	}
	return result;
}

const char *ByteSequence::rawBytes() const
{
	return _bytes.data();
}

size_t ByteSequence::size() const
{
	return _bytes.size();
}

size_t ByteSequence::length() const
{
	return _bytes.size();
}

bool ByteSequence::empty() const
{
	return _bytes.empty();
}

char ByteSequence::front() const
{
	return _bytes.front();
}

char ByteSequence::back() const
{
	return _bytes.back();
}

const char &ByteSequence::operator[](const size_t index) const
{
	return _bytes.at(index);
}

char &ByteSequence::operator[](const size_t index)
{
	return _bytes.at(index);
}

bool ByteSequence::endsWith(const std::string &suffix) const
{
	if (suffix.size() > _bytes.size())
	{
		return false;
	}
	return std::equal( suffix.rbegin(), suffix.rend(), _bytes.rbegin() );
}
