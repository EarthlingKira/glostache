#include <benchmark/benchmark.h>


static char original_tmp[] =
R"(<h1>{{header}}</h1>
{{#bug}}
{{/bug}}

{{# items}}
  {{#first}}
    <li><strong>{{name}}</strong></li>
  {{/first}}
  {{#link}}
    <li><a {{>href}}>{{name}}</a></li>
  {{/link}}
{{ /items}}

{{#empty}}
  <p>The list is empty.</p>
{{/ empty }}

{{=[ ]=}}

[#array]([.])[/array]

[#items]
[count]->[count]->[count]
[/items]

[a.b.c] == [#a][#b][c][/b][/a]

<div class="comments">
    <h3>[header]</h3>
    <ul>
        [#comments]
        <li class="comment">
            <h5>[name]</h5>
            <p>[body]</p>
        </li>
        <!--[count]-->
        [/comments]
    </ul>
</div>)";

static char tmp[] =
R"(<h1>{{header}}</h1>
{{#bug}}
{{/bug}}

{{# items}}
  {{#first}}
    <li><strong>{{very_long_key_name}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}}</strong></li>
  {{/first}}
  {{#link}}
    <li><a {{>href}}>{{very_long_key_name}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}}</a></li>
  {{/link}}
{{ /items}}

{{#empty}}
  <p>The list is empty.</p>
{{/ empty }}

{{#array}}({{.}}){{/array}}

{{#items}}
{{count}}->{{count}}->{{count}}
{{/items}}

{{a.b.c}} == {{#a}}{{#b}}{{c}}{{/b}}{{/a}}

<div class="comments">
    <h3>{{header}}</h3>
    <ul>
        {{#comments}}
        <li class="comment">
            <h5>{{very_long_key_name}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}} - {{very_long_key_name_miss}}</h5>
            <p>{{body}}</p>
        </li>
        <!--{{count}}-->
        {{/comments}}
    </ul>
</div>)";


#include "glo/stache.h"

static void basic_usage_glostache(benchmark::State& state) {
    
    using namespace glo::stache;
    
    Mustache comment_tmp{
        "<div class=\"comments\"><h3>{{header}}</h3><ul>"
        "{{#comments}}<li class=\"comment\"><h5>{{name}}</h5>"
        "<p>{{body}}</p></li>{{/comments}}</ul></div>"};

    auto comment_view = Object{
        {"header", std::string{"My Post Comments"}},
            {"comments", Array{
                Object{{"name", std::string{"Joe"}}, {"body", std::string{"Thanks for this post!"}}},
                Object{{"name", std::string{"Sam"}}, {"body", std::string{"Thanks for this post!"}}},
                Object{{"name", std::string{"Heather"}}, {"body", std::string{"Thanks for this post!"}}},
                Object{{"name", std::string{"Kathy"}}, {"body", std::string{"Thanks for this post!"}}},
                Object{{"name", std::string{"George"}}, {"body", std::string{"Thanks for this post!"}}}}}};

    Partials partials;
                
    while (state.KeepRunning())
        shave(comment_tmp, comment_view, partials);
}



static void glostache_usage(benchmark::State& state)
{
    using namespace glo::stache;
    using namespace std::string_literals;

    Partials partials
    {{
        {"href", "href=\"{{url}}\""_mustache}
    }};
    
    Mustache mustache{tmp};

//     int n = 0;
    Object data
    {
        {"header", "Colors"s},
        {"items",
            Array
            {
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "red"s},
                    {"first", true},
                    {"url", "#Red"s}
                },
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "green"s},
                    {"link", true},
                    {"url", "#Green"s}
                },
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "blue"s},
                    {"link", true},
                    {"url", "#Blue"s}
                }
            }
        },
        {"empty", false},
//         {"count", lambda{[&n]() -> node { return ++n; }}},
        {"array", Array{1, 2, 3}},
        {"a", Object{{"b", Object{{"c", true}}}}},
        {"comments",
            Array
            {
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "Joe"s},
                    {"body", "<html> should be escaped"s}
                },
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "Sam"s},
                    {"body", "{{mustache}} can be seen"s}
                },
                Object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name", "New"s},
                    {"body", "break\nup"s}
                }
            }
        }
    };

    while (state.KeepRunning())
    {
//         n = 0;
        shave(mustache, data, partials);
    }
}

BENCHMARK(basic_usage_glostache);

BENCHMARK(glostache_usage);



#include "mstch/mstch.hpp"

static void basic_usage_mstch(benchmark::State& state) {
    std::string comment_tmp{
        "<div class=\"comments\"><h3>{{header}}</h3><ul>"
        "{{#comments}}<li class=\"comment\"><h5>{{name}}</h5>"
        "<p>{{body}}</p></li>{{/comments}}</ul></div>"};

    auto comment_view = mstch::map{
        {"header", std::string{"My Post Comments"}},
            {"comments", mstch::array{
                mstch::map{{"name", std::string{"Joe"}}, {"body", std::string{"Thanks for this post!"}}},
                mstch::map{{"name", std::string{"Sam"}}, {"body", std::string{"Thanks for this post!"}}},
                mstch::map{{"name", std::string{"Heather"}}, {"body", std::string{"Thanks for this post!"}}},
                mstch::map{{"name", std::string{"Kathy"}}, {"body", std::string{"Thanks for this post!"}}},
                mstch::map{{"name", std::string{"George"}}, {"body", std::string{"Thanks for this post!"}}}}}};

    while (state.KeepRunning())
        mstch::render(comment_tmp, comment_view);
}



static void mstch_usage(benchmark::State& state)
{
    using namespace mstch;
    using namespace std::string_literals;

    std::map<std::string, std::string> context
    {
        {"href", "href=\"{{url}}\""}
    };

//     int n = 0;
    map data
    {
        {"header", "Colors"s},
        {"items",
            array
            {
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "red"s},
                    {"first", true},
                    {"url", "#Red"s}
                },
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "green"s},
                    {"link", true},
                    {"url", "#Green"s}
                },
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "blue"s},
                    {"link", true},
                    {"url", "#Blue"s}
                }
            }
        },
        {"empty", false},
//         {"count", lambda{[&n]() -> node { return ++n; }}},
        {"array", array{1, 2, 3}},
        {"a", map{{"b", map{{"c", true}}}}},
        {"comments",
            array
            {
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "Joe"s},
                    {"body", "<html> should be escaped"s}
                },
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "Sam"s},
                    {"body", "{{mustache}} can be seen"s}
                },
                map
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "New"s},
                    {"body", "break\nup"s}
                }
            }
        }
    };

    while (state.KeepRunning())
    {
//         n = 0;
        render(tmp, data, context);
    }
}

BENCHMARK(basic_usage_mstch);

BENCHMARK(mstch_usage);



#include <bustache/model.hpp>

static void bustache_usage(benchmark::State& state)
{
    using namespace bustache;

    boost::unordered_map<std::string, bustache::format> context
    {
        {"href", "href=\"{{url}}\""_fmt}
    };

    int n = 0;
    object data
    {
        {"header", "Colors"},
        {"items",
            array
            {
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "red"},
                    {"first", true},
                    {"url", "#Red"}
                },
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "green"},
                    {"link", true},
                    {"url", "#Green"}
                },
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "blue"},
                    {"link", true},
                    {"url", "#Blue"}
                }
            }
        },
        {"empty", false},
