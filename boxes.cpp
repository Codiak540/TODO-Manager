#include "boxes.h"
#include "colors.h"
#include <vector>
#include <algorithm>

// Helper function to calculate the visible length of a string (excluding ANSI codes)
// Properly handles UTF-8 multi-byte characters
size_t visible_length(const std::string& str) {
    size_t len = 0;
    bool in_escape = false;

    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];

        // Check for ANSI escape sequence
        if (c == '\033') {
            in_escape = true;
            i++;
            continue;
        }

        if (in_escape) {
            if (c == 'm') {
                in_escape = false;
            }
            i++;
            continue;
        }

        // Count UTF-8 characters
        // UTF-8 character lengths:
        // 0xxxxxxx = 1 byte (ASCII)
        // 110xxxxx = 2 bytes
        // 1110xxxx = 3 bytes
        // 11110xxx = 4 bytes
        if ((c & 0x80) == 0) {
            // 1-byte character (ASCII)
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte character
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte character
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte character
            i += 4;
        } else {
            // Invalid UTF-8, just skip this byte
            i += 1;
        }

        len++;
    }

    return len;
}

u_long boxes::padding(const u_long length, const u_long size) {
    return (size - length + PADDING) / 2;
}

std::string boxes::box(std::string header, const std::vector<std::string>& contents, const std::string& bodyColor, const std::string barColor)
{
    if (!header.empty()) header = " " + header + " ";

    u_long max_len = 0;
    for (const auto& line : contents) {
        u_long line_len = visible_length(line);
        if (line_len > max_len) max_len = line_len;
    }

    const u_long content_max = (visible_length(header) > max_len) ? visible_length(header) : max_len;
    const u_long total_inner_width = content_max + PADDING;

    // Ensure minimum header padding if we have a header
    u_long actual_width = total_inner_width;
    if (!header.empty()) {
        const u_long min_header_pad = 3;
        const u_long min_width_for_header = visible_length(header) + (min_header_pad * 2);
        if (min_width_for_header > actual_width) {
            actual_width = min_width_for_header;
        }
    }

    std::string fin;
    if (header.empty()) fin += barColor + boxes::header(actual_width) + RESET;
    else fin += barColor + namedHeader(header, actual_width) + RESET;

    for (const auto& line : contents) {
        fin += bodyColor + spacedContent(line, actual_width) + RESET;
    }

    fin += barColor + footer(actual_width) + RESET;
    return fin;
}

std::string boxes::box(std::string header, const std::vector<std::string>& contents)
{
    if (!header.empty()) header = " " + header + " ";

    u_long max_len = 0;
    for (const auto& line : contents) {
        u_long line_len = visible_length(line);
        if (line_len > max_len) max_len = line_len;
    }

    const u_long content_max = (visible_length(header) > max_len) ? visible_length(header) : max_len;
    const u_long total_inner_width = content_max + PADDING;

    // Ensure minimum header padding if we have a header
    u_long actual_width = total_inner_width;
    if (!header.empty()) {
        const u_long min_header_pad = 3;
        const u_long min_width_for_header = visible_length(header) + (min_header_pad * 2);
        if (min_width_for_header > actual_width) {
            actual_width = min_width_for_header;
        }
    }

    std::string fin;
    if (header.empty()) fin += boxes::header(actual_width);
    else fin += namedHeader(header, actual_width);

    for (const auto& line : contents) {
        fin += spacedContent(line, actual_width);
    }

    fin += footer(actual_width);
    return fin;
}

std::string boxes::box(std::string header, const std::string& body) {
    if (!header.empty()) header = ' ' + header + ' ';
    std::string fin;
    const u_long content_max = visible_length(header) > visible_length(body) ? visible_length(header) : visible_length(body);
    const u_long total_inner_width = content_max + PADDING;

    if (header.empty()) fin += boxes::header(total_inner_width);
    else fin += namedHeader(header, total_inner_width);

    fin += spacedContent(body, total_inner_width);
    fin += footer(total_inner_width);

    return fin;
}

// Helper function to repeat a string
std::string repeat(const std::string& s, const size_t n) {
    std::string res;
    res.reserve(s.length() * n);
    for (size_t i = 0; i < n; ++i) res += s;
    return res;
}

std::string boxes::spacedContent(const std::string& toSpace, const u_long size) {
    const u_long visible_len = visible_length(toSpace);
    const u_long left_pad = (size - visible_len) / 2;
    const u_long right_pad = size - visible_len - left_pad;

    return "│" + std::string(left_pad, ' ') + toSpace + std::string(right_pad, ' ') + "│\n";
}

std::string boxes::namedHeader(const std::string& toSpace, const u_long size) {
    const u_long visible_len = visible_length(toSpace);
    const u_long left_pad = (size - visible_len) / 2;
    const u_long right_pad = size - visible_len - left_pad;

    return "╔" + repeat("═", left_pad) + toSpace + repeat("═", right_pad) + "╗\n";
}

std::string boxes::header(const u_long length) {
    return "╔" + repeat("═", length) + "╗\n";
}

std::string boxes::footer(const u_long size) {
    return "╚" + repeat("═", size) + "╝\n";
}

std::string boxes::indent(const std::string& text, const std::string& prefix) {
    std::string result;
    std::string line;

    for (char c : text) {
        line += c;
        if (c == '\n') {
            result += prefix + line;
            line.clear();
        }
    }

    // Don't add prefix to trailing escape codes or empty content
    if (!line.empty()) {
        // Check if line contains only escape sequences (no visible content)
        bool has_visible = false;
        bool in_escape = false;
        for (char c : line) {
            if (c == '\033') {
                in_escape = true;
            } else if (in_escape && c == 'm') {
                in_escape = false;
            } else if (!in_escape && c != ' ' && c != '\t') {
                has_visible = true;
                break;
            }
        }

        // Only add the remaining line if it has visible content,
        // otherwise just append it without prefix
        if (has_visible) {
            result += prefix + line;
        } else {
            result += line;
        }
    }

    return result;
}