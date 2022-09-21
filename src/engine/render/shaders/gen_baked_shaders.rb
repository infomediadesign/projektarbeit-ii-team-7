def make_shader_fragment(name)
  f = File.open("#{name}.spv")
  data = []

  pretty_name = File.basename(name).gsub! /\./, '_'

  while byte = f.getbyte
    data.push byte
  end

  f.close

  contents = "/* Generated from #{name}.spv */\nconst u8 #{pretty_name}_data[#{data.size}] = {"

  data.each_with_index do |val, idx|
    contents += "\n  " if idx % 10 == 0
    contents += "#{' ' * (3 - val.to_s.length)}#{val}, "
  end

  "#{contents}\n};\nconst u32 #{pretty_name}_data_size = #{data.size}U;".gsub /\s+\n/, "\n"
end

path = ARGV[0] || './'

contents = <<-TEXT
/* Do not modify this file. It has been auto-generated */
#ifndef __ENGINE_RENDER_SHADERS_H
#define __ENGINE_RENDER_SHADERS_H

// clang-format off

#ifdef __cplusplus
extern "C" {
#endif

TEXT

contents += make_shader_fragment "#{path}unlit_generic.frag"
contents += "\n\n"
contents += make_shader_fragment "#{path}unlit_generic.vert"
contents += "\n\n"
contents += make_shader_fragment "#{path}text.frag"
contents += "\n\n"
contents += make_shader_fragment "#{path}text.vert"
contents += "\n\n"
contents += <<-TEXT
#ifdef __cplusplus
}
#endif

// clang-format on

#endif
TEXT

File.write("#{path}shaders.h", contents)
