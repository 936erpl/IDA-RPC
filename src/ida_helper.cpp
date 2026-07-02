#include "ida_helper.hpp"

#include <idp.hpp>
#include <diskio.hpp>
#include <funcs.hpp>
#include <segment.hpp>
#include <name.hpp>
#include <bytes.hpp>

#include <sstream>
#include <algorithm>
#include <cctype>

#include "log/log.hpp"

namespace idarpc::idahelper {

std::string get_filename()
{
    char filename_buf[260] = {};
    get_root_filename(filename_buf, sizeof(filename_buf));
    return std::string(filename_buf);
}

bool is_ida_home_version()
{
    qstrvec_t dirs;
    get_ida_subdirs(&dirs, nullptr);

    for (const auto &dir : dirs)
    {
        std::string path = dir.c_str();

        path.erase(0, path.find_first_not_of(" \t\r\n"));
        path.erase(path.find_last_not_of(" \t\r\n") + 1);

        std::string clean_path;
        for (char c : path)
        {
            if (c == '\\')
                c = '/';
            if (c != ' ' && c != '\t')
            {
                clean_path += std::tolower(static_cast<unsigned char>(c));
            }
        }

        if (clean_path.find("idahome") != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

std::string get_processor_name()
{
    qstring proc = inf_get_procname();
    if (!proc.empty())
        return std::string(proc.c_str());
    return "Unknown";
}

int get_function_count()
{
    return get_func_qty();
}

int get_segment_count()
{
    return get_segm_qty();
}

std::string get_screen_ea_str()
{
    ea_t ea = get_screen_ea();
    if (ea == BADADDR)
        return "N/A";
    char buf[32];
    qsnprintf(buf, sizeof(buf), "%a", ea);
    return std::string(buf);
}

std::string get_current_function_name()
{
    ea_t ea = get_screen_ea();
    if (ea == BADADDR)
        return "";

    qstring name;
    if (get_func_name(&name, ea) > 0)
        return std::string(name.c_str());
    return "";
}

std::string get_file_type_string()
{
    switch (inf_get_filetype())
    {
        case f_PE:    return "PE";
        case f_ELF:   return "ELF";
        case f_MACHO: return "Mach-O";
        case f_COFF:  return "COFF";
        case f_BIN:   return "Binary";
        case f_HEX:   return "Intel Hex";
        case f_SREC:  return "S-Record";
        case f_OMF:   return "OMF";
        case f_AR:    return "AR";
        case f_COM_old: return "COM";
        case f_EXE_old:
        case f_EXE:   return "DOS EXE";
        case f_WIN:   return "NE";
        case f_LE:    return "LE";
        case f_LX:    return "LX";
        case f_NLM:   return "NLM";
        case f_ZIP:   return "ZIP";
        case f_LOADER: return "External";
        default:      return "Unknown";
    }
}

std::string get_analysis_summary()
{
    int funcs = get_function_count();
    int segs = get_segment_count();

    std::ostringstream ss;
    ss << funcs << " functions, " << segs << " segments";
    return ss.str();
}

}
