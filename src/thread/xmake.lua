add_rules("mode.debug", "mode.release")

target("thread")
    set_kind("static")
    add_includedirs("include", { public = true })