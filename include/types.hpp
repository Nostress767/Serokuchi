#pragma once

#include <cstdint>

// Because I love rust-lang
using i8 = int8_t; using u8 = uint8_t;
using u16 = uint16_t; using i16 = int16_t;
using f32 = float; using i32 = int32_t; using u32 = uint32_t;
using f64 = double; using i64 = int64_t; using u64 = uint64_t;

#ifdef DEBUG

#include <iostream>
#include <source_location>

#define reset_text                         "\e[0m"

#define black_regular_text                 "\e[0;30m"
#define red_regular_text                   "\e[0;31m"
#define green_regular_text                 "\e[0;32m"
#define yellow_regular_text                "\e[0;33m"
#define blue_regular_text                  "\e[0;34m"
#define purple_regular_text                "\e[0;35m"
#define cyan_regular_text                  "\e[0;36m"
#define white_regular_text                 "\e[0;37m"

#define black_bold_text                    "\e[1;30m"
#define red_bold_text                      "\e[1;31m"
#define green_bold_text                    "\e[1;32m"
#define yellow_bold_text                   "\e[1;33m"
#define blue_bold_text                     "\e[1;34m"
#define purple_bold_text                   "\e[1;35m"
#define cyan_bold_text                     "\e[1;36m"
#define white_bold_text                    "\e[1;37m"

#define black_faint_text                   "\e[2;30m"
#define red_faint_text                     "\e[2;31m"
#define green_faint_text                   "\e[2;32m"
#define yellow_faint_text                  "\e[2;33m"
#define blue_faint_text                    "\e[2;34m"
#define purple_faint_text                  "\e[2;35m"
#define cyan_faint_text                    "\e[2;36m"
#define white_faint_text                   "\e[2;37m"

#define black_italic_text                  "\e[3;30m"
#define red_italic_text                    "\e[3;31m"
#define green_italic_text                  "\e[3;32m"
#define yellow_italic_text                 "\e[3;33m"
#define blue_italic_text                   "\e[3;34m"
#define purple_italic_text                 "\e[3;35m"
#define cyan_italic_text                   "\e[3;36m"
#define white_italic_text                  "\e[3;37m"

#define black_underline_text               "\e[4;30m"
#define red_underline_text                 "\e[4;31m"
#define green_underline_text               "\e[4;32m"
#define yellow_underline_text              "\e[4;33m"
#define blue_underline_text                "\e[4;34m"
#define purple_underline_text              "\e[4;35m"
#define cyan_underline_text                "\e[4;36m"
#define white_underline_text               "\e[4;37m"

#define black_background                   "\e[40m"
#define red_background                     "\e[41m"
#define green_background                   "\e[42m"
#define yellow_background                  "\e[43m"
#define blue_background                    "\e[44m"
#define purple_background                  "\e[45m"
#define cyan_background                    "\e[46m"
#define white_background                   "\e[47m"

//#define purple_regular_background "\e[1;45m"

#define black_regular_high_intensity_text  "\e[0;90m"
#define red_regular_high_intensity_text    "\e[0;91m"
#define green_regular_high_intensity_text  "\e[0;92m"
#define yellow_regular_high_intensity_text "\e[0;93m"
#define blue_regular_high_intensity_text   "\e[0;94m"
#define purple_regular_high_intensity_text "\e[0;95m"
#define cyan_regular_high_intensity_text   "\e[0;96m"
#define white_regular_high_intensity_text  "\e[0;97m"

#define black_bold_high_intensity_text     "\e[1;90m"
#define red_bold_high_intensity_text       "\e[1;91m"
#define green_bold_high_intensity_text     "\e[1;92m"
#define yellow_bold_high_intensity_text    "\e[1;93m"
#define blue_bold_high_intensity_text      "\e[1;94m"
#define purple_bold_high_intensity_text    "\e[1;95m"
#define cyan_bold_high_intensity_text      "\e[1;96m"
#define white_bold_high_intensity_text     "\e[1;97m"

#define black_high_intensity_background    "\e[0;100m"
#define red_high_intensity_background      "\e[0;101m"
#define green_high_intensity_background    "\e[0;102m"
#define yellow_high_intensity_background   "\e[0;103m"
#define blue_high_intensity_background     "\e[0;104m"
#define purple_high_intensity_background   "\e[0;105m"
#define cyan_high_intensity_background     "\e[0;106m"
#define white_high_intensity_background    "\e[0;107m"


/*
 *
    auto const localTime = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}; \
 *
                 << black_bold_high_intensity_text                                                                 \
                 << localTime                                                                                      \
                 << " "                                                                                            \
                 << ___globalLogClock.getTotalElapsed<TimeUnit::seconds>()                                         \
                 << reset_text                                                                                     \
                 << " "                                                                                            \
 *
 *
                 << location.file_name() << ':'                                                                    \
                 << location.line() << ':'                                                                         \
                 << location.column() << ": "                                                                      \
 *
 */

#define ___log(outputStream, typeBackgroundColor, type, place, messageBackgroundColor, message)                    \
  do{                                                                                                              \
    outputStream << black_bold_high_intensity_text                                                                 \
                 << "["                                                                                            \
                 << reset_text                                                                                     \
                 << white_regular_text                                                                             \
                 << ___globalLogClock.tick().getTotalFrames()                                                      \
                 << reset_text                                                                                     \
                 << " "                                                                                            \
                 << typeBackgroundColor                                                                            \
                 << black_faint_text                                                                               \
                 << type                                                                                           \
                 << reset_text                                                                                     \
                 << " "                                                                                            \
                 << black_bold_high_intensity_text                                                                 \
                 << "'" << place << "'"                                                                            \
                 << "] "                                                                                           \
                 << reset_text                                                                                     \
                 << messageBackgroundColor                                                                         \
                 << black_faint_text                                                                               \
                 << "|" << message << "|"                                                                          \
                 << reset_text                                                                                     \
                 << "\n";                                                                                          \
  } while(0)
#define log_debug(message) ___log(std::wclog, blue_high_intensity_background, "|DEBUG|", std::source_location::current().function_name(), blue_high_intensity_background, message)
#define log_error(message) ___log(std::wcerr, red_high_intensity_background, "|ERROR|", std::source_location::current().function_name(), red_high_intensity_background, message)
#define log_info(message) ___log(std::wclog, green_background, "|INFO|", std::source_location::current().function_name(), green_background, message)
#define log_warn(message) ___log(std::wclog, yellow_background, "|WARN|", std::source_location::current().function_name(), yellow_background, message)

#ifdef show_message_loop
#define log_message(place, message) ___log(std::wclog, white_high_intensity_background, "|MESSAGE|", place, white_high_intensity_background, message)
#else
#define log_message(nothing1, nothing2)
#endif

#else

#define log_debug(nothing)
#define log_error(nothing)
#define log_info(nothing)
#define log_warn(nothing)
#define log_message(nothing1, nothing2)

#endif // DEBUG
