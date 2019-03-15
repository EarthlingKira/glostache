#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "glo/stache.h"

    
using namespace glo::stache;



TEST_CASE("Test simple variable output", "[glostache]") {
    
    Mustache mustache{"Hallo {{name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hallo Kira!");
}


TEST_CASE("Test section variable output", "[glostache]") {
    
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
    
    Mustache mustache{"Hello {{#name}}{{name}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Object anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with self-dot", "[glostache]") {
    
    
    Mustache mustache{"Hello {{#name}}{{.}}{{/name}}{{^name}}Anonymous{{/name}}!"};
    
    Object object;
    
    object["name"] = "Kira";
    
    Object anonymous;
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
    CHECK(shave(mustache, anonymous, partials) == "Hello Anonymous!");
}


TEST_CASE("Test string section with list", "[glostache]") {
    
    
    Mustache mustache{"Hello {{#names}}{{.}}, {{/names}}!"};
    
    Object object{{"names", Array{"Kira", "Jana"}}};
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira, Jana, !");
}


TEST_CASE("Test simple partial", "[glostache]") {
    
    
    Mustache mustache{"Hello {{> name}}!"_mustache};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{name}}"_mustache;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
}


TEST_CASE("Test dot-notation partial", "[glostache]") {
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Object object;
    
    object["name"] = "Kira";
    
    Partials partials;
    
    partials["name"] = "{{.}}"_mustache;
    
    CHECK(shave(mustache, object, partials) == "Hello Kira!");
}


TEST_CASE("Test unescaped variable", "[glostache]") {
    
    
    Mustache mustache{"Hello {{{name}}}!"_mustache};
    
    Object object;
    
    object["name"] = "Ki<br>ra";
    
    Partials partials;
    
    CHECK(shave(mustache, object, partials) == "Hello Ki<br>ra!");
}


TEST_CASE("Test not importing partial when section is false", "[glostache]") {
    
    
    Mustache mustache{"Hello {{#name}}{{> name}}{{/name}}!"_mustache};
    
    Partials partials;
    
    partials["name"] = "Blubb"_mustache;
    
    CHECK(shave(mustache, Object{{"name", "Kira"}}, partials) == "Hello Blubb!");
    CHECK(shave(mustache, Object{}, partials) == "Hello !");
}



TEST_CASE("Implicit Iterator - Array", "[glostache, mustache_spec]") {
    CHECK(shave(R"("{{#list}}({{#.}}{{.}}{{/.}}){{/list}}")"_mustache,
                 {{"list", Array{Array{1, 2, 3}, Array{"a", "b", "c"}}}},
                 {}) == R"~("(123)(abc)")~");
}



TEST_CASE("Dotted Names - Truthy", "[glostache, mustache_spec]") {
    CHECK(shave(R"("{{#a.b.c}}Here{{/a.b.c}}" == "Here")"_mustache,
                 {{"a", Object{{"b", Object{{"c", true}}}}}},
                 {}) == R"("Here" == "Here")");
}



TEST_CASE("Dotted Names - Falsey", "[glostache, mustache_spec]") {
    CHECK(shave(R"("{{#a.b.c}}Here{{/a.b.c}}" == "")"_mustache,
                 {{"a", Object{{"b", Object{{"c", false}}}}}},
                 {}) == R"("" == "")");
}



TEST_CASE("Dotted Names - Broken Chains", "[glostache, mustache_spec]") {
    CHECK(shave(R"("{{#a.b.c}}Here{{/a.b.c}}" == "")"_mustache,
                 {{"a", Object{}}},
                 {}) == R"("" == "")");
}



TEST_CASE("Standalone Lines - Standalone lines should be removed from the template.", "[glostache, mustache_spec]") {
    auto t = R"(|
| This Is
{{#boolean}}
|
{{/boolean}}
| A Line)"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == R"(|
| This Is
|
| A Line)");
}



TEST_CASE("Indented Standalone Lines - Indented standalone lines should be removed from the template.", "[glostache, mustache_spec]") {
    auto t = R"(|
| This Is
  {{#boolean}}
|
  {{/boolean}}
| A Line)"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == R"(|
| This Is
|
| A Line)");
}



TEST_CASE("(Custom) Indented Standalone Lines - Indented standalone lines should be removed from the template on lists", "[glostache]") {
    auto t = R"(|
| This Is
  {{#list}}
|
  {{/list}}
| A Line)"_mustache;
    Object o{{"list", Array{1, 2, 3}}};
    
    CHECK(shave(t, o) == R"(|
| This Is
|
|
|
| A Line)");
}



TEST_CASE(R"(Standalone Line Endings - "\r\n" should be considered a newline for standalone tags.)", "[glostache, mustache_spec]") {
    auto t = "|\r\n{{#boolean}}\r\n{{/boolean}}\r\n|"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == "|\r\n|");
}



TEST_CASE("Standalone Without Previous Line - Standalone tags should not require a newline to precede them.", "[glostache, mustache_spec]") {
    auto t = "  {{#boolean}}\n#{{/boolean}}\n/"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == "#\n/");
}



TEST_CASE("Standalone Without Newline - Standalone tags should not require a newline to follow them.", "[glostache, mustache_spec]") {
    auto t = "#{{#boolean}}\n/\n  {{/boolean}}"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == "#\n/\n");
}



TEST_CASE("Padding - Superfluous in-tag whitespace should be ignored.", "[glostache, mustache_spec]") {
    auto t = "|{{# boolean }}={{/ boolean }}|"_mustache;
    Object o{{"boolean", true}};
    
    CHECK(shave(t, o) == "|=|");
}