//         {"count", [&n] { return ++n; }},
        {"array", array{1, 2, 3}},
        {"a", object{{"b", object{{"c", true}}}}},
        {"comments",
            array
            {
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "Joe"},
                    {"body", "<html> should be escaped"}
                },
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "Sam"},
                    {"body", "{{mustache}} can be seen"}
                },
                object
                {
                    {"very_long_key_name1", "red"},
                    {"very_long_key_name2", "red"},
                    {"very_long_key_name3", "red"},
                    {"very_long_key_name4", "red"},
                    {"very_long_key_name5", "red"},
                    {"very_long_key_name6", "red"},
                    {"very_long_key_name7", "red"},
                    {"very_long_key_name8", "red"},
                    {"very_long_key_name9", "red"},
                    {"very_long_key_name10", "red"},
                    {"very_long_key_name11", "red"},
                    {"very_long_key_name12", "red"},
                    {"very_long_key_name13", "red"},
                    {"very_long_key_name14", "red"},
                    {"very_long_key_name15", "red"},
                    {"very_long_key_name16", "red"},
                    {"very_long_key_name17", "red"},
                    {"very_long_key_name18", "red"},
                    {"very_long_key_name19", "red"},
                    {"very_long_key_name20", "red"},
                    {"very_long_key_name", "New"},
                    {"body", "break\nup"}
                }
            }
        }
    };

    while (state.KeepRunning())
    {
        n = 0;
        format fmt(tmp);
        to_string(fmt(data, context, escape_html));
    }
}

BENCHMARK(bustache_usage);


BENCHMARK_MAIN();
