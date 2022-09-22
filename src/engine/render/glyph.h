#ifndef __ENGINE_RENDER_GLYPH_H
#define __ENGINE_RENDER_GLYPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/numeric.h"
#include "../types/vector.h"

typedef struct Glyph {
  Vector4 pos;
  Vector4 color;
  Vector2 uv_offset;
  Vector2 scale;
} Glyph;

typedef struct GlyphText {
  Vector4 pos;
  Vector4 color;
  Vector2 scale;
  char *text;
  Glyph *glyphs;
  u32 size;
} GlyphText;

Glyph *glyph_from_string(const char *str, const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale);
GlyphText *glyph_make_text(const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale);
void glyph_default_text(GlyphText *txt, const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale);
void glyph_set_text(GlyphText *txt, const char *str);
void glyph_clear_text(GlyphText *txt);

#ifdef __cplusplus
}
#endif

#endif
