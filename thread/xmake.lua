add_rules("mode.debug", "mode.release")

target("thread")
    set_languages("c++20")
    set_kind("binary")
    add_files("test/*.cpp")
    add_includedirs("$(curdir)/thread")