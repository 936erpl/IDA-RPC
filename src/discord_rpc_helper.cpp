#include "discord_rpc_helper.hpp"
#include "discord/rich_presence.hpp"
#include "ida_helper.hpp"

#include <pro.h>
#include <kernwin.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <name.hpp>
#include <bytes.hpp>

#include <sstream>

extern idarpc::discord::RichPresence *rpc;

static time_t start_time = time(nullptr);

static std::string get_ida_version_string()
{
    char version[32] = {0};
    get_kernel_version(version, sizeof(version));
    return std::string(version);
}

namespace idarpc::discord_rpc_helper
{
    void update_presence(const discord_helper_spec &spec)
    {
        if (!rpc)
            return;

        DiscordRichPresence presence{};
        presence.startTimestamp = start_time;

        if (idarpc::idahelper::is_ida_home_version())
        {
            presence.largeImageKey = "ida_home";
            std::string homeText = "IDA Home " + get_ida_version_string();
            presence.largeImageText = homeText.c_str();
        }
        else
        {
            presence.largeImageKey = "ida_pro";
            std::string proText = "IDA Pro " + get_ida_version_string();
            presence.largeImageText = proText.c_str();
        }

        std::string state = spec.state.empty()
            ? idahelper::get_analysis_summary()
            : spec.state;
        presence.state = state.c_str();

        std::string details = spec.details.empty()
            ? std::string("Analyzing ") + idahelper::get_filename()
            : spec.details;
        presence.details = details.c_str();

        if (!spec.large_image_key.empty())
            presence.largeImageKey = spec.large_image_key.c_str();
        if (!spec.large_image_text.empty())
            presence.largeImageText = spec.large_image_text.c_str();
        if (!spec.small_image_key.empty())
            presence.smallImageKey = spec.small_image_key.c_str();
        if (!spec.small_image_text.empty())
            presence.smallImageText = spec.small_image_text.c_str();

        if (!spec.button1_label.empty() && !spec.button1_url.empty())
        {
            presence.button1_label = spec.button1_label.c_str();
            presence.button1_url = spec.button1_url.c_str();
        }
        if (!spec.button2_label.empty() && !spec.button2_url.empty())
        {
            presence.button2_label = spec.button2_label.c_str();
            presence.button2_url = spec.button2_url.c_str();
        }

        rpc->update_presence(presence);
    }

}
