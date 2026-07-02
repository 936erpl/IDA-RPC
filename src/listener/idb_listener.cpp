#include "idb_listener.hpp"
#include "log/log.hpp"

#include <funcs.hpp>
#include <name.hpp>

#include "discord_rpc_helper.hpp"
#include "ida_helper.hpp"

ssize_t idaapi idarpc::listener::IDBListener::on_event(ssize_t code, va_list va)
{
    switch(code) {
        case idb_event::func_added:
            update_presence();
            break;

        case idb_event::func_updated:
            update_presence();
            break;

        case idb_event::renamed:
            update_presence();
            break;

        case idb_event::make_code:
            break;

        case idb_event::make_data:
            break;

        case idb_event::segm_added:
            update_presence();
            break;
    }

    return 0;
}

void idarpc::listener::IDBListener::update_presence()
{
    discord_helper_spec spec;

    std::string filename = idahelper::get_filename();
    std::string summary = idahelper::get_analysis_summary();
    std::string current_func = idahelper::get_current_function_name();
    std::string current_addr = idahelper::get_screen_ea_str();

    if (!current_func.empty())
    {
        spec.details = current_func;
        spec.state = current_addr + " | " + filename;
    }
    else
    {
        spec.details = std::string("Analyzing ") + filename;
        spec.state = summary + " | " + idahelper::get_file_type_string();
    }

    spec.small_image_key = "reverser";
    spec.small_image_text = "made by reversinglabs";

    spec.button1_label = "Github";
    spec.button1_url = "https://github.com/936erpl";
    spec.button2_label = "Discord";
    spec.button2_url = "https://discord.gg/VcMegfyeRw";

    idarpc::discord_rpc_helper::update_presence(spec);
}
