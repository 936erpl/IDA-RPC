#include "view_listener.hpp"
#include "log/log.hpp"
#include <kernwin.hpp>
#include <pro.h>
#include <chrono>

#include "discord_rpc_helper.hpp"
#include "ida_helper.hpp"
#include <name.hpp>
#include <funcs.hpp>

static std::string cache_key = "";

namespace idarpc::listener
{

    ssize_t idaapi ViewListener::on_event(ssize_t code, va_list va)
    {

        switch(code) {
            case view_curpos:
            {
                TWidget *view = va_arg(va, TWidget *);
                if(view != nullptr) {
                    qstring title;
                    if(get_widget_title(&title, view)) {
                        update_presence(title.c_str());
                    }
                }
                break;
            }
        }

        return 0;
    }

    void ViewListener::update_presence(const std::string text)
    {
        std::string current_func = idahelper::get_current_function_name();
        std::string current_addr = idahelper::get_screen_ea_str();

        std::string new_key = current_func + "|" + current_addr + "|" + text;
        if (new_key == cache_key)
            return;
        cache_key = new_key;

        std::string summary = idahelper::get_analysis_summary();

        discord_helper_spec spec;
        if (!current_func.empty())
        {
            spec.details = current_func;
            spec.state = current_addr + " @ " + text;
        }
        else
        {
            spec.details = text;
            spec.state = current_addr + " | " + summary;
        }

        spec.small_image_key = "reverser";
        spec.small_image_text = "made by reversinglabs";

        spec.button1_label = "Github";
        spec.button1_url = "https://github.com/936erpl";
        spec.button2_label = "Discord";
        spec.button2_url = "https://discord.gg/VcMegfyeRw";

        idarpc::discord_rpc_helper::update_presence(spec);
    }
}
