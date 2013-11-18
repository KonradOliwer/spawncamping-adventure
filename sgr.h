#ifndef SGR_H
#define SGR_H

// Select graphic rendition
std::string sgr(const char *spec = "0")
{
#ifdef COLORS
    return std::string("\033[") + spec + "m";
#else
    return "";
#endif
}

#endif // SGR_H
