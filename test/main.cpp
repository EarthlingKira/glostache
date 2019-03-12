#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "glo/stache.h"


TEST_CASE("Test simple variable output", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hallo {{name}}!"};
    
    Stash stash;
    
    stash["name"] = "Kira";
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Hallo Kira!");
}


TEST_CASE("Test section variable output", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"{{name}}, {{#test_section}}{{name}} is {{ attr }}{{/test_section}}!"};
    
    Stash stash;
    
    stash["name"] = "Nope";
    
    Stash test_section;
    test_section["name"] = "Kira";
    test_section["attr"] = "coding";
    
    stash["test_section"] = std::move(test_section);
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Nope, Kira is coding!");
}


TEST_CASE("Test inverted section", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{name}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Stash stash;
    
    stash["name"] = "Kira";
    
    Stash anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with self-dot", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{.}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Stash stash;
    
    stash["name"] = "Kira";
    
    Stash anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with list", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#names}}{{.}}, {{/names}}!"};
    
    Stash stash{{"names", Value_list{"Kira", "Jana"}}};
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Hello Kira, Jana, !");
}


TEST_CASE("Test simple partial", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{> name}}!"_mustache};
    
    Stash stash;
    
    stash["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{name}}"_mustache;
    
    CHECK(shave(mustache, stash, partials) == "Hello Kira!");
}


TEST_CASE("Test dot-notation partial", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Stash stash;
    
    stash["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{.}}"_mustache;
    
    CHECK(shave(mustache, stash, partials) == "Hello Kira!");
}


TEST_CASE("Test unescaped variable", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{{name}}}!"_mustache};
    
    Stash stash;
    
    stash["name"] = "Ki<br>ra";
    
    Partials partials;
    
    CHECK(shave(mustache, stash, partials) == "Hello Ki<br>ra!");
}


TEST_CASE("Test not importing partial when section is false", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Partials partials;
    
    partials["name"] = "Blubb"_mustache;
    
    CHECK(shave(mustache, Stash{{"name", "Kira"}}, partials) == "Hello Blubb!");
    CHECK(shave(mustache, Stash{}, partials) == "Hello !");
}
