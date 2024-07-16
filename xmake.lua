set_project("recipes")
set_version("1.0.0")
set_languages("c++17")
    
includes("src/*/xmake.lua")

-- Define the test dependency
add_requires("gtest", {configs = {main = false, gmock = true}})
-- Iterate over test files and create test targets
for _, file in ipairs(os.files("tests/*Test.cpp")) do
    local name = path.basename(file)
    target(name)
        add_deps("thread")
        set_kind("binary")
        add_packages("gtest")
        set_default(false)
        add_files(file)
        add_tests("default")
end