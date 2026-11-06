#include "doctest/doctest.h"
#include "../../DomainModels/tag.h"

TEST_CASE("Tag stores a name") {
	Tag tag("Greeting");
	CHECK(tag.GetName() == "Greeting");
}

TEST_CASE("Tag::Split trims names and skips empty items") {
	std::vector<Tag> tags = Tag::Split(" Greeting, Noun ,\tVerb,  , ");
	REQUIRE(tags.size() == 3);
	CHECK(tags[0].GetName() == "Greeting");
	CHECK(tags[1].GetName() == "Noun");
	CHECK(tags[2].GetName() == "Verb");

	CHECK(Tag::Split("").empty());
	CHECK(Tag::Split("   ").empty());
	CHECK(Tag::Split(",,,").empty());
}

TEST_CASE("Tag::Join inserts comma-space separators") {
	CHECK(Tag::Join({}).empty());
	CHECK(Tag::Join({Tag("Greeting")}) == "Greeting");
	CHECK(Tag::Join({Tag("Greeting"), Tag("Noun")}) == "Greeting, Noun");
}

TEST_CASE("Tag::Join round-trips Tag::Split") {
	const std::string original = "Greeting, Noun, Verb";
	CHECK(Tag::Join(Tag::Split(original)) == original);
}
