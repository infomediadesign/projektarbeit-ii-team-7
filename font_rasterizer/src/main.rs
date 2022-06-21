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
        fontdue::FontSettings::default(),
    )
    .unwrap();

    let chars: Vec<char> = ('A'..'Z').collect();

    println!("{}", chars.len());

    let data: Vec<u8> = chars
        .iter()
        .flat_map(|c| {
            //let (_metrics, bitmap) = font.rasterize(*c, 16.0);
            //println!("{} vs. {}", 16 * 16, bitmap.len());
            //bitmap.iter().flat_map(|b| vec![255, 255, 255, *b]).collect::<Vec<u8>>()
        })
        .collect();

    println!("{}", data.len());

    let path = Path::new(r"font_rasterized_out.png");
    let file = File::create(path).unwrap();
    let ref mut w = BufWriter::new(file);

    let mut encoder = png::Encoder::new(w, 16, 25 * 16);
    encoder.set_color(png::ColorType::Rgba);
    encoder.set_depth(png::BitDepth::Eight);
    encoder.set_trns(vec![0xFFu8, 0xFFu8, 0xFFu8, 0xFFu8]);
    encoder.set_source_gamma(png::ScaledFloat::new(1.0 / 2.2));

    let mut writer = encoder.write_header().unwrap();
    writer.write_image_data(&data).unwrap();
}
