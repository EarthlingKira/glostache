#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "glo/stache.h"


TEST_CASE("Test simple variable output", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hallo {{name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hallo Kira!");
}


TEST_CASE("Test section variable output", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"{{name}}, {{#test_section}}{{name}} is {{ attr }}{{/test_section}}!"};
    
    Object object;
    
    object["name"] = "Nope";
    
    Object test_section;
    test_section["name"] = "Kira";
    test_section["attr"] = "coding";
    
    object["test_section"] = std::move(test_section);
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Nope, Kira is coding!");
}


TEST_CASE("Test inverted section", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{name}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Object anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with self-dot", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{.}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Object anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with list", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#names}}{{.}}, {{/names}}!"};
    
    Object object{{"names", Array{"Kira", "Jana"}}};
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira, Jana, !");
}


TEST_CASE("Test simple partial", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{> name}}!"_mustache};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{name}}"_mustache;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
}


TEST_CASE("Test dot-notation partial", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{.}}"_mustache;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
}


TEST_CASE("Test unescaped variable", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{{name}}}!"_mustache};
    
    Object object;
    
    object["name"] = "Ki<br>ra";
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Ki<br>ra!");
}


TEST_CASE("Test not importing partial when section is false", "[glostache]") {
    
    using namespace glo::stache;
    
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Partials partials;
    
    partials["name"] = "Blubb"_mustache;
    
    CHECK(shave(mustache, Object{{"name", "Kira"}}, partials) == "Hello Blubb!");
    CHECK(shave(mustache, Object{}, partials) == "Hello !");
}
