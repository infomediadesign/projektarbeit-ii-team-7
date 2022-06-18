use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let font_name = args.first().unwrap().as_str().as_bytes();
    let font = fontdue::Font::from_bytes(font_name, fontdue::FontSettings::default()).unwrap();
}
