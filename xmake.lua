add_rules("mode.debug", "mode.release")

target("recipes")
    set_kind("binary")
    add_files("src/*.cpp")