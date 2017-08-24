#include "GUI/ColorPicker.hpp"

void ColorPicker::draw(nk_context* ctx) {
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_labelf(ctx, NK_TEXT_LEFT, "%s:", name.c_str());
        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_combo_begin_color(ctx, color, nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 20, 1);
                int width = nk_widget_width(ctx);
                nk_labelf(ctx, NK_TEXT_LEFT, "%s: %d", "width:", width);
                nk_layout_row_dynamic(ctx, width, 1);
                color = nk_color_picker(ctx, color, NK_RGB);
                nk_layout_row_dynamic(ctx, 25, 1);
                color.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, color.r, 255, 1,1);
                color.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, color.g, 255, 1,1);
                color.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, color.b, 255, 1,1);
                nk_combo_end(ctx);
        }
}
