#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace duckx {
typedef unsigned const int formatting_flag;

// text-formatting flags
formatting_flag none = 0;
formatting_flag bold = 1 << 0;
formatting_flag italic = 1 << 1;
formatting_flag underline = 1 << 2;
formatting_flag strikethrough = 1 << 3;
formatting_flag superscript = 1 << 4;
formatting_flag subscript = 1 << 5;
formatting_flag smallcaps = 1 << 6;
formatting_flag shadow = 1 << 7;
//----------------------------------
formatting_flag size_8 = 1 << 8;
formatting_flag size_11 = 1 << 9;
formatting_flag size_12 = 1 << 10;
//----------------------------------
formatting_flag tnr_style = 1 << 11;
//----------------------------------
formatting_flag align_left = 1 << 12;
formatting_flag align_center = 1 << 13;
formatting_flag align_right = 1 << 14;
//----------------------------------
formatting_flag border = 1 << 15;
formatting_flag fill_green = 1 << 16;
formatting_flag vertical_aligment_text = 1 << 17;
} // namespace duckx

#endif
