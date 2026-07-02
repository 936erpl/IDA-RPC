#include "ui_listener.hpp"
#include "log/log.hpp"

#include <loader.hpp>
#include <name.hpp>
#include <funcs.hpp>

#include "discord_rpc_helper.hpp"
#include "ida_helper.hpp"

ssize_t idaapi idarpc::listener::UIListener::on_event(ssize_t code, va_list va)
{
    switch(code) {
        case ui_ready_to_run:
            break;

        case ui_load_file:
            idarpc::log(LogLevel::Info, "File loaded, updating presence.");
            update_presence();
            break;

        case ui_refresh:
            break;

        case ui_get_cursor:
            break;

        case ui_get_curline:
            break;
    }

    return 0;
}

void idarpc::listener::UIListener::update_presence()
{
    discord_helper_spec spec;

    std::string filename = idahelper::get_filename();
    std::string summary = idahelper::get_analysis_summary();

    spec.details = std::string("Analyzing ") + filename;
    spec.state = summary + " | " + idahelper::get_file_type_string();
    spec.small_image_key = "reverser";
    spec.small_image_text = "made by reversinglabs";

    spec.button1_label = "Github";
    spec.button1_url = "https://github.com/936erpl";
    spec.button2_label = "Discord";
    spec.button2_url = "https://discord.gg/VcMegfyeRw";

    idarpc::discord_rpc_helper::update_presence(spec);
}
