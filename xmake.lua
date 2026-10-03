add_rules("mode.debug", "mode.release")
set_xmakever("3.0.0")

add_repositories("levimc-repo https://github.com/LiteLDev/xmake-repo.git")

option("target_type")
    set_default("client")
    set_showmenu(true)
    set_values("client")
option_end()

add_requires("levilamina 26.51.*", {configs = {target_type = "client"}})
add_requires("levibuildscript")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("NovaMapClient")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")

    if is_plat("windows") then
        add_defines("NOMINMAX", "UNICODE", "WIN32_LEAN_AND_MEAN")
        add_cxflags("/EHa", "/utf-8", "/W4")
        set_toolchains("clang-cl")
        add_syslinks("user32", "gdi32", "dwmapi")
    end

    add_packages("levilamina")
    set_kind("shared")
    set_languages("c++20")

    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_includedirs("src")
