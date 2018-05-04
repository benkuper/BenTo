/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   noise_png;
    const int            noise_pngSize = 1839;

    extern const char*   rainbow_png;
    const int            rainbow_pngSize = 4481;

    extern const char*   solidColor_png;
    const int            solidColor_pngSize = 4514;

    extern const char*   strobe_png;
    const int            strobe_pngSize = 5621;

    extern const char*   dmx_png;
    const int            dmx_pngSize = 3170;

    extern const char*   node_png;
    const int            node_pngSize = 3682;

    extern const char*   script_png;
    const int            script_pngSize = 2116;

    extern const char*   timeline_png;
    const int            timeline_pngSize = 1253;

    extern const char*   video_png;
    const int            video_pngSize = 6004;

    extern const char*   default_btlayout;
    const int            default_btlayoutSize = 2467;

    extern const char*   fadeIn_png;
    const int            fadeIn_pngSize = 1868;

    extern const char*   fadeOut_png;
    const int            fadeOut_pngSize = 392;

    extern const char*   icon_png;
    const int            icon_pngSize = 98974;

    extern const char*   shader_frag;
    const int            shader_fragSize = 1012;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 14;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) noexcept;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8) noexcept;
}
