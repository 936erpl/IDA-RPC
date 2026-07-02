#pragma once

#include <string>

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
}
