#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ByteSequence.hpp"

TEST_CASE("append", "[ByteSequence]")
{
	ByteSequence seq;

	seq.append("abcd", 4);

	REQUIRE_FALSE(seq.empty());

	REQUIRE(seq.size() == 4);

	REQUIRE(seq[0] == 'a');
	REQUIRE(seq[1] == 'b');
	REQUIRE(seq[2] == 'c');
	REQUIRE(seq[3] == 'd');
}

TEST_CASE("subSequence", "[ByteSequence]")
{
	ByteSequence seq("Hello, World!", 13);
	ByteSequence sub = seq.subSequence(0, 5);

	REQUIRE_FALSE(sub.empty());

	REQUIRE(sub.size() == 5);

	REQUIRE(sub.rawBytes()[0] == 'H');
	REQUIRE(sub.rawBytes()[1] == 'e');
	REQUIRE(sub.rawBytes()[2] == 'l');
	REQUIRE(sub.rawBytes()[3] == 'l');
	REQUIRE(sub.rawBytes()[4] == 'o');
}

TEST_CASE("split_test_1", "[ByteSequence]")
{
	ByteSequence seq("GET / HTTP/1.1\nContent-Length: 5\nabc", 36);

	seq.append('\0');
	seq.append('d');

	std::vector<ByteSequence> result = seq.split('\n');

	REQUIRE(result.size() == 3);

	REQUIRE(result[0].size() == 15);
	REQUIRE(result[1].size() == 18);
	REQUIRE(result[2].size() == 5);

	REQUIRE(result[2][0] == 'a');
	REQUIRE(result[2][1] == 'b');
	REQUIRE(result[2][2] == 'c');
	REQUIRE(result[2][3] == '\0');
	REQUIRE(result[2][4] == 'd');
}

TEST_CASE("split_test_2", "[ByteSequence]")
{
	ByteSequence seq("GET / HTTP/1.1\nContent-Length: 5\nabc\ndef\nghi", 44);

	seq.append('\0');
	seq.append("jkl", 3);

	REQUIRE(seq.size() == 48);

	std::vector<ByteSequence> result = seq.split('\n');

	REQUIRE(result.size() == 5);

	REQUIRE(result[0].size() == 15);
	REQUIRE(result[1].size() == 18);
	REQUIRE(result[2].size() == 4);
	REQUIRE(result[3].size() == 4);
	REQUIRE(result[4].size() == 7);

	REQUIRE(result[4][3] == '\0');
}

TEST_CASE("split_test_3", "[ByteSequence]")
{
	ByteSequence seq("\nabc\n\n", 6);

	std::vector<ByteSequence> result = seq.split('\n');

	REQUIRE(result.size() == 3);

	REQUIRE(result[0].size() == 1);
	REQUIRE(result[1].size() == 4);
	REQUIRE(result[2].size() == 1);
}
