set_project("LearnOpenGL")
set_version("1.0.0")
set_languages("c++17")

target("base")
    set_kind("static")
    add_files("base/*.cpp")