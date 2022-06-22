use std::env;
use std::fs;
use std::fs::File;
use std::io::BufWriter;
use std::path::Path;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        panic!("Font not provided.");
    }

    let font = fontdue::Font::from_bytes(
        fs::read(&args[1]).unwrap().as_slice(),
        fontdue::FontSettings {
            collection_index: 0,
            scale: 16.0,
        },
    )
    .unwrap();

    let mut chars: Vec<char> = (' '..='~').collect();
    chars.push('ü');
    chars.push('Ü');
    chars.push('ä');
    chars.push('Ä');
    chars.push('ö');
    chars.push('Ö');

    let data: Vec<u8> = chars
        .iter()
        .flat_map(|c| {
            let (metrics, bitmap) = font.rasterize(*c, 20.0);
            let offset_x = (16 - metrics.width as i32) / 2;
            let offset_y = (16 - metrics.height as i32) / 2;
            let mut pixels = bitmap.iter().map(|b| *b).rev().collect::<Vec<u8>>();

            let d: Vec<u8> = (0..256)
                .flat_map(|i| {
                    let line: i32 = i / 16;
                    let line_pos = i % 16 + 1;

                    if (offset_y + metrics.height as i32) < line
                        || (offset_x + metrics.width as i32) < line_pos
                        || offset_x >= line_pos
                        || offset_y >= line
                    {
                        vec![0, 0, 0, 0]
                    } else {
                        vec![255, 255, 255, pixels.pop().unwrap_or_else(|| 0)]
                    }
                })
                .collect();
            d
        })
        .collect();

    let path = Path::new(r"font_rasterized_out.png");
    let file = File::create(path).unwrap();
    let ref mut w = BufWriter::new(file);

    let mut encoder = png::Encoder::new(w, 16, chars.len() as u32 * 16);
    encoder.set_color(png::ColorType::Rgba);
    encoder.set_depth(png::BitDepth::Eight);
    encoder.set_trns(vec![0xFFu8, 0xFFu8, 0xFFu8, 0xFFu8]);
    encoder.set_source_gamma(png::ScaledFloat::new(1.0 / 2.2));

    let mut writer = encoder.write_header().unwrap();
    writer.write_image_data(&data).unwrap();
}
