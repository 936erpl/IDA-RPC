#pragma once

#include <string>
#include <ida.hpp>

namespace idarpc::idahelper {
    std::string get_filename();
    bool is_ida_home_version();
    std::string get_processor_name();
    int get_function_count();
    int get_segment_count();
    std::string get_screen_ea_str();
    std::string get_current_function_name();
    std::string get_file_type_string();
    std::string get_analysis_summary();

    struct func_extra_info
    {
        size_t size = 0;
        int xref_count = 0;
        bool is_lib = false;
        bool is_thunk = false;
        std::string segment_name;
    };
    func_extra_info get_function_extra_info(ea_t ea);

}
