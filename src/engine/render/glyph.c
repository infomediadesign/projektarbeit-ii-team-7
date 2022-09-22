#include "glyph.h"

#include <string.h>

static const u8 offsets[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
  84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  98, 0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  100, 0,  0,  0,  0,  0,  96, 0,  0,  0,  0,  0,  0,  0,  97, 0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  99, 0,  0,  0,  0,  0,  95, 0,  0,  0,
};

static const u8 sizes[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 2, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 2, 1, 2, 1, 2, 4, 2, 2, 4, 2, 2, 1, 1,
  1, 4, 4, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Glyph *glyph_from_string(const char *str, const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale) {
  Glyph *glyphs = (Glyph *)calloc(size, sizeof(Glyph));

  for (u32 i = 0; i < size; i++) {
    f32 add = 0.0f;

    switch (sizes[(u8)str[i]]) {
    case 1: add = 0.01f; break;
    case 2: add = -0.006f; break;
    case 3: add = 0.04f; break;
    case 4: add = 0.02f; break;
    default: add = 0.0f;
    };

    glyphs[i].pos       = vector_add4(pos, vector_make4(0.1f * scale.x * (f32)i, 0.0f + add * scale.y, 0.0f, 0.0f));
    glyphs[i].color     = color;
    glyphs[i].scale     = scale;
    glyphs[i].uv_offset = vector_make2(0.0f, 1.0f / 101.0f * offsets[(u8)str[i]]);
  }

  return glyphs;
}

GlyphText *glyph_make_text(const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale) {
  GlyphText *txt = (GlyphText *)calloc(1, sizeof(GlyphText));

  txt->text   = (char *)calloc(size, sizeof(char));
  txt->glyphs = (Glyph *)calloc(size, sizeof(Glyph));
  txt->pos    = pos;
  txt->size   = size;
  txt->color  = color;
  txt->scale  = scale;

  return txt;
}

void glyph_default_text(GlyphText *txt, const u32 size, const Vector4 pos, const Vector4 color, const Vector2 scale) {
  if (txt->text != NULL) {
    free(txt->text);
    txt->text = NULL;
  }

  if (txt->glyphs != NULL) {
    free(txt->glyphs);
    txt->glyphs = NULL;
  }

  txt->text   = (char *)calloc(size, sizeof(char));
  txt->glyphs = (Glyph *)calloc(size, sizeof(Glyph));
  txt->pos    = pos;
  txt->size   = size;
  txt->color  = color;
  txt->scale  = scale;
}

void glyph_set_text(GlyphText *txt, const char *str) {
#ifdef _WIN32
  strcpy_s(txt->text, txt->size, str);
#else
  strcpy(txt->text, str);
#endif

  for (u32 i = 0; i < txt->size; i++) {
    if (str[i] == '\0')
      break;

    f32 add = 0.0f;

    switch (sizes[(u8)str[i]]) {
    case 1: add = 0.01f; break;
    case 2: add = -0.006f; break;
    case 3: add = 0.04f; break;
    case 4: add = 0.02f; break;
    default: add = 0.0f;
    };

    txt->glyphs[i].pos =
      vector_add4(txt->pos, vector_make4(0.1f * txt->scale.x * (f32)i, 0.0f + add * txt->scale.y, 0.0f, 0.0f));
    txt->glyphs[i].color     = txt->color;
    txt->glyphs[i].scale     = txt->scale;
    txt->glyphs[i].uv_offset = vector_make2(0.0f, 1.0f / 101.0f * offsets[(u8)str[i]]);
  }
}

void glyph_clear_text(GlyphText *txt) { memset(txt->glyphs, 0, sizeof(Glyph) * txt->size); }
