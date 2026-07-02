#include "plugin_mod.hpp"
#include "log/log.hpp"

#include "discord/rich_presence.hpp"
#include "discord_rpc_helper.hpp"

#include "ida_helper.hpp"

#include <kernwin.hpp>
#include <name.hpp>
#include <funcs.hpp>
#include <loader.hpp>
#include <nalt.hpp>
#include <diskio.hpp>

idarpc::discord::RichPresence *rpc = nullptr;

void ida_rpc_mod::init_events()
{
    idb_listener = new idarpc::listener::IDBListener();
    hook_event_listener(HT_IDB, idb_listener);

    ui_listener = new idarpc::listener::UIListener();
    hook_event_listener(HT_UI, ui_listener);

    view_listener = new idarpc::listener::ViewListener();
    hook_event_listener(HT_VIEW, view_listener);
}

void ida_rpc_mod::init_discord_rpc()
{
    rpc = new idarpc::discord::RichPresence("1522327899166736565");
}

void ida_rpc_mod::disabled_events()
{
    if (idb_listener)
    {
        unhook_event_listener(HT_IDB, idb_listener);
        delete idb_listener;
        idb_listener = nullptr;
    }

    if (ui_listener)
    {
        unhook_event_listener(HT_UI, ui_listener);
        delete ui_listener;
        ui_listener = nullptr;
    }

    if (view_listener)
    {
        unhook_event_listener(HT_VIEW, view_listener);
        delete view_listener;
        view_listener = nullptr;
    }
}

void ida_rpc_mod::clear_rich_presence()
{
    if (rpc)
    {
        rpc->clear_presence();
        delete rpc;
        rpc = nullptr;
    }
}

void ida_rpc_mod::update_current_context()
{
    discord_helper_spec spec;

    std::string filename = idarpc::idahelper::get_filename();
    std::string summary = idarpc::idahelper::get_analysis_summary();
    std::string current_func = idarpc::idahelper::get_current_function_name();
    std::string current_addr = idarpc::idahelper::get_screen_ea_str();

    if (!current_func.empty())
    {
        spec.details = current_func;
        spec.state = current_addr + " | " + filename;
    }
    else
    {
        spec.details = std::string("Analyzing ") + filename;
        spec.state = summary + " | " + idarpc::idahelper::get_file_type_string();
    }

    spec.small_image_key = "reverser";
    spec.small_image_text = "made by reversinglabs";

    spec.button1_label = "Github";
    spec.button1_url = "https://github.com/936erpl";
    spec.button2_label = "Discord";
    spec.button2_url = "https://discord.gg/VcMegfyeRw";

    idarpc::discord_rpc_helper::update_presence(spec);
}

ida_rpc_mod::ida_rpc_mod()
{
    init_discord_rpc();
    init_events();
    update_current_context();

    idarpc::log(LogLevel::Info, std::string("Plugin initialized for: ") + idarpc::idahelper::get_filename());
}

ida_rpc_mod::~ida_rpc_mod()
{
    idarpc::log(LogLevel::Warning, "Shutting down...");
    disabled_events();
    clear_rich_presence();
}

bool ida_rpc_mod::run(size_t arg)
{
    idarpc::log(LogLevel::Info, "Plugin run() triggered.");
    return true;
}
